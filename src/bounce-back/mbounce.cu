#ifndef dt
#include "../.conf.h"
#endif
#include "../common.h"

#include <cassert>

#include "roots.h"
#include "mbounce.h"

namespace mbounce
{
    enum {X, Y, Z};

    enum BBState
    {
        BB_SUCCESS,   /* succesfully bounced            */
        BB_NOCROSS,   /* did not cross the plane        */
        BB_WTRIANGLE, /* [w]rong triangle               */
        BB_HFAIL      /* no time solution h             */
    };

#define _DH_ __device__ __host__

#define BBOX_MARGIN 0.1f

#define debug_output
    
    static void rvprev(const float *r1, const float *v1, const float *f0, /**/ float *r0, float *v0)
    {
#ifdef FORWARD_EULER
        for (int c = 0; c < 3; ++c)
        {
            v0[c] = v1[c] - f0[c] * dt;
            r0[c] = r1[c] - v0[c] * dt;
        }
#else // velocity-verlet
        for (int c = 0; c < 3; ++c)
        {
            r0[c] = r1[c] - v1[c] * dt;
            //v0[c] = v1[c] - f0[c] * dt;

            // BB assumes r0 + v0 dt = r1 for now
            v0[c] = v1[c];
        }
#endif
    }
    
    static bool cubic_root(float a, float b, float c, float d, /**/ float *h)
    {
        #define valid(t) ((t) >= 0 && (t) <= dt)
        #define eps 1e-8
        
        if (fabs(a) > eps) // cubic
        {
            const float sc = 1.f / a;
            b *= sc; c *= sc; d *= sc;
            
            float h1, h2, h3;
            int nsol = roots::cubic(b, c, d, &h1, &h2, &h3);

            if (valid(h1))             {*h = h1; return true;}
            if (nsol > 1 && valid(h2)) {*h = h2; return true;}
            if (nsol > 2 && valid(h3)) {*h = h3; return true;}
        }
        else if(fabs(b) > eps) // quadratic
        {
            float h1, h2;
            if (!roots::quadratic(b, c, d, &h1, &h2)) return false;
            if (valid(h1)) {*h = h1; return true;}
            if (valid(h2)) {*h = h2; return true;}
        }
        else if (fabs(c) < eps) // linear
        {
            const float h1 = -d/c;

            if (valid(h1)) {*h = h1; return true;}
        }
        return false;
    }
    
