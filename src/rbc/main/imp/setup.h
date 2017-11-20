static void setup_edg0(float *rr, adj::Map m, /**/ Edg *edg) {
    int i0, i1, i2;
    float *r0, *r1, *r2;
    float r01[3], r12[3], r20[3];
    float a, b, c, A; /* edges and area */

    i0 = m.i0; i1 = m.i1; i2 = m.i2;

    r0 = &rr[3*i0]; r1 = &rr[3*i1]; r2 = &rr[3*i2];

    diff(r0, r1, /**/ r01);
    diff(r1, r2, /**/ r12);
    diff(r2, r0, /**/ r20);

    a = vabs(r01); b = vabs(r12); c = vabs(r20);
    A = heron(a, b, c);

    edg->a = a; edg->A = A;
}

static void setup_edg1(int md, int nv, adj::Hst *adj, float *rr, /**/
                       Edg *edg, float *ptotArea) {
    int valid, i;
    adj::Map m;
    float totArea;

    totArea = 0;
    for (i = 0; i < md*nv; i++) {
        valid = adj::hst(md, nv, i, adj, /**/ &m);
        if (!valid) continue;
        setup_edg0(rr, m, /**/ &edg[i]);
        totArea += edg[i].A;
    }
    totArea /= 3; /* seen every face three times */

    MSG("totArea: %g", totArea);
    *ptotArea = totArea;
}

static void setup_edg(int md, int nv, adj::Hst *adj, /**/ Edg *dev, float *totArea) {
    float *rr;
    Edg *hst;
    UC(emalloc(md*nv*sizeof(Edg), (void**) &hst));
    UC(emalloc(3*nv*sizeof(float), (void**) &rr));

    evert("rbc.off", nv, /**/ rr);
    setup_edg1(md, nv, adj, rr, /**/ hst, totArea);

    cH2D(dev, hst, md*nv);

    free(hst); free(rr);
}

static void setup_anti(int md, int nv, adj::Hst *adj, /**/ int *dev) {
    int n;
    int *hst;
    n = md*nv;
    UC(emalloc(n*sizeof(int), (void**) &hst));
    
    anti::ini(md, nv, adj, /**/ hst);
    cH2D(dev, hst, n);
    
    free(hst);
}

static void setup0(int md, int nt, int nv, int4 *faces, /**/
                   int *anti, Edg *edg, float *totArea, int *adj0, int *adj1) {
    adj::Hst adj;
    adj::ini(md, nt, nv, faces, /**/ &adj);

    if (RBC_STRESS_FREE) setup_edg(md,  nv, &adj, /**/ edg, totArea);
    if (RBC_RND)         setup_anti(md, nv, &adj, /**/ anti);

    cH2D(adj0, adj.adj0, nv*md); /* TODO */
    cH2D(adj1, adj.adj1, nv*md);

    adj::fin(&adj);
}

static void setup(int md, int nt, int nv, const char *r_templ, /**/
                  int *anti, Edg *edg, float *totArea, int4 *faces, int4 *tri,
                  int *adj0, int *adj1) {
    efaces(r_templ, nt, /**/ faces);
    setup0(md, nt, nv, faces, /**/ anti, edg, totArea, adj0, adj1);
    cH2D(tri, faces, nt);
}
