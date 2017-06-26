namespace dev {
/* [m]aximumd [d]egree, number of vertices, number of triangles */
#define md ( RBCmd )
#define nv ( RBCnv )
#define nt ( RBCnt )

/* first and second */
#define fst(t) ( (t).x )
#define scn(t) ( (t).y )

/* particle - float2 union */
union Part {
    float2 f2[3];
    float3 f3[2];
};

/* position - float2 union */
union Pos {
    float2 f2[2];
    struct { float3 r; float dummy; };
};

__device__ void tt2r(float2 t1, float2 t2, /**/ float3 *r) {
    r->x = fst(t1); r->y = scn(t1); r->z = fst(t2);
}

__device__ void ttt2ru(float2 t1, float2 t2, float2 t3, /**/ float3 *r, float3 *u) {
    r->x = fst(t1); r->y = scn(t1); r->z = fst(t2);
    u->x = scn(t2); u->y = fst(t3); u->z = scn(t3);
}

__device__ void tex2Pos(const Texo<float2> texvert, const int i, /**/ Pos *r) {
    r->f2[0] = texvert.fetch(3 * i + 0);
    r->f2[1] = texvert.fetch(3 * i + 1);    
}

__device__ void tex2Part(const Texo<float2> texvert, const int i, /**/ Part *p) {
    p->f2[0] = texvert.fetch(3 * i + 0);
    p->f2[1] = texvert.fetch(3 * i + 1);
    p->f2[2] = texvert.fetch(3 * i + 2);
}

__device__ float3 adj_tris(const Texo<float2> texvert, const Texo<int> texadj0,
                           float2 t0a, float2 t0b, const float *av) {
    int pid, lid, idrbc, offset, neighid, i2, i3;
    float2 t0c;
    float2 t1a, t1b, t1c, t2a, t2b;
    float3 r1, u1, r2, u2, r3, f;
    bool valid;

    pid     = (threadIdx.x + blockDim.x * blockIdx.x) / md;
    neighid = (threadIdx.x + blockDim.x * blockIdx.x) % md;
    lid   = pid % nv;
    idrbc = pid / nv;
    offset = idrbc * nv * 3;
    i2 = texadj0.fetch(neighid + md * lid);
    valid = i2 != -1;

    i3 = texadj0.fetch(((neighid + 1) % md) + md * lid);
    if (i3 == -1 && valid)
    i3 = texadj0.fetch(0 + md * lid);

    if (valid) {
        t0c = texvert.fetch(        pid * 3 + 2);
        t1a = texvert.fetch(offset + i2 * 3 + 0);
        t1b = texvert.fetch(offset + i2 * 3 + 1);
        t1c = texvert.fetch(offset + i2 * 3 + 2);
        t2a = texvert.fetch(offset + i3 * 3 + 0);
        t2b = texvert.fetch(offset + i3 * 3 + 1);

        ttt2ru( t0a, t0b, t0c, &r1, &u1);
        ttt2ru( t1a, t1b, t1c, &r2, &u2);
        tt2r  ( t2a, t2b,      &r3     );

        f  = tri(r1, r2, r3, av[2 * idrbc], av[2 * idrbc + 1]);
        f += visc(r1, r2, u1, u2);
        return f;
    }
    return make_float3(-1.0e10f, -1.0e10f, -1.0e10f);
}

__device__ float3 adj_dihedrals(const Texo<float2> texvert, const Texo<int> texadj0,
                                const Texo<int> texadj1, float2 t0a, float2 t0b) {
    int pid, lid, offset, neighid;
    int i1, i2, i3, i4;
    Pos r1, r2, r3, r4;
    bool valid;

    pid     = (threadIdx.x + blockDim.x * blockIdx.x) / md;
    neighid = (threadIdx.x + blockDim.x * blockIdx.x) % md;

    offset = (pid / nv) * nv;
    lid =     pid % nv;

    float3 r0 = make_float3(fst(t0a), scn(t0a), fst(t0b));

    /*
      r4
      /   \
      r1 --> r2 --> r3
      \   /
      V
      r0

      dihedrals: 0124, 0123
    */

    i1 = texadj0.fetch(neighid + md * lid);
    valid = i1 != -1;

    i2 = texadj0.fetch(((neighid + 1) % md) + md * lid);

    if (i2 == -1 && valid) {
        i2 = texadj0.fetch(0 + md * lid);
        i3 = texadj0.fetch(1 + md * lid);
    } else {
        i3 =
            texadj0.fetch(((neighid + 2) % md) + md * lid);
        if (i3 == -1 && valid) i3 = texadj0.fetch(0 + md * lid);
    }

    i4 = texadj1.fetch(neighid + md * lid);

    if (valid) {
        tex2Pos(texvert, offset + i1, /**/ &r1);
        tex2Pos(texvert, offset + i2, /**/ &r2);
        tex2Pos(texvert, offset + i3, /**/ &r3);
        tex2Pos(texvert, offset + i4, /**/ &r4);

        return dihedral<1>(r0, r2.r, r1.r, r4.r) + dihedral<2>(r1.r, r0, r2.r, r3.r);
    }
    return make_float3(-1.0e10f, -1.0e10f, -1.0e10f);
}

__global__ void force(const Texo<float2> texvert, const Texo<int> texadj0, const Texo<int> texadj1,
                      int nc, const float *__restrict__ av, float *ff) {
    int pid = (threadIdx.x + blockDim.x * blockIdx.x) / md;

    if (pid < nc * nv) {
        Part p0;
        tex2Part(texvert, pid, /**/ &p0);

        /* all triangles and dihedrals adjusting to vertex `pid` */
        float3 f = adj_tris(texvert, texadj0, p0.f2[0], p0.f2[1], av);
        f += adj_dihedrals(texvert, texadj0, texadj1, p0.f2[0], p0.f2[1]);

        if (f.x > -1.0e9f) {
            atomicAdd(&ff[3 * pid + 0], f.x);
            atomicAdd(&ff[3 * pid + 1], f.y);
            atomicAdd(&ff[3 * pid + 2], f.z);
        }
    }
}

__DF__ float3 tex2vec(const Texo<float2> texvert, int id) {
    float2 ta = texvert.fetch(id + 0);
    float2 tb = texvert.fetch(id + 1);
    return make_float3(fst(ta), scn(ta), fst(tb));
}

__DF__ float2 warpReduceSum(float2 val) {
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        fst(val) += __shfl_down(fst(val), offset);
        scn(val) += __shfl_down(scn(val), offset);
    }
    return val;
}

__global__ void area_volume(const Texo<float2> texvert, const Texo<int4> textri, float *totA_V) {
    float2 a_v = make_float2(0.0f, 0.0f);
    int cid = blockIdx.y;

    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < nt;
         i += blockDim.x * gridDim.x) {
        int4 ids = textri.fetch(i);

        float3 r0(tex2vec(texvert, 3 * (ids.x + cid * nv)));
        float3 r1(tex2vec(texvert, 3 * (ids.y + cid * nv)));
        float3 r2(tex2vec(texvert, 3 * (ids.z + cid * nv)));

        fst(a_v) += area0(r0, r1, r2);
        scn(a_v) += volume0(r0, r1, r2);
    }
    a_v = warpReduceSum(a_v);
    if ((threadIdx.x & (warpSize - 1)) == 0) {
        atomicAdd(&totA_V[2 * cid + 0], fst(a_v));
        atomicAdd(&totA_V[2 * cid + 1], scn(a_v));
    }
}

#undef fst
#undef scn

#undef md
#undef nv
#undef nt
} /* namespace k_rbc */
