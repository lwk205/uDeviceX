/* [m]aximumd [d]egree, number of vertices, number of triangles */
#define md ( RBCmd )
#define nv ( RBCnv )
#define nt ( RBCnt )

static void reg(int f, int x, int y,  /**/ int *hx, int *hy) { /* register an edge */
    int j = f*md;
    while (hx[j] != -1) j++;
    hx[j] = x; hy[j] = y;
}

static int nxt(int i, int x, int *hx, int *hy) { /* next */
    i *= md;
    while (hx[i] != x) i++;
    return hy[i];
}

static void gen_a12(int i0, int *hx, int *hy, /**/ int *a1, int *a2) {
    int lo = i0*md, hi = lo + md, mi = hx[lo];
    int i;
    for (i = lo + 1; (i < hi) && (hx[i] != -1); i++)
        if (hx[i] < mi) mi = hx[i]; /* minimum */

    int c = mi, c0;
    i = lo;
    do {
        c     = nxt(i0, c0 = c, hx, hy);
        a1[i] = c0;
        a2[i] = nxt(c, c0, hx, hy);
        i++;
    }  while (c != mi);
}

void setup(const char *r_templ, int *faces, int4 *tri, int *adj0, int *adj1) {
    off::faces(r_templ, faces);

    int   *trs4 = new int  [4 * nt];
    for (int ifa = 0, i0 = 0, i1 = 0; ifa < nt; ifa++) {
        trs4 [i0++] = faces[i1++]; trs4[i0++] = faces[i1++]; trs4[i0++] = faces[i1++];
        trs4 [i0++] = 0;
    }

    cH2D(tri, (int4*) trs4, nt);
    delete[] trs4;

    int hx[nv*md], hy[nv*md], a1[nv*md], a2[nv*md];
    int i;
    for (i = 0; i < nv*md; i++) hx[i] = a1[i] = a2[i] = -1;

    for (int ifa = 0; ifa < nt; ifa++) {
        i = 3*ifa;
        int f0 = faces[i++], f1 = faces[i++], f2 = faces[i++];
        reg(f0, f1, f2,   hx, hy); /* register an edge */
        reg(f1, f2, f0,   hx, hy);
        reg(f2, f0, f1,   hx, hy);
    }
    for (i = 0; i < nv; i++) gen_a12(i, hx, hy, /**/ a1, a2);

    cH2D(adj0, a1, nv*md);
    cH2D(adj1, a2, nv*md);
}

void setup_textures(int4 *tri, Texo<int4> *textri, int *adj0, Texo<int> *texadj0,
                    int *adj1, Texo<int> *texadj1, Particle *pp, Texo<float2> *texvert) {
    TE(texadj0, adj0, nv*md);
    TE(texadj1, adj1, nv*md);
    TE(textri,  tri,  nt);
    TE(texvert, (float2*)pp, 3*MAX_PART_NUM);
}

void forces(int nc, const Texo<float2> texvert, const Texo<int4> textri, const Texo<int> texadj0, const Texo<int> texadj1, Force *ff, float* av) {
    if (nc <= 0) return;

    dim3 avThreads(256, 1);
    dim3 avBlocks(1, nc);

    Dzero(av, 2*nc);
    KL(dev::area_volume, (avBlocks, avThreads), (texvert, textri, av));
    KL(dev::force, (k_cnf(nc*nv*md)), (texvert, texadj0, texadj1, nc, av, (float*)ff));
}

void setup_from_strt(const int id, /**/ Particle *pp, int *nc, int *n, /*w*/ Particle *pp_hst) {
    restart::read_pp("rbc", id, pp_hst, n);
    *nc = *n / nv;
    
    if (*n) cH2D(pp, pp_hst, *n);
}

void strt_dump(const int id, const int n, const Particle *pp, /*w*/ Particle *pp_hst) {
    if (n) cD2H(pp_hst, pp, n);

    restart::write_pp("rbc", id, pp_hst, n);
}

#undef md
#undef nv
#undef nt
