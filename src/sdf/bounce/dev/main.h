enum {
    MAX_RESCUE = 8,
    MAX_NEWTON = 2
};

static __device__ void p2rv(const Particle *pp, int i, /**/ float3 *r, float3 *v) {
    enum {X, Y, Z};
    Particle p = pp[i];    
    r->x = p.r[X];
    r->y = p.r[Y];
    r->z = p.r[Z];

    v->x = p.v[X];
    v->y = p.v[Y];
    v->z = p.v[Z];
}

static __device__ void rv2p(float3 r, float3 v, int i, /**/ Particle *pp) {
    enum {X, Y, Z};
    Particle p = {r.x, r.y, r.z, v.x, v.y, v.z};
    pp[i] = p;
}

static __device__ bool is_small(float f) {return fabs(f) < 1e-6f;}
static __device__ void crop(float *t) {
        if (*t < -dt) *t = -dt;
        if (*t >   0) *t = 0;
}

static __device__ void rescue(const tex3Dca texsdf, float currsdf, /* io */ float3 *r, float3 *v) {
    float sdf0, jump;
    float3 dsdf;
    int l;
    
    dsdf = grad_sdf(texsdf, r);
    sdf0 = currsdf;

    axpy(-sdf0, &dsdf, /**/ r);
        
    for (l = MAX_RESCUE; l >= 1; --l) {
        if (sdf(texsdf, r->x, r->y, r->z) < 0) {
            k_wvel::bounce_vel(r->x, r->y, r->z, &v->x, &v->y, &v->z);
            return;
        }
        jump = 1.1f * sdf0 / (1 << l);
        axpy(-jump, &dsdf, /**/ r);
    }
}

static __device__ void bounce_back_1p(const tex3Dca texsdf, float currsdf, /* io */ float3 *r, float3 *v) {
    float3 r0, rc, rw, dsdf;
    float phi, dphi, t;
    int l;
    r0 = *r;
    axpy(-dt, v, /**/ &r0);

    if (sdf(texsdf, r0.x, r0.y, r0.z) >= 0) {
        rescue(texsdf, currsdf, /* io */ r, v);
        return;
    }

    t = 0;
    for (l = 0; l < MAX_NEWTON; ++l) {
        rc = *r;
        axpy(t, v, /**/ &rc);
        phi = currsdf;
        dsdf = ugrad_sdf(texsdf, &rc);
        dphi = dot<float> (v, &dsdf);

        if (is_small(dphi))
            break;
        
        t -= phi/dphi;
        crop(&t);
    }

    rw = *r;
    axpy(t, v, /**/ &rw);
    k_wvel::bounce_vel(rw.x, rw.y, rw.z, &v->x, &v->y, &v->z);

    *r = rw;
    axpy(-t, v, /**/ r);
    
    if (sdf(texsdf, r->x, r->y, r->z) >= 0)
        *r = r0;    
}

__global__ void bounce_back(const tex3Dca texsdf, int n, /**/ Particle *pp) {
    float s, currsdf;
    float3 r, v;
    int i;
    i = threadIdx.x + blockDim.x * blockIdx.x;
    if (i >= n) return;

    p2rv(pp, i, /**/ &r, &v);

    s = cheap_sdf(texsdf, r.x, r.y, r.z);

    if (s >= -1.7320 * XSIZE_WALLCELLS / XTE) {
        currsdf = sdf(texsdf, r.x, r.y, r.z);
        if (currsdf >= 0) {
            bounce_back_1p(texsdf, currsdf, /*io*/ &r, &v);
            rv2p(r, v, i, /**/ pp);
        }
    }
}
