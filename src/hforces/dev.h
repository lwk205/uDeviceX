namespace hforces { namespace dev {

struct Fo { float *x, *y, *z; }; /* force */

static __device__ void pair(const forces::Pa a, const forces::Pa b, float rnd,
                            /**/ float *fx, float *fy, float *fz) {
    forces::Fo f;
    forces::f32f(fx, fy, fz, /**/ &f);
    forces::genf(a, b, rnd, /**/ f);
}

static __device__ float random(int aid, int bid, float seed, int mask) {
    uint a1, a2;
    a1 = mask ? aid : bid;
    a2 = mask ? bid : aid;
    return rnd::mean0var1uu(seed, a1, a2);
}

static __device__ void force0(const Rnd rnd, const Frag bfrag, const Map m, const forces::Pa a, int aid, /**/
                              float *fx, float *fy, float *fz) {
    forces::Pa b;
    int i;
    int bid; /* id of b */
    float x, y, z; /* pair force */

    *fx = *fy = *fz = 0;
    for (i = 0; !endp(m, i); i ++ ) {
        bid = m2id(m, i);
        cloud_get(bfrag.c, bid, /**/ &b);
        pair(a, b, random(aid, bid, rnd.seed, rnd.mask), &x, &y, &z);
        *fx += x; *fy += y; *fz += z;
    }
}

static __device__ void force1(const Rnd rnd, const Frag frag, const Map m, const forces::Pa p, int id, Fo f) {
    float x, y, z; /* force */
    force0(rnd, frag, m, p, id, /**/ &x, &y, &z);
    atomicAdd(f.x, x);
    atomicAdd(f.y, y);
    atomicAdd(f.z, z);
}

static __device__ void force2(const Frag frag, const Rnd rnd, forces::Pa p, int id, /**/ Fo f) {
    float x, y, z;
    Map m;
    forces::p2r3(&p, /**/ &x, &y, &z);
    m = r2map(frag, x, y, z);
    forces::shift(-frag.dx*XS, -frag.dy*YS, -frag.dz*ZS, /**/ &p);
    force1(rnd, frag, m, p, id, /**/ f);
}

static __device__ Fo i2f(const int *ii, float *ff, int i) {
    /* local id and index to force */
    Fo f;
    ff += 3*ii[i];
    f.x = ff++; f.y = ff++; f.z = ff++;
    return f;
}

static __device__ Fo sfrag2f(const SFrag frag, float *ff, int i) {
    return i2f(frag.ii, ff, i);
}

static __device__ void force3(const SFrag afrag, const Frag bfrag, const Rnd rnd, int i, /**/ float *ff) {
    forces::Pa p;
    Fo f;
    cloud_get(afrag.c, i, &p);
    f = sfrag2f(afrag, ff, i);
    force2(bfrag, rnd, p, i, f);
}

__global__ void force(const int27 start, const SFrag26 ssfrag, const Frag26 ffrag, const Rnd26 rrnd, /**/ float *ff) {
    Frag frag;
    Rnd  rnd;
    SFrag sfrag;
    int gid;
    int fid; /* fragment id */
    int i; /* particle id */

    gid = threadIdx.x + blockDim.x * blockIdx.x;
    if (gid >= start.d[26]) return;
    fid = k_common::fid(start.d, gid);
    i = gid - start.d[fid];
    sfrag = ssfrag.d[fid];
    if (i >= sfrag.n) return;

    frag = ffrag.d[fid];
    rnd = rrnd.d[fid];
    force3(sfrag, frag, rnd, i, /**/ ff);
}

}} /* namespace */