    /* see Fedosov PhD Thesis */
    static BBState intersect_triangle(const float *s10, const float *s20, const float *s30,
                                      const float *vs1, const float *vs2, const float *vs3,
                                      const Particle *p0,  /**/ float *h, float *rw)
    {
#define diff(a, b) {a[X] - b[X], a[Y] - b[Y], a[Z] - b[Z]}
#define cross(a, b) {a[Y] * b[Z] - a[Z] * b[Y], a[Z] * b[X] - a[X] * b[Z], a[X] * b[Y] - a[Y] * b[X]}
#define dot(a, b) (a[X]*b[X] + a[Y]*b[Y] + a[Z]*b[Z])
#define apxb(a, x, b) {a[X] + (float) x * b[X], a[Y] + (float) x * b[Y], a[Z] + (float) x * b[Z]} 
        
        const float *r0 = p0->r;
        const float *v0 = p0->v;
    
        const float a1[3] = diff(s20, s10);
        const float a2[3] = diff(s30, s10);
    
        const float at1[3] = diff(vs2, vs1);
        const float at2[3] = diff(vs3, vs1);

        // n(t) = n + t*nt + t^2 * ntt
        const float n0[3] = cross(a1, a2);
        const float ntt[3] = cross(at1, at2);
        const float nt[3] = {a1[Y] * at2[Z] - a1[Z] * at2[Y]  +  at1[Y] * a2[Z] - at1[Z] * a2[Y],
                             a1[Z] * at2[X] - a1[X] * at2[Z]  +  at1[Z] * a2[X] - at1[X] * a2[Z],
                             a1[X] * at2[Y] - a1[Y] * at2[X]  +  at1[X] * a2[Y] - at1[Y] * a2[X]};
    
        const float dr0[3] = diff(r0, s10);
        
        // check intersection with plane
        {
            const float r1[3] = apxb(r0, dt, v0);
            const float s11[3] = apxb(s10, dt, vs1);

            const float n1[3] = {n0[X] + (float) dt * (nt[X] + (float) dt * ntt[X]),
                                 n0[Y] + (float) dt * (nt[Y] + (float) dt * ntt[Y]),
                                 n0[Z] + (float) dt * (nt[Z] + (float) dt * ntt[Z])};
            
            const float dr1[3] = diff(r1, s11);

            const float b0 = dot(dr0, n0);
            const float b1 = dot(dr1, n1);

            if (b0 * b1 > 0)
            return BB_NOCROSS;
        }

        // find intersection time with plane

        const float dv[3] = diff(v0, vs1);
        
        const float a = dot(ntt, dv);
        const float b = dot(ntt, dr0) + dot(nt, dv);
        const float c = dot(nt, dr0) + dot(n0, dv);
        const float d = dot(n0, dr0);
        
        if (!cubic_root(a, b, c, d, h))
        return BB_HFAIL;

        rw[X] = r0[X] + *h * v0[X];
        rw[Y] = r0[Y] + *h * v0[Y];
        rw[Z] = r0[Z] + *h * v0[Z];

        // check if inside triangle

        {
            const float g[3] = {rw[X] - s10[X] - *h * vs1[X],
                                rw[Y] - s10[Y] - *h * vs1[Y],
                                rw[Z] - s10[Z] - *h * vs1[Z]};

            const float a1_[3] = apxb(a1, *h, at1);
            const float a2_[3] = apxb(a2, *h, at2);
            
            const float ga1 = dot(g, a1_);
            const float ga2 = dot(g, a2_);
            const float a11 = dot(a1_, a1_);
            const float a12 = dot(a1_, a2_);
            const float a22 = dot(a2_, a2_);

            const float fac = 1.f / (a11*a22 - a12*a12);
            
            const float u = (ga1 * a22 - ga2 * a12) * fac;
            const float v = (ga2 * a11 - ga1 * a12) * fac;

            if (!((u >= 0) && (v >= 0) && (u+v <= 1)))
            return BB_WTRIANGLE;
        }

        return BB_SUCCESS;
    }

    static _DH_ void lin_mom_solid(const float *v1, const float *vn, /**/ float *dP)
    {
        for (int c = 0; c < 3; ++c)
        dP[c] = -(vn[c] - v1[c]) / dt;
    }

    static _DH_ void ang_mom_solid(const float *com, const float *rw, const float *v0, const float *vn, /**/ float *dL)
    {
        const float dr[3] = {rw[X] - com[X], rw[Y] - com[Y], rw[Z] - com[Z]};
        
        dL[X] = -(dr[Y] * vn[Z] - dr[Z] * vn[Y] - dr[Y] * v0[Z] + dr[Z] * v0[Y]) / dt;
        dL[Y] = -(dr[Z] * vn[X] - dr[X] * vn[Z] - dr[Z] * v0[X] + dr[X] * v0[Z]) / dt;
        dL[Z] = -(dr[X] * vn[Y] - dr[Y] * vn[X] - dr[X] * v0[Y] + dr[Y] * v0[X]) / dt;
    }

#ifdef debug_output
    int bbstates[4], dstep = 0;
#endif
    
