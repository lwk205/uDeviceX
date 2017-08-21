namespace odstr {
namespace sub {
namespace dev {

/* which neighboring subdomain `p' belongs to? */
static __device__ int box(const Particle *p) {
    enum {X, Y, Z};
    int c;
    int vc[3]; /* vcode */
    const float *r = p->r;
    int   L[3] = {XS, YS, ZS};
    for (c = 0; c < 3; ++c) vc[c] = (2 + (r[c] >= -L[c]/2) + (r[c] >= L[c]/2)) % 3;
    return vc[X] + 3 * (vc[Y] + 3 * vc[Z]);
}

#define DBG

#ifdef DBG
static __device__ void check(float x, int i, int L) {
    if (i < 0 || i >= L) printf("odstr: i = %d (L = %d) from x = %g\n", i, L, x);
}
#else
static __device__ void check(float x, int i, int L) {}
#endif

static __device__ int x2c(float x, int L) {
    int i = (int) floor((double)x + L / 2);
    check(x, i, L);
    return i;
}

__global__ void halo(const Particle *pp, const int n, /**/ int *iidx[], int size[]) {
    int pid, code, entry;
    pid = threadIdx.x + blockDim.x * blockIdx.x;
    if (pid >= n) return;
    const Particle *p = &pp[pid];
    code = box(p);
    if (code > 0) {
        entry = atomicAdd(size + code, 1);
        iidx[code][entry] = pid;
    }
}

__global__ void scan(const int n, const int size[], /**/ int strt[], int size_pin[]) {
    int tid = threadIdx.x;
    int val = 0, cnt = 0;

    if (tid < 27) {
        val = cnt = size[threadIdx.x];
        if (tid > 0) size_pin[tid] = cnt;
    }

    for (int L = 1; L < 32; L <<= 1) val += (tid >= L) * __shfl_up(val, L) ;
    if (tid < 28) strt[tid] = val - cnt;
    if (tid == 26) {
        strt[tid + 1] = val;
        int nbulk = n - val;
        size_pin[0] = nbulk;
    }
}

template <typename T, int STRIDE>
__global__ void pack(const T *data, int *const iidx[], const int send_strt[], /**/ T *buf[]) {
    int gid, slot, tid, idpack;
    int offset, pid, c, d;
    __shared__ int start[28];
    
    gid = threadIdx.x + blockDim.x * blockIdx.x;
    slot = gid / STRIDE;
    tid = threadIdx.x;

    if (tid < 28) start[tid] = send_strt[tid];
    __syncthreads();
    idpack = k_common::fid(start, slot);

    if (slot >= start[27]) return;

    offset = slot - start[idpack];
    pid = __ldg(iidx[idpack] + offset);

    c = gid % STRIDE;
    d = c + STRIDE * offset;
    buf[idpack][d] = data[c + STRIDE * pid];
}

template <typename T, int STRIDE>
__global__ void unpack(T *const recv[], const int strt[], /**/ T *data) {
    const int gid = threadIdx.x + blockDim.x * blockIdx.x;
    const int slot = gid / STRIDE;

    const int tid = threadIdx.x;

    __shared__ int start[28];

    if (tid < 28) start[tid] = strt[tid];
    __syncthreads();
    const int idpack = k_common::fid(start, slot);

    if (slot >= start[27]) return;

    const int offset = slot - start[idpack];
    const int c = gid % STRIDE;
    const int srcid = c + STRIDE * offset;

    data[gid] = recv[idpack][srcid];
}

__global__ void subindex_remote(const int n, const int strt[], /*io*/ float2 *pp, int *counts, /**/ uchar4 *subids) {
    int tid, warpid, laneid, base, nlocal, slot, idpack;
    float2 d0, d1, d2;

    tid    = threadIdx.x;
    warpid = tid / warpSize;
    laneid = tid % warpSize;
    base   = warpSize * warpid + blockDim.x * blockIdx.x;

    if (base >= n) return;
    
    nlocal = min(warpSize, n - base);
    slot   = base + laneid;
    idpack = k_common::fid(strt, slot);
    
    k_read::AOS6f(pp + 3*base, nlocal, d0, d1, d2);
    
    if (laneid < nlocal) {
        int xi, yi, zi, cid, subindex;

        d0.x += XS * ((idpack     + 1) % 3 - 1);
        d0.y += YS * ((idpack / 3 + 1) % 3 - 1);
        d1.x += ZS * ((idpack / 9 + 1) % 3 - 1);

        xi = x2c(d0.x, XS);
        yi = x2c(d0.y, YS);
        zi = x2c(d1.x, ZS);

        cid = xi + XS * (yi + YS * zi);
        subindex = atomicAdd(counts + cid, 1);

        subids[slot] = make_uchar4(xi, yi, zi, subindex);
    }

    k_write::AOS6f(pp + 3*base, nlocal, d0, d1, d2);
}

__global__ void scatter(const bool remote, const uchar4 *subi, const int n, const int *start,
                        /**/ uint *iidx) {
    uint pid = threadIdx.x + blockDim.x * blockIdx.x;
    if (pid >= n) return;
    uchar4 entry = subi[pid];
    int subindex = entry.w;

    if (subindex != 255) {
        int cid = entry.x + XS * (entry.y + YS * entry.z);
        int base = __ldg(start + cid);

        pid |= remote << 31;
        iidx[base + subindex] = pid;
    }
}

static __forceinline__ __device__
void xchg_aos2f(int srclane0, int srclane1, int start, float& s0, float& s1) {
    float t0 = __shfl(s0, srclane0);
    float t1 = __shfl(s1, srclane1);

    s0 = start == 0 ? t0 : t1;
    s1 = start == 0 ? t1 : t0;
    s1 = __shfl_xor(s1, 1);
}

static __forceinline__ __device__
void xchg_aos4f(int srclane0, int srclane1, int start, float3& s0, float3& s1) {
    xchg_aos2f(srclane0, srclane1, start, s0.x, s1.x);
    xchg_aos2f(srclane0, srclane1, start, s0.y, s1.y);
    xchg_aos2f(srclane0, srclane1, start, s0.z, s1.z);
}

__global__ void gather_id(const int *ii_lo, const int *ii_re, int n, const uint *iidx, /**/ int *ii) {
    int spid, data;
    const int pid = threadIdx.x + blockIdx.x * blockDim.x;
    if (pid >= n) return;

    spid = iidx[pid];
    
    const bool remote = (spid >> 31) & 1;
    spid &= ~(1 << 31);
    if (remote) data = ii_re[spid];
    else        data = ii_lo[spid];

    ii[pid] = data;
}

__global__ void gather_pp(const float2  *pp_lo, const float2 *pp_re, int n, const uint *iidx,
                          /**/ float2  *pp, float4  *zip0, ushort4 *zip1) {
    /* pp_lo, pp_re, pp: local, remote and output particles */
    int warpid, tid, base, pid;
    bool valid, remote;
    uint spid;
    float2 d0, d1, d2; /* data */
    int nsrc, src0, src1, start, destbase;
    float3 s0, s1;

    warpid = threadIdx.x / warpSize;
    tid = threadIdx.x % warpSize;

    base = warpSize * warpid + blockDim.x * blockIdx.x;
    pid = base + tid;

    valid = (pid < n);

    if (valid) spid = iidx[pid];

    if (valid) {
        remote = (spid >> 31) & 1;
        spid &= ~(1 << 31);
        if (remote) {
            d0 = __ldg(pp_re + 0 + 3 * spid);
            d1 = __ldg(pp_re + 1 + 3 * spid);
            d2 = __ldg(pp_re + 2 + 3 * spid);
        } else {
            d0 = pp_lo[0 + 3 * spid];
            d1 = pp_lo[1 + 3 * spid];
            d2 = pp_lo[2 + 3 * spid];
        }
    }
    nsrc = min(32, n - base);

    src0 = (32 * ((tid    ) & 0x1) + tid) >> 1;
    src1 = (32 * ((tid + 1) & 0x1) + tid) >> 1;
    start = tid % 2;
    destbase = 2 * base;

    s0 = make_float3(d0.x, d0.y, d1.x);
    s1 = make_float3(d1.y, d2.x, d2.y);

    xchg_aos4f(src0, src1, start, s0, s1);

    if (tid < 2 * nsrc)
        zip0[destbase + tid] = make_float4(s0.x, s0.y, s0.z, 0);

    if (tid + 32 < 2 * nsrc)
        zip0[destbase + tid + 32] = make_float4(s1.x, s1.y, s1.z, 0);

    if (tid < nsrc)
        zip1[base + tid] = make_ushort4(__float2half_rn(d0.x),
                                        __float2half_rn(d0.y),
                                        __float2half_rn(d1.x),
                                        0);
    k_write::AOS6f(pp + 3 * base, nsrc, d0, d1, d2);
}

} // dev
} // sub
} // odstr
