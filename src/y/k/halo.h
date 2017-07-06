namespace k_halo {

typedef Sarray<int,  26> int26;
typedef Sarray<int,  27> int27;
typedef Sarray<int*, 26> intp26;
typedef Sarray<Particle*, 26> Particlep26;

static __device__ int get_idpack(const int a[], const int i) {  /* where is `i' in sorted a[27]? */
    int k1, k3, k9;
    k9 = 9 * ((i >= a[9])           + (i >= a[18]));
    k3 = 3 * ((i >= a[k9 + 3])      + (i >= a[k9 + 6]));
    k1 =      (i >= a[k9 + k3 + 1]) + (i >= a[k9 + k3 + 2]);
    return k9 + k3 + k1;
}

/* returns halo box; 0 is a corner of subdomain */
static __device__ void get_box(int i, /**/ int org[3], int ext[3]) {
    /* i, org, ext : halo id, origin, extend */
    int L[3] = {XS, YS, ZS};
    int d[3] = {(i + 2) % 3 - 1, (i / 3 + 2) % 3 - 1, (i / 9 + 2) % 3 - 1};
    int c;
    for (c = 0; c < 3; ++c) {
        org[c] = (d[c] == 1) ? L[c] - 1 : 0;
        ext[c] = (d[c] == 0) ? L[c]     : 1;
    }
}

/* halo to bulk cell id */
static __device__ int h2cid(int hci, const int org[3], const int ext[3]) {
    enum {X, Y, Z};
    int c;
    int srccellpos[3];
    int dstcellpos[3] = {hci % ext[X], (hci / ext[X]) % ext[Y], hci / (ext[X] * ext[Y])};
    for (c = 0; c < 3; ++c) srccellpos[c] = org[c] + dstcellpos[c];
    return srccellpos[X] + XS * (srccellpos[Y] + YS * srccellpos[Z]);
}

__global__ void count(const int27 cellpackstarts, const int *start, const int *count, /**/
                      intp26 fragss, intp26 fragcc) {
    enum {X, Y, Z};
    int gid;
    int hid; /* halo id */
    int nhc; /* number of hallo cells */
    int cid, hci; /* bulk and halo cell ids */
    int org[3], ext[3]; /* halo origin and extend */
    gid = threadIdx.x + blockDim.x * blockIdx.x;
    if (gid >= cellpackstarts.d[26]) return;

    hid = get_idpack(cellpackstarts.d, gid);
    hci = gid - cellpackstarts.d[hid];

    get_box(hid, /**/ org, ext);
    nhc = ext[X] * ext[Y] * ext[Z];

    if (hci < nhc) {
        cid = h2cid(hci, org, ext);
        fragss.d[hid][hci] = start[cid];
        fragcc.d[hid][hci] = count[cid];
    } else if (hci == nhc) {
        fragss.d[hid][hci] = 0;
        fragcc.d[hid][hci] = 0;
    }
}

__global__ void copycells(const int27 cellpackstarts, const intp26 srccells, /**/ intp26 dstcells) {
    int gid = threadIdx.x + blockDim.x * blockIdx.x;

    if (gid >= cellpackstarts.d[26]) return;

    int idpack = get_idpack(cellpackstarts.d, gid);
    int offset = gid - cellpackstarts.d[idpack];

    dstcells.d[idpack][offset] = srccells.d[idpack][offset];
}

template <int NWARPS>
__global__ void scan(const int26 fragn, const intp26 fragcc, /**/ intp26 fragcum) {
    __shared__ int shdata[32];

    int hid = blockIdx.x;
    int *count = fragcc.d[hid];
    int *start = fragcum.d[hid];
    int n = fragn.d[hid];

    int tid = threadIdx.x;
    int laneid = threadIdx.x & 0x1f;
    int warpid = threadIdx.x >> 5;

    int lastval = 0;
    for (int sourcebase = 0; sourcebase < n; sourcebase += 32 * NWARPS) {
        int sourceid = sourcebase + tid;
        int mycount = 0, myscan = 0;
        if (sourceid < n) myscan = mycount = count[sourceid];
        if (tid == 0) myscan += lastval;

        for (int L = 1; L < 32; L <<= 1) {
            int val = __shfl_up(myscan, L);
            if (laneid >= L) myscan += val;
        }

        if (laneid == 31) shdata[warpid] = myscan;
        __syncthreads();
        if (warpid == 0) {
            int gs = 0;
            if (laneid < NWARPS) gs = shdata[tid];
            for (int L = 1; L < 32; L <<= 1) {
                int val = __shfl_up(gs, L);
                if (laneid >= L) gs += val;
            }

            shdata[tid] = gs;
            lastval = __shfl(gs, 31);
        }
        __syncthreads();
        if (warpid) myscan += shdata[warpid - 1];
        __syncthreads();
        if (sourceid < n) start[sourceid] = myscan - mycount;
    }
}
  
__global__ void fill_all(const int27 cellpackstarts, Particle *pp, int *required_bag_size,
                         const intp26 fragss, const intp26 fragcc, const intp26 fragcum,
                         const int26 fragcapacity, intp26 fragindices, Particlep26 fragpp) {
    int gid, hid, hci, tid, src, dst, nsrc, nfloats;
    int i, lpid, dpid, spid, c;
    float2 word;

    gid = (threadIdx.x >> 4) + 2 * blockIdx.x;
    if (gid >= cellpackstarts.d[26]) return;

    hid = get_idpack(cellpackstarts.d, gid);
    hci = gid - cellpackstarts.d[hid];

    tid = threadIdx.x & 0xf;
    src = fragss.d[hid][hci];
    dst = fragcum.d[hid][hci];
    nsrc = min(fragcc.d[hid][hci], fragcapacity.d[hid] - dst);
    nfloats = nsrc * 6;
    for (i = 2 * tid; i < nfloats; i += warpSize) {
        lpid = i / 6;
        c    = i % 6;
        dpid = dst + lpid;
        spid = src + lpid;
        word = *(float2 *)&pp[spid].r[c];
        *(float2 *)&fragpp.d[hid][dpid].r[c] = word;
    }
    for (lpid = tid; lpid < nsrc; lpid += warpSize / 2) {
        dpid = dst + lpid;
        spid = src + lpid;
        fragindices.d[hid][dpid] = spid;
    }
    if (gid + 1 == cellpackstarts.d[hid + 1]) required_bag_size[hid] = dst;
}
}
