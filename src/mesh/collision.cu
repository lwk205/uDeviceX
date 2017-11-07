#include <stdio.h>
#include <assert.h>

#include <conf.h>
#include "inc/conf.h"
#include "inc/def.h"
#include "msg.h"
#include "utils/cc.h"

#include "d/q.h"
#include "d/ker.h"
#include "d/api.h"

#include "inc/type.h"
#include "inc/dev.h"
#include "utils/texo.h"
#include "utils/texo.dev.h"

#include "utils/kl.h"
#include "mesh/collision.h"

namespace collision
{
enum {X, Y, Z};

static __host__ __device__ bool same_side(const float *x, const float *p, const float *a, const float *b, const float *inplane) {
    const float n[3] = {a[Y] * b[Z] - a[Z] * b[Y],
                        a[Z] * b[X] - a[X] * b[Z],
                        a[X] * b[Y] - a[Y] * b[X]};

    const float ndx = n[X] * (x[X] - inplane[X]) + n[Y] * (x[Y] - inplane[Y]) + n[Z] * (x[Z] - inplane[Z]);
    const float ndp = n[X] * (p[X] - inplane[X]) + n[Y] * (p[Y] - inplane[Y]) + n[Z] * (p[Z] - inplane[Z]);
        
    return ndx * ndp > 0;
}
    
static __host__ __device__ bool in_tetrahedron(const float *x, const float *A, const float *B, const float *C, const float *D) {
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
    
int inside_1p(const float *r, const float *vv, const int4 *tt, const int nt) {
    int c = 0;
    float origin[3] = {0, 0, 0};
#ifdef spdir
    origin[spdir] = r[spdir];
#endif
        
    for (int i = 0; i < nt; ++i) {
        int4 t = tt[i];
        if (in_tetrahedron(r, vv + 3*t.x, vv + 3*t.y, vv + 3*t.z, origin)) ++c;
    }
    return c%2;
}

static int inside_1p(const float *r, const Particle *vv, const int4 *tt, const int nt) {
    int c = 0;
    float origin[3] = {0, 0, 0};
#ifdef spdir
    origin[spdir] = r[spdir];
#endif

    for (int i = 0; i < nt; ++i) {
        int4 t = tt[i];
        if (in_tetrahedron(r, vv[t.x].r, vv[t.y].r, vv[t.z].r, origin)) ++c;
    }
    return c%2;
}
    
void inside_hst(const Particle *pp, const int n, int nt, int nv, const int4 *tt, const Particle *i_pp, const int ns, /**/ int *tags) {
    for (int i = 0; i < n; ++i) {
        tags[i] = -1;
        for (int sid = 0; sid < ns; ++sid)
        if (inside_1p(pp[i].r, i_pp + nv * sid, tt, nt)) {
            tags[i] = sid;
            break;
        }
    }
}

namespace kernels
{
__global__ void init_tags(const int n, const int color, /**/ int *tags) {
    const int gid = threadIdx.x + blockIdx.x * blockDim.x;
    if (gid < n) tags[gid] = color;
}

/* assume ns blocks along y */
/* if the ith particle is inside jth mesh, sets tag[i] to j */
__global__ void compute_tags(const Particle *pp, const int n, const Particle *vv, const int nv, const int4 *tt, const int nt, /**/ int *tags) {
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
        int4 t = tt[i];
        const int t1 = sid * nv + t.x;
        const int t2 = sid * nv + t.y;
        const int t3 = sid * nv + t.z;

        const float a[3] = {vv[t1].r[0], vv[t1].r[1], vv[t1].r[2]};
        const float b[3] = {vv[t2].r[0], vv[t2].r[1], vv[t2].r[2]};
        const float c[3] = {vv[t3].r[0], vv[t3].r[1], vv[t3].r[2]};
            
        if (in_tetrahedron(p.r, a, b, c, origin)) ++count;
    }

    // dont consider the case of inside several solids
    if (count % 2) atomicExch(tags + gid, sid);
}

union Pos {
    float2 f2[2];
    struct { float r[3]; float dummy; };
};

__device__ Pos tex2Pos(const Texo<float2> texvert, const int id) {
    Pos r;
    r.f2[0] = fetch(texvert, 3 * id + 0);
    r.f2[1] = fetch(texvert, 3 * id + 1);
    return r;
}

/* assume nm blocks along y */
/* if the ith particle is inside jth mesh, sets tag[i] to IN (see enum in collision.h) */
__global__ void compute_colors_tex(const Particle *pp, const int n, const Texo<float2> texvert, const int nv, const Texo<int4> textri, const int nt, /**/ int *cc) {
    const int sid = blockIdx.y;
    const int gid = threadIdx.x + blockIdx.x * blockDim.x;
    if (gid >= n) return;

    int count = 0;

    const Particle p = pp[gid];
    float origin[3] = {0, 0, 0};
#ifdef spdir
    origin[spdir] = p.r[spdir];
#endif

    int mbase = nv * sid;
    for (int i = 0; i < nt; ++i) {
        const int4 t = fetch(textri, i);

        const Pos a = tex2Pos(texvert, mbase + t.x);
        const Pos b = tex2Pos(texvert, mbase + t.y);
        const Pos c = tex2Pos(texvert, mbase + t.z);
            
        if (in_tetrahedron(p.r, a.r, b.r, c.r, origin)) ++count;
    }

    // dont consider the case of inside several solids
    if (count % 2) atomicExch(cc + gid, IN);
}
}
    
void inside_dev(const Particle *pp, const int n, int nt, int nv, const int4 *tt, const Particle *i_pp, const int ns, /**/ int *tags) {
    if (ns == 0 || n == 0) return;
        
    KL(kernels::init_tags, (k_cnf(n)), (n, -1, /**/ tags));

    enum {THR = 128};
    dim3 thrd(THR, 1);
    dim3 blck(ceiln(n, THR), ns);

    KL(kernels::compute_tags, (blck, thrd), (pp, n, i_pp, nv, tt, nt, /**/ tags));
}

/* 
   n:  number of particles
   nm: number of meshes 
   nt: number of triangles per mesh
   nv: number of vertices per mesh
*/
void get_colors(const Particle *pp, const int n, const Texo<float2> texvert, const Texo<int4> textri, const int nt,
                const int nv, const int nm, /**/ int *cc) {
    if (nm == 0 || n == 0) return;

    KL(kernels::init_tags, (k_cnf(n)), (n, OUT, /**/ cc));

    enum {THR = 128};
    dim3 thrd(THR, 1);
    dim3 blck(ceiln(n, THR), nm);

    KL(kernels::compute_colors_tex, (blck, thrd), (pp, n, texvert, nv, textri, nt, /**/ cc));
}
}
