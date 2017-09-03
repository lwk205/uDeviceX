namespace dev {
static __device__ Pa warp2p(const Particle *pp, int n, int i) {
    Pa p;
    float2 s0, s1, s2;
    pp += i;
    k_read::AOS6f((float2*)pp, n, s0, s1, s2);
    
    p.x = fst(s0);  p.y = scn(s0);  p.z = fst(s1);
    p.vx = scn(s1); p.vy = fst(s2); p.vz = scn(s2);
    return p;
}

static __device__ void halo0(const float *ppB, int nb, float seed, int aid, int dw, int dwe, int nunpack,
                             Particle *pp, Force *ff,
                             /**/ float *ffB) {
    Pa A, B; /* local and remote particles */
    Fo f;
    float *dst = NULL;

    Map m;
    int nzplanes;
    int zplane;
    int i;
    int bid; /* remote particle id */
    float xforce, yforce, zforce;

    A = warp2p(pp, nunpack, dwe);
    dst = (float *)(ff + dwe);

    xforce = yforce = zforce = 0;
    nzplanes = dw < nunpack ? 3 : 0;
    for (zplane = 0; zplane < nzplanes; ++zplane) {
        if (!tex2map(zplane, nb, A.x, A.y, A.z, /**/ &m)) continue;
        for (i = 0; !endp(m, i); ++i) {
            bid = m2id(m, i);
            pp2p(ppB, bid, /**/ &B);
            f = ff2f(ffB, bid);
            pair(A, B, random(aid, bid, seed), /**/ &xforce, &yforce, &zforce,   f);
        }
    }

    k_write::AOS3f(dst, nunpack, xforce, yforce, zforce);
}

static __device__ void halo1(const float *ppB, int nb, float seed, int aid, int ws, int dw, /**/ float *ffB) {
    int fid; /* fragment id */
    int start, count;
    Particle *pp;
    Force *ff;
    int nunpack, dwe;
    fid = k_common::fid(g::starts, ws);
    start = g::starts[fid];
    count = g::counts[fid];
    pp = g::pp[fid];
    ff = g::ff[fid];
    dwe = ws - start;
    nunpack = min(32, count - dwe);
    if (nunpack == 0) return;

    halo0(ppB, nb, seed, aid, dw, dwe, nunpack, pp, ff, /**/ ffB);
}

__global__ void halo(const float *ppB, int n0, int nb, float seed, float *ffB) {
    int dw, warp, ws;
    int i; /* particle id */
    warp = threadIdx.x / warpSize;
    dw = threadIdx.x % warpSize;
    ws = warpSize * warp + blockDim.x * blockIdx.x;
    if (ws >= n0) return;
    i = threadIdx.x + blockDim.x * blockIdx.x;
    halo1(ppB, nb, seed, i, ws, dw, /**/ ffB);
}
}