    static void bounce_1s1p(const float *f, const Mesh m, Particle *p, Solid *s)
    {
        float dL[3] = {0}, dP[3] = {0};

        const Particle p1 = *p;
        Particle p0;
        
        rvprev(p1.r, p1.v, f, /**/ p0.r, p0.v);

        for (int it = 0; it < m.nt; ++it)
        {
            const int t1 = m.tt[3*it + 0];
            const int t2 = m.tt[3*it + 1];
            const int t3 = m.tt[3*it + 2];

            float a1[3] = {m.vv[3*t1+0], m.vv[3*t1+1], m.vv[3*t1+2]};
            float a2[3] = {m.vv[3*t2+0], m.vv[3*t2+1], m.vv[3*t2+2]};
            float a3[3] = {m.vv[3*t3+0], m.vv[3*t3+1], m.vv[3*t3+2]};

            get_vl_solid(a1, vcml, oml, /**/ v1);
            get_vl_solid(a2, vcml, oml, /**/ v2);
            get_vl_solid(a3, vcml, oml, /**/ v3);

#define revert(a, v) do {                       \
                a[X] -= v[X] * dt;              \
                a[Y] -= v[Y] * dt;              \
                a[Z] -= v[Z] * dt;              \
            } while(0)

            revert(a1, v1);
            revert(a2, v2);
            revert(a3, v3);

            const BBState bbstate = intersect_triangle(a1, a2, a3, v1, v2, v3, &p0l, /**/ &h, rwl);

#ifdef debug_output
            bbstates[bbstate] ++;
#endif
            if (bbstate == BB_SUCCESS)
            {
                get_vl_solid(rwl, vcml, oml, /**/ vwl);

                pnl.v[X] = 2 * vwl[X] - p0l.v[X];
                pnl.v[Y] = 2 * vwl[Y] - p0l.v[Y];
                pnl.v[Z] = 2 * vwl[Z] - p0l.v[Z];

                pnl.r[X] = rwl[X] + (dt - h) * pnl.v[X];
                pnl.r[Y] = rwl[Y] + (dt - h) * pnl.v[Y];
                pnl.r[Z] = rwl[Z] + (dt - h) * pnl.v[Z];

                r2global(s->e0, s->e1, s->e2, s->com, rwl,   /**/ rw);
                r2global(s->e0, s->e1, s->e2, s->com, pnl.r, /**/ p->r);
                v2global(s->e0, s->e1, s->e2,         pnl.v, /**/ p->v);
                v2global(s->e0, s->e1, s->e2, p0l.v,         /**/ v0);
                
                lin_mom_solid(v0, p->v, /**/ dP);
                ang_mom_solid(s->com, rw, v0, p->v, /**/ dL);
                
                break;
            }
        }

        s->fo[X] += dP[X];
        s->fo[Y] += dP[Y];
        s->fo[Z] += dP[Z];

        s->to[X] += dL[X];
        s->to[Y] += dL[Y];
        s->to[Z] += dL[Z];
    }
    
    static bool in_bbox(const float *rg, const float *com, const float *bbox, const float tol)
    {
        const float rl[3] = {rg[X] - com[X], rg[Y] - com[Y], rg[Z] - com[Z]};
        
        return (rl[X] >= bbox[2*X + 0] - tol) && (rl[X] < bbox[2*X + 1] + tol) &&
            (rl[Y] >= bbox[2*Y + 0] - tol) && (rl[Y] < bbox[2*Y + 1] + tol) &&
            (rl[Z] >= bbox[2*Z + 0] - tol) && (rl[Z] < bbox[2*Z + 1] + tol);
    }
    
    static void bounce_1s(const Force *ff, const int np, const Mesh m, const float *bbox, /**/ Particle *pp, Solid *shst)
    {
        for (int i = 0; i < np; ++i)
        {
            Particle p = pp[i];
            if (in_bbox(p.r, shst->com, bbox, BBOX_MARGIN))
            {
                const Force f = ff[i];
                bounce_1s1p(f.f, m, /**/ &p, shst);
                pp[i] = p;
            }
        }
    }

    void bounce_hst(const Force *ff, const int np, const int ns, const Mesh m, const float *bboxes, /**/ Particle *pp, Solid *shst)
    {
#ifdef debug_output
        if (dstep % steps_per_dump == 0)
        for (int c = 0; c < 4; ++c) bbstates[c] = 0;
#endif

        for (int j = 0; j < ns; ++j)
        {
            Solid *s = shst + j;
            const float *bbox = bboxes + 6 * j;

            bounce_1s(ff, np, m, bbox, /**/ pp, s);
        }

#ifdef debug_output
        if ((++dstep) % steps_per_dump == 0)
        printf("%d success, %d nocross, %d wrong triangle, %d hfailed\n",
               bbstates[0], bbstates[1], bbstates[2], bbstates[3]);
#endif
    }
}
