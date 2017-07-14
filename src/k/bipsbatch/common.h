namespace k_bipsbatch {
static __constant__ unsigned int start[27];

static __constant__ SFrag        ssfrag[26];
static __constant__ Frag          ffrag[26];
static __constant__ Rnd            rrnd[26];

struct LPart { /* local particle */
    float x, y, z;
    float vx, vy, vz;
    float *fx, *fy, *fz;
    uint id;
};

struct RPart { /* remote particle */
    float x, y, z;
    float vx, vy, vz;
    uint id;
};

static __device__ void force0(const Rnd rnd, float2 *pp, const Map m, LPart p) {
    float x, y, z;
    float vx, vy, vz;
    float *fx, *fy, *fz;
    uint dpid;

     x = p.x;   y = p.y;   z = p.z;
    vx = p.vx; vy = p.vy; vz = p.vz;
    fx = p.fx; fy = p.fy; fz = p.fz;
    dpid = p.id;
    
    int mask = rnd.mask;
    float seed = rnd.seed;
    float xforce = 0, yforce = 0, zforce = 0;
    for (uint i = threadIdx.x & 1; !endp(m, i); i += 2) {
        uint spid = m2id(m, i);
        float2 s0 = __ldg(pp + 0 + spid * 3);
        float2 s1 = __ldg(pp + 1 + spid * 3);
        float2 s2 = __ldg(pp + 2 + spid * 3);

        uint arg1 = mask ? dpid : spid;
        uint arg2 = mask ? spid : dpid;
        float myrandnr = l::rnd::d::mean0var1uu(seed, arg1, arg2);
        float3 r1 = make_float3(x, y, z), r2 = make_float3(s0.x, s0.y, s1.x);
        float3 v1 = make_float3(vx, vy, vz), v2 = make_float3(s1.y, s2.x, s2.y);
        float3 strength = force(SOLVENT_TYPE, SOLVENT_TYPE, r1, r2, v1, v2, myrandnr);

        xforce += strength.x;
        yforce += strength.y;
        zforce += strength.z;
    }
    atomicAdd(fx, xforce);
    atomicAdd(fy, yforce);
    atomicAdd(fz, zforce);
}


static __device__ void force1(const Frag frag, const Rnd rnd, /**/ LPart p) {
    int dx, dy, dz;
    Map m;
    m = p2map(frag, p.x, p.y, p.z);

    dx = frag.dx; dy = frag.dy; dz = frag.dz;
    p.x -= dx * XS;
    p.y -= dy * YS;
    p.z -= dz * ZS;
    force0(rnd, frag.pp, m, p);
}

static __device__ void i2f(const int *ii, float *f, uint i, /**/ float **fx, float **fy, float **fz) {
    /* local id and index to force */
    f += 3*ii[i];
    *fx = f++; *fy = f++; *fz = f++;
}

static __device__ void p2rv(const float *p, uint i,
                            float  *x, float  *y, float  *z,
                            float *vx, float *vy, float *vz) {
    p += 6*i;
     *x = *(p++);  *y = *(p++);  *z = *(p++);
    *vx = *(p++); *vy = *(p++); *vz = *(p++);
}

static __device__ LPart sfrag2p(const SFrag sfrag, float *ff, uint i) {
    LPart p;
    p2rv(sfrag.pp,     i, /**/ &p.x, &p.y, &p.z,   &p.vx, &p.vy, &p.vz);
    i2f (sfrag.ii, ff, i, /**/ &p.fx, &p.fy, &p.fz);
    p.id = i;
    return p;
}

static __device__ void force2(const SFrag sfrag, const Frag frag, const Rnd rnd, uint i, /**/ float *ff) {
    LPart p;
    p = sfrag2p(sfrag, ff, i);
    force1(frag, rnd, p);
}

static __device__ unsigned int get_hid(const unsigned int a[], const unsigned int i) {
    /* where is `i' in sorted a[27]? */
    unsigned int k1, k3, k9;
    k9 = 9 * ((i >= a[9])           + (i >= a[18]));
    k3 = 3 * ((i >= a[k9 + 3])      + (i >= a[k9 + 6]));
    k1 =      (i >= a[k9 + k3 + 1]) + (i >= a[k9 + k3 + 2]);
    return k9 + k3 + k1;
}

__global__ void force(/**/ float *ff) {
    Frag frag;
    Rnd  rnd;
    SFrag sfrag;
    int gid;
    uint hid; /* halo id */
    uint i; /* particle id */

    gid = (threadIdx.x + blockDim.x * blockIdx.x) >> 1;
    if (gid >= start[26]) return;
    hid = get_hid(start, gid);
    i = gid - start[hid];
    sfrag = ssfrag[hid];
    if (i >= sfrag.n) return;

    frag = ffrag[hid];
    rnd = rrnd[hid];
    force2(sfrag, frag, rnd, i, /**/ ff);
}
}
