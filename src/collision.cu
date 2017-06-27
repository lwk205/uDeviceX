#include <mpi.h>
#include "common.h"
#include "minmax.h"
#include <conf.h>
#include "conf.common.h"

#include "collision.h"

namespace collision
{
enum {X, Y, Z};
#define _HD_ __host__ __device__

#define dot(x, y) (x[0]*y[0] + x[1]*y[1] + x[2]*y[2])
static void project_t(const float *a, const float *b, const float *c, const float *r, /**/ float *p) {
    const float ab[3] = {b[0]-a[0], b[1]-a[1], b[2]-a[2]};
    const float ac[3] = {c[0]-a[0], c[1]-a[1], c[2]-a[2]};
    const float ar[3] = {r[0]-a[0], r[1]-a[1], r[2]-a[2]};

    float n[3] = {ab[1]*ac[2] - ab[2]*ac[1],
                  ab[2]*ac[0] - ab[0]*ac[2],
                  ab[0]*ac[1] - ab[1]*ac[0]};
    {
        const float s = 1.f / sqrt(dot(n,n));
        n[0] *= s; n[1] *= s; n[2] *= s;
    }
        
    const float arn = dot(ar, n);
    const float g[3] = {ar[0] - arn * n[0],
                        ar[1] - arn * n[1],
                        ar[2] - arn * n[2]};
    
    float u, v;
    {
        const float ga1 = dot(g, ab);
        const float ga2 = dot(g, ac);
        const float a11 = dot(ab, ab);
        const float a12 = dot(ab, ac);
        const float a22 = dot(ac, ac);

        const float fac = 1.f / (a11*a22 - a12*a12);
            
        u = (ga1 * a22 - ga2 * a12) * fac;
        v = (ga2 * a11 - ga1 * a12) * fac;
    }
        
    // project (u,v) onto unit triangle
    
    if ( (v > u - 1) && (v < u + 1) && (v > 1 - u) ) {
        const float a_ = 0.5f * (u + v - 1);
        u -= a_;
        v -= a_;
    }
    else {
        u = max(min(1.f, u), 0.f);
        v = max(min(v, 1.f-u), 0.f);
    }
    
    // compute projected point
    p[0] = a[0] + u * ab[0] + v * ac[0];
    p[1] = a[1] + u * ab[1] + v * ac[1];
    p[2] = a[2] + u * ab[2] + v * ac[2];
}

static float dist_from_triangle(const float *a, const float *b, const float *c, const float *r) {
    float p[3];
    project_t(a, b, c, r, /**/ p);
    //for (int k = 0; k < 3; ++k) p[k] = (a[k] + b[k] + c[k]) / 3.f;
    
    const float dr[3] = {p[0] - r[0], p[1] - r[1], p[2] - r[2]};
    return sqrt(dot(dr, dr));
}
#undef dot

float dist_from_mesh(const Mesh m, const float *r0) {
    float dmin = 1e5f;

    for (int it = 0; it < m.nt; ++it) {
        const int i1 = m.tt[3*it + 0];
        const int i2 = m.tt[3*it + 1];
        const int i3 = m.tt[3*it + 2];

        const float *A = m.vv + 3*i1;
        const float *B = m.vv + 3*i2;
        const float *C = m.vv + 3*i3;

        const float d = dist_from_triangle(A, B, C, r0);

        dmin = min(d, dmin);
    }
    return dmin;
}

static _HD_ bool same_side(const float *x, const float *p, const float *a, const float *b, const float *inplane) {
    const float n[3] = {a[Y] * b[Z] - a[Z] * b[Y],
                        a[Z] * b[X] - a[X] * b[Z],
                        a[X] * b[Y] - a[Y] * b[X]};

    const float ndx = n[X] * (x[X] - inplane[X]) + n[Y] * (x[Y] - inplane[Y]) + n[Z] * (x[Z] - inplane[Z]);
    const float ndp = n[X] * (p[X] - inplane[X]) + n[Y] * (p[Y] - inplane[Y]) + n[Z] * (p[Z] - inplane[Z]);
        
    return ndx * ndp > 0;
}
    
static _HD_ bool in_tetrahedron(const float *x, const float *A, const float *B, const float *C, const float *D) {
    const float AB[3] = {B[X] - A[X], B[Y] - A[Y], B[Z] - A[Z]};
    const float AC[3] = {C[X] - A[X], C[Y] - A[Y], C[Z] - A[Z]};
    const float AD[3] = {D[X] - A[X], D[Y] - A[Y], D[Z] - A[Z]};
    
    const float BC[3] = {C[X] - B[X], C[Y] - B[Y], C[Z] - B[Z]};
    const float BD[3] = {D[X] - B[X], D[Y] - B[Y], D[Z] - B[Z]};

    return
        same_side(x, A, BC, BD, D) &&
        same_side(x, B, AD, AC, D) &&
        same_side(x, C, AB, BD, D) &&
        same_side(x, D, AB, AC, A);
}
    
int inside_1p(const float *r, const float *vv, const int *tt, const int nt) {
    int c = 0;
    float origin[3] = {0, 0, 0};
#ifdef spdir
    origin[spdir] = r[spdir];
#endif
        
    for (int i = 0; i < nt; ++i) {
        const int *t = tt + 3 * i;
        if (in_tetrahedron(r, vv + 3*t[0], vv + 3*t[1], vv + 3*t[2], origin)) ++c;
    }
    return c%2;
}

void inside_hst(const Particle *pp, const int n, const Mesh m, /**/ int *inout) {
    for (int i = 0; i < n; ++i)
    inout[i] = inside_1p(pp[i].r, m.vv, m.tt, m.nt);
}

static int inside_1p(const float *r, const Particle *vv, const int *tt, const int nt) {
    int c = 0;
    float origin[3] = {0, 0, 0};
#ifdef spdir
    origin[spdir] = r[spdir];
#endif

    for (int i = 0; i < nt; ++i) {
        const int *t = tt + 3 * i;
        if (in_tetrahedron(r, vv[t[0]].r, vv[t[1]].r, vv[t[2]].r, origin)) ++c;
    }
    return c%2;
}
    
void inside_hst(const Particle *pp, const int n, const Mesh m, const Particle *i_pp, const int ns, /**/ int *tags) {
    for (int i = 0; i < n; ++i) {
        tags[i] = -1;
        for (int sid = 0; sid < ns; ++sid)
        if (inside_1p(pp[i].r, i_pp + m.nv * sid, m.tt, m.nt)) {
            tags[i] = sid;
            break;
        }
    }
}

namespace kernels
{
__global__ void init_tags(const int n, /**/ int *tags) {
    const int gid = threadIdx.x + blockIdx.x * blockDim.x;
    if (gid < n) tags[gid] = -1;
}

// assume ns blocks along y
__global__ void compute_tags(const Particle *pp, const int n, const Particle *vv, const int nv, const int *tt, const int nt, /**/ int *tags) {
    const int sid = blockIdx.y;
    const int gid = threadIdx.x + blockIdx.x * blockDim.x;
    if (gid >= n) return;

    int count = 0;

    const Particle p = pp[gid];
    float origin[3] = {0, 0, 0};
#ifdef spdir
    origin[spdir] = p.r[spdir];
#endif

        
    for (int i = 0; i < nt; ++i) {
        const int t1 = sid * nv + tt[3*i + 0];
        const int t2 = sid * nv + tt[3*i + 1];
        const int t3 = sid * nv + tt[3*i + 2];

        const float a[3] = {vv[t1].r[0], vv[t1].r[1], vv[t1].r[2]};
        const float b[3] = {vv[t2].r[0], vv[t2].r[1], vv[t2].r[2]};
        const float c[3] = {vv[t3].r[0], vv[t3].r[1], vv[t3].r[2]};
            
        if (in_tetrahedron(p.r, a, b, c, origin)) ++count;
    }

    // dont consider the case of inside several solids
    if (count % 2) atomicExch(tags + gid, sid);
}
}
    
void inside_dev(const Particle *pp, const int n, const Mesh m, const Particle *i_pp, const int ns, /**/ int *tags) {
    if (ns == 0 || n == 0) return;
        
    kernels::init_tags <<< k_cnf(n) >>> (n, /**/ tags);

    dim3 thrd(128, 1);
    dim3 blck((127 + n)/128, ns);

    kernels::compute_tags <<< blck, thrd >>> (pp, n, i_pp, m.nv, m.tt, m.nt, /**/ tags);
}

static void get_bbox(const Particle *pp, const int n, /**/ float3 *minbb, float3 *maxbb) {
    if (n == 0) return;

    const float *r = pp[0].r;

    float3 minb = make_float3(r[0], r[1], r[2]);
    float3 maxb = make_float3(r[0], r[1], r[2]);

    auto min = [](float a, float b) {return a > b ? b : a;};
    auto max = [](float a, float b) {return a > b ? a : b;};
    
    for (int i = 1; i < n; ++i) {
        r = pp[i].r;
        minb.x = min(minb.x, r[0]); maxb.x = max(maxb.x, r[0]);
        minb.y = min(minb.y, r[1]); maxb.y = max(maxb.y, r[1]);
        minb.z = min(minb.z, r[2]); maxb.z = max(maxb.z, r[2]);
    }
    *minbb = minb; *maxbb = maxb;
}

void get_bbox(const float *rr, const int n, /**/ float3 *minbb, float3 *maxbb) {
    if (n == 0) return;

    const float *r = rr;
        
    float3 minb = make_float3(r[0], r[1], r[2]);
    float3 maxb = make_float3(r[0], r[1], r[2]);

    auto min = [](float a, float b) {return a > b ? b : a;};
    auto max = [](float a, float b) {return a > b ? a : b;};

    for (int i = 1; i < n; ++i) {
        r = rr + 3 * i;
        minb.x = min(minb.x, r[0]); maxb.x = max(maxb.x, r[0]);
        minb.y = min(minb.y, r[1]); maxb.y = max(maxb.y, r[1]);
        minb.z = min(minb.z, r[2]); maxb.z = max(maxb.z, r[2]);
    }
}

void get_bboxes_hst(const Particle *pp, const int nps, const int ns, /**/ float3 *minbb, float3 *maxbb) {
    for (int i = 0; i < ns; ++i)
    get_bbox(pp + i*nps, nps, /**/ minbb + i, maxbb + i);
}

void get_bboxes_dev(const Particle *pp, const int nps, const int ns, /**/ float3 *minbb, float3 *maxbb) {
    if (ns == 0) return;
    minmax(pp, nps, ns, /**/ minbb, maxbb);
}
}
