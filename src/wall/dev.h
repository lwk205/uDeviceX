namespace wall { namespace sub { namespace dev {
__global__ void particle2float4(const Particle *src, const int n, float4 *dst) {
    enum {X, Y, Z};
    int pid = threadIdx.x + blockDim.x * blockIdx.x;
    if (pid >= n) return;
    Particle p = src[pid];
    dst[pid] = make_float4(p.r[X], p.r[Y], p.r[Z], 0);
}

__global__ void float42particle(const float4 *src, const int n, Particle *dst) {
    enum {X, Y, Z};
    int pid = threadIdx.x + blockDim.x * blockIdx.x;
    if (pid >= n) return;
    const float4 r = src[pid];
    Particle p;
    p.r[X] = r.x; p.r[Y] = r.y; p.r[Z] = r.z;
    p.v[X] = p.v[Y] = p.v[Z] = 0;
    dst[pid] = p;
}

__device__ int minmax(int lo, int hi, int a) { return min(hi, max(lo, a)); }

namespace sdfdev = sdf::sub::dev;
typedef const sdf::tex3Dca<float> TexSDF_t;

__global__ void interactions(TexSDF_t texsdf, const float2 *const pp, const int np, const int w_n,
                             float *const ff, const float seed, const int type,
                             const Texo<int> texstart, const Texo<float4> texwpp) {
#define start_fetch(i) (texstart.fetch(i))
#define   wpp_fetch(i) (texwpp.fetch(i))
    forces::Pa a, b;  /* bulk and wall particles */
    float vx, vy, vz; /* wall velocity */
    float fx, fy, fz, rnd;
    int gid, pid, zplane;
    float2 dst0, dst1, dst2;
    uint scan1, scan2, ncandidates, spidbase;
    int deltaspid1, deltaspid2;
    float threshold;

    gid = threadIdx.x + blockDim.x * blockIdx.x;
    pid = gid / 3;
    zplane = gid % 3;

    if (pid >= np) return;

    dst0 = pp[3 * pid + 0];
    dst1 = pp[3 * pid + 1];

    threshold =
        -1 - 1.7320f * ((float)XSIZE_WALLCELLS / (float)XTE);
    if (sdfdev::cheap_sdf(texsdf, dst0.x, dst0.y, dst1.x) <= threshold) return;

    dst2 = pp[3 * pid + 2];

    {
        int xbase = (int)(dst0.x - (-XS / 2 - XWM));
        int ybase = (int)(dst0.y - (-YS / 2 - YWM));
        int zbase = (int)(dst1.x - (-ZS / 2 - ZWM));

        xbase = minmax(-XWM+1, XS + XWM - 2, xbase);
        ybase = minmax(-YWM+1, YS + YWM - 2, ybase);
        zbase = minmax(-ZWM+1, ZS + ZWM - 2, zbase);
                
        enum {
            XCELLS = XS + 2 * XWM,
            YCELLS = YS + 2 * YWM,
            ZCELLS = ZS + 2 * ZWM,
            NCELLS = XCELLS * YCELLS * ZCELLS
        };

        int cid0 = xbase - 1 + XCELLS * (ybase - 1 + YCELLS * (zbase - 1 + zplane));

        spidbase = start_fetch(cid0);
        int count0 = start_fetch(cid0 + 3) - spidbase;

        int cid1 = cid0 + XCELLS;
        deltaspid1 = start_fetch(cid1);
        int count1 = start_fetch(cid1 + 3) - deltaspid1;

        int cid2 = cid0 + XCELLS * 2;
        deltaspid2 = start_fetch(cid2);
        int count2 = cid2 + 3 == NCELLS
            ? w_n
            : start_fetch(cid2 + 3) - deltaspid2;

        scan1 = count0;
        scan2 = count0 + count1;
        ncandidates = scan2 + count2;

        deltaspid1 -= scan1;
        deltaspid2 -= scan2;
    }

    float xforce = 0, yforce = 0, zforce = 0;
    forces::f2k2p(dst0, dst1, dst2, type, /**/ &a);
    for (int i = 0; i < ncandidates; ++i) {
        int m1 = (int)(i >= scan1);
        int m2 = (int)(i >= scan2);
        int spid = i + (m2 ? deltaspid2 : m1 ? deltaspid1 : spidbase);
        const float4 r = wpp_fetch(spid);
        k_wvel::vell(r.x, r.y, r.z, /**/ &vx, &vy, &vz);
        forces::r3v3k2p(r.x, r.y, r.z, vx, vy, vz, WALL_TYPE, /**/ &b);
        rnd = rnd::mean0var1ii(seed, pid, spid);
        forces::gen(a, b, rnd, /**/ &fx, &fy, &fz);
        xforce += fx; yforce += fy; zforce += fz;
    }
    atomicAdd(ff + 3 * pid + 0, xforce);
    atomicAdd(ff + 3 * pid + 1, yforce);
    atomicAdd(ff + 3 * pid + 2, zforce);
#undef start_fetch
#undef wpp_fetch
}

}}} /* namespace */

