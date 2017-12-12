static __device__ void wvel_cste(WvelPar_d p, Coords c, float3 r, /**/ float3 *v) {
    *v = p.cste.u;
}

static __device__ void wvel_shear(WvelPar_d p, Coords c, float3 r, /**/ float3 *v) {
    float3 rc; // relative to center
    float gdot, d;
    int vdir, gdir;

    gdot = p.shear.gdot;
    vdir = p.shear.vdir;
    gdir = p.shear.gdir;

    local2center(c, r, /**/ &rc);
    
    d = 0;
    if      (gdir == 0) d = rc.x;
    else if (gdir == 1) d = rc.y;
    else if (gdir == 2) d = rc.z;

    v->x = v->y = v->z = 0;
    if      (vdir == 0) v->x = d * gdot;
    else if (vdir == 1) v->y = d * gdot;
    else if (vdir == 2) v->z = d * gdot;
}

/* a hack for hele shaw */
static __device__ void wvel_hs(WvelPar_d p, Coords c, float3 r, /**/ float3 *v) {
    float3 rc; // relative to center
    float u, h, r2inv, hfac;

    u = p.hs.u;
    h = p.hs.h;

    local2center(c, r, /**/ &rc);

    r2inv = 1.f / (rc.x*rc.x + rc.y*rc.y);
    hfac = u * (1.f - rc.z * rc.z * 4 / (h*h));
    hfac = max(0.f, hfac);

    v->x = r2inv * hfac * rc.x;
    v->y = r2inv * hfac * rc.y;
    v->z = 0;
}



/* device interface */

static __device__ void wvel(Wvel_d wv, Coords c, float3 r, /**/ float3 *v) {
    int type;
    wvel_fun wvel_funs[] = {&wvel_cste, &wvel_shear, &wvel_hs};
    type = wv.type;
    wvel_funs[type](wv.p, c, r, /**/ v);
}

static __device__ void bounce_vel(Wvel_d wv, Coords c, float3 rw, /* io */ float3* v) {
    float3 vw;
    wvel(wv, c, rw, /**/ &vw);
    v->x = 2 * vw.x - v->x;
    v->y = 2 * vw.y - v->y;
    v->z = 2 * vw.z - v->z;
}
