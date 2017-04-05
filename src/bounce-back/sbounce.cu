#include "sbounce.h"
#include <cassert>

namespace solidbounce {

    enum {X, Y, Z};
        
    // from forward Euler
    _DH_ void rvprev(const float *r1, const float *v1, const float *f0, /**/ float *r0, float *v0)
    {
        for (int c = 0; c < 3; ++c)
        {
            v0[c] = v1[c] - f0[c] * dt;
            r0[c] = r1[c] - v0[c] * dt;
        }
    }
    
    /*
      return true if a root h lies in [0, dt] (output h), false otherwise
      if 2 roots in [0, dt], smallest root h0 is returned (first collision)
    */
    _DH_ bool robust_quadratic_roots(const float a, const float b, const float c, /**/ float *h)
    {
        float D, h0, h1;
        int sgnb;

        sgnb = b > 0 ? 1 : -1;
        D = b*b - 4*a*c;

        if (D < 0) return false;
        
        h0 = (-b - sgnb * sqrt(D)) / (2 * a);
        h1 = c / (a * h0);
        
        if (h0 > h1)
        {
            float htmp = h1;
            h1 = h0; h0 = htmp;
        }

        if (h0 >= 0 && h0 <= dt) {*h = h0; return true;}
        if (h1 >= 0 && h1 <= dt) {*h = h1; return true;}
        
        return false;
    }

#if defined(rsph)

#define shape sphere

    namespace sphere
    {
        #define rsph_bb rsph

        _DH_ bool inside(const float *r)
        {
            return r[X] * r[X] + r[Y] * r[Y] + r[Z] * r[Z] < rsph_bb * rsph_bb;
        }

        _DH_ bool intersect(const float *r0, const float *v0, const float *vcm, const float *om0, /**/ float *h)
        {
            float r0x = r0[X],          r0y = r0[Y],          r0z = r0[Z];
            float v0x = v0[X] - vcm[X], v0y = v0[Y] - vcm[Y], v0z = v0[Z] - vcm[Z];
                        
            const float a = v0x*v0x + v0y*v0y + v0z*v0z;
            
            const float b = 2 * (r0x * v0x + r0y * v0y + r0z * v0z);
            const float c = r0x * r0x + r0y * r0y + r0z * r0z - rsph_bb * rsph_bb;
        
            return robust_quadratic_roots(a, b, c, h);
        }

        _DH_ void rescue(float *r)
        {
            float scale = (rsph_bb + 1e-6) / sqrt(r[X] * r[X] + r[Y] * r[Y] + r[Z] * r[Z]);

            r[X] *= scale;
            r[Y] *= scale;
            r[Z] *= scale;
        }
    }
    
#elif defined(rcyl)

#define shape cylinder
    
    namespace cylinder
    {   
        #define rcyl_bb rcyl

        _DH_ bool inside(const float *r)
        {
            return r[X] * r[X] + r[Y] * r[Y] < rcyl_bb * rcyl_bb;
        }

        /* output h between 0 and dt */
        _DH_ bool intersect(const float *r0, const float *v0, const float *vcm, const float *om0, /**/ float *h)
        {
            float r0x = r0[X],          r0y = r0[Y];
            float v0x = v0[X] - vcm[X], v0y = v0[Y] - vcm[Y];

            const float a = v0x * v0x + v0y * v0y;
            
            const float b = 2 * (r0x * v0x + r0y * v0y);
                        
            const float c = r0x * r0x + r0y * r0y - rcyl_bb * rcyl_bb;

            return robust_quadratic_roots(a, b, c, h);
        }

        _DH_ void rescue(float *r)
        {
            float scale = (rcyl_bb + 1e-6) / sqrt(r[X] * r[X] + r[Y] * r[Y]);

            r[X] *= scale;
            r[Y] *= scale;
        }
    }

#elif defined(a2_ellipse)

#define shape ellipse // "extruded" ellipse x^2/2^ + y^2/b^2 = 1
    
    namespace ellipse
    {
#define a2_bb a2_ellipse 
#define b2_bb b2_ellipse

        _DH_ bool inside(const float *r)
        {
            const float x = r[X];
            const float y = r[Y];
            
            return x*x / a2_bb + y*y / b2_bb < 1;
        }
        
        /* output h between 0 and dt */
        // for now: assume vcm = 0
        _DH_ bool intersect(const float *r0, const float *v0, const float *vcm, const float *om0, /**/ float *h)
        {
            const float r0x = r0[X],          r0y = r0[Y];
            const float v0x = v0[X] - vcm[X], v0y = v0[Y] - vcm[Y];

            const float om0z = -om0[Z];
            
            const float v0x_ = v0x - om0z * (r0y + dt * v0y);
            const float v0y_ = v0y + om0z * (r0x + dt * v0x);

            const float r0x_ = r0x + dt * om0z * (r0y + dt * v0y);
            const float r0y_ = r0y - dt * om0z * (r0x + dt * v0x);
            
            const float a = v0x_*v0x_ / a2_bb + v0y_*v0y_ / b2_bb;
            
            const float b = 2 * (r0x_ * v0x_ / a2_bb + r0y_ * v0y_ / b2_bb);
                        
            const float c = r0x_ * r0x_ / a2_bb + r0y_ * r0y_ / b2_bb - 1;

            return robust_quadratic_roots(a, b, c, h);
        }

        _DH_ void rescue(float *r) // cheap rescue
        {
            float scale = (1 + 1e-6) / sqrt(r[X] * r[X] / a2_bb + r[Y] * r[Y] / b2_bb);
            
            r[X] *= scale;
            r[Y] *= scale;
        }
    }

#elif defined(a2_ellipsoid)

#define shape ellipsoid
    
    namespace ellipsoid
    {
#define a2_bb a2_ellipsoid
#define b2_bb b2_ellipsoid
#define c2_bb c2_ellipsoid

        __DH__ bool inside(const float *r)
        {
            const float x = r[X];
            const float y = r[Y];
            const float z = r[Z];
            
            return x*x / a2_bb + y*y / b2_bb + z*z / c2_bb < 1;
        }

        _DH_ bool intersect(const float *r0, const float *v0, const float *vcm, const float *om0, /**/ float *h)
        {
            const float r0x  = r0[X],          r0   = r0[Y],          r0z  = r0[Z];
            const float v0x  = v0[X] - vcm[X], v0   = v0[Y] - vcm[Y], v0z  = v0[Z] - vcm[Z];
            const float om0x = om0[X],         om0y = om0[Y],         om0z = om0[Z];

            const float r1x = r0x + dt * v0x;
            const float r1y = r0y + dt * v0y;
            const float r1z = r0z + dt * v0z;
            
            const float v0x_ = v0x + om0y * r1z - om0z * r1y;
            const float v0y_ = v0y + om0z * r1x - om0x * r1z;
            const float v0z_ = v0z + om0x * r1y - om0y * r1x;

            const float r0x_ = r0x - dt * (om0z * r1z - om0z * r1y);
            const float r0y_ = r0y - dt * (om0z * r1x - om0x * r1z);
            const float r0z_ = r0z - dt * (om0x * r1y - om0y * r1x);
                
            
            const float a = v0x_*v0x_ / a2_bb + v0y_*v0y_ / b2_bb + v0z_*v0z_ / c2_bb;
            
            const float b = 2 * (r0x_*v0x_ / a2_bb + r0y_*v0y_ / b2_bb + r0z_*v0z_ / c2_bb);
                        
            const float c = r0x_*r0x_ / a2_bb + r0y_*r0y_ / b2_bb + r0z_*r0z_ / c2_bb - 1;

            return robust_quadratic_roots(a, b, c, h);
        }

        _DH_ void rescue(float *r) // cheap rescue
        {
            float scale = (1 + 1e-6) / sqrt(r[X] * r[X] / a2_bb + r[Y] * r[Y] / b2_bb + r[Z] * r[Z] / c2_bb);

            r[X] *= scale;
            r[Y] *= scale;
            r[Z] *= scale;
        }
    }
    
#else

#define shape none
    namespace none
    {
        _DH_ bool inside(const float *r)
        {
            printf("solidbounce: not implemented\n");
            exit(1);

            return false;
        }

        _DH_ bool intersect(const float *r0, const float *v0, const float *vcm, const float *om0, /**/ float *h)
        {
            printf("solidbounce: not implemented\n");
            exit(1);

            return false;
        }

        _DH_ void rescue(float *r)
        {
            printf("solidbounce: not implemented\n");
            exit(1);
        }
    }
    
#endif

    _DH_ bool inside_prev(const float *r, const float *om0)
    {
        float rl[3] = {float(r[X] + dt * (om0[Y] * r[Z] - om0[Z] * r[Y])),
                       float(r[Y] + dt * (om0[Z] * r[X] - om0[X] * r[Z])),
                       float(r[Z] + dt * (om0[X] * r[Y] - om0[Y] * r[X]))};
        
        return shape::inside(rl);
    }

    
    _DH_ void collision_point(const float *r0, const float *v0, const float h, /**/ float *rcol)
    {
        for (int c = 0; c < 3; ++c)
        rcol[c] = r0[c] + h * v0[c];
    }

    _DH_ void vsolid(const float *vcm, const float *om, const float *r, /**/ float *vs)
    {
        vs[X] = vcm[X] + om[Y]*r[Z] - om[Z]*r[Y];
        vs[Y] = vcm[Y] + om[Z]*r[X] - om[X]*r[Z];
        vs[Z] = vcm[Z] + om[X]*r[Y] - om[Y]*r[X];
    }

    _DH_ void bounce_particle(const float *vs, const float *rcol, const float *v0, const float h, /**/ float *rn, float *vn)
    {
        assert(h >= 0);
        assert(h <= dt);
        
        for (int c = 0; c < 3; ++c)
        {
            vn[c] = 2 * vs[c] - v0[c];
            rn[c] = rcol[c] + (dt - h) * vn[c];
        }
    }

    _DH_ void rescue_particle(const float *vcm, const float *om, /**/ float *r, float *v)
    {
        shape::rescue(/**/ r);
        vsolid(vcm, om, r, /**/ v);

        assert(!shape::inside(r));
    }

    _DH_ void lin_mom_solid(const float *v1, const float *vn, /**/ float *dP)
    {
        for (int c = 0; c < 3; ++c)
        dP[c] = -(vn[c] - v1[c]) / dt;
    }

    _DH_ void ang_mom_solid(const float *com, const float *r1, const float *rn, const float *v1, const float *vn, /**/ float *dL)
    {
        const float drn[3] = {rn[X] - com[X], rn[Y] - com[Y], rn[Z] - com[Z]};
        const float dr1[3] = {r1[X] - com[X], r1[Y] - com[Y], r1[Z] - com[Z]};
        
        dL[X] = -(drn[Y] * vn[Z] - drn[Z] * vn[Y] - dr1[Y] * v1[Z] + dr1[Z] * v1[Y]) / dt;
        dL[Y] = -(drn[Z] * vn[X] - drn[X] * vn[Z] - dr1[Z] * v1[X] + dr1[X] * v1[Z]) / dt;
        dL[Z] = -(drn[X] * vn[Y] - drn[Y] * vn[X] - dr1[X] * v1[Y] + dr1[Y] * v1[X]) / dt;
    }

#define debug_output
#ifdef debug_output
    int nrescued, nbounced, still_in, failed, step = 0;
    __device__ int bbinfosdev[5];
    FILE * fdebug;
#endif

    enum BBState
    {
        BB_SUCCESS,
        BB_RESCUED,
        BB_FAILED,
        BB_INSIDE,
        BB_NOBOUNCE
    };
    
    
    _DH_ BBState bb_part_local(const float *fp, const float *vcm, const float *om, /*o*/ Particle *p1, float *rw, float *vw, /*w*/ Particle *p0)
    {
        float h;
        
        if (!shape::inside(p1->r))
        return BB_NOBOUNCE;

        /* previous position and velocity                        */
        /* this step should be dependant on the time scheme only */
        
        rvprev(p1->r, p1->v, fp, /**/ p0->r, p0->v);

        /* rescue particles which were already in the solid   */
        /* put them back on the surface with surface velocity */

        if (inside_prev(p0->r, om))
        {
            rescue_particle(vcm, om, /**/ p1->r, p1->v);
            return BB_RESCUED;
        }
        
        /* find collision point */
        
        if (!shape::intersect(p0->r, p0->v, vcm, om, /**/ &h))
        return BB_FAILED;
        
        assert(h >= 0 );
        assert(h <= dt);
        
        collision_point(p0->r, p0->v, h, /**/ rw);
        
        /* handle collision for particle */
        
        vsolid(vcm, om, rw, /**/ vw);

        bounce_particle(vw, rw, p0->v, h, /**/ p1->r, p1->v);

        if (shape::inside(p1->r))
        return BB_INSIDE;

        return BB_SUCCESS;
    }
    
    _DH_ void r2local (const float *e0, const float *e1, const float *e2, const float *com, const float *rg, /**/ float *rl)
    {
        float x = rg[X] - com[X];
        float y = rg[Y] - com[Y];
        float z = rg[Z] - com[Z];
        
        rl[X] = x*e0[X] + y*e0[Y] + z*e0[Z];
        rl[Y] = x*e1[X] + y*e1[Y] + z*e1[Z];
        rl[Z] = x*e2[X] + y*e2[Y] + z*e2[Z];
    }

    _DH_ void r2global(const float *e0, const float *e1, const float *e2, const float *com, const float *rl, /**/ float *rg)
    {
        rg[X] = com[X] + rl[X] * e0[X] + rl[Y] * e1[X] + rl[Z] * e2[X];
        rg[Y] = com[Y] + rl[X] * e0[Y] + rl[Y] * e1[Y] + rl[Z] * e2[Y];
        rg[Z] = com[Z] + rl[X] * e0[Z] + rl[Y] * e1[Z] + rl[Z] * e2[Z];
    }

    _DH_ void v2local (const float *e0, const float *e1, const float *e2, const float *vg, /**/ float *vl)
    {
        vl[X] = vg[X]*e0[X] + vg[Y]*e0[Y] + vg[Z]*e0[Z];
        vl[Y] = vg[X]*e1[X] + vg[Y]*e1[Y] + vg[Z]*e1[Z];
        vl[Z] = vg[X]*e2[X] + vg[Y]*e2[Y] + vg[Z]*e2[Z];
    }

    _DH_ void v2global(const float *e0, const float *e1, const float *e2, const float *vl, /**/ float *vg)
    {
        vg[X] = vl[X] * e0[X] + vl[Y] * e1[X] + vl[Z] * e2[X];
        vg[Y] = vl[X] * e0[Y] + vl[Y] * e1[Y] + vl[Z] * e2[Y];
        vg[Z] = vl[X] * e0[Z] + vl[Y] * e1[Z] + vl[Z] * e2[Z];
    }
    
    void bounce(const Force *ff, const int np, /**/ Particle *pp, Solid *shst)
    {
        Particle p0l, p1, pn, pnl;
        float dP[3], dL[3], vcml[3], oml[3], fl[3], rw[3], vw[3];

#ifdef debug_output
        fdebug = fopen("debug.txt", "a");

        if (step % steps_per_dump == 0)
        nbounced = nrescued = still_in = failed = 0;
#endif
        
        for (int ip = 0; ip < np; ++ip)
        {
            p1 = pp[ip];
            pn = p1;

            r2local(shst->e0, shst->e1, shst->e2, shst->com, pn.r, /**/ pnl.r);
            v2local(shst->e0, shst->e1, shst->e2,            pn.v, /**/ pnl.v);
                
            v2local(shst->e0, shst->e1, shst->e2,  shst->v, /**/ vcml);
            v2local(shst->e0, shst->e1, shst->e2, shst->om, /**/  oml);
                
            v2local(shst->e0, shst->e1, shst->e2, ff[ip].f, /**/ fl);

#ifdef debug_output
            Particle p1l = pnl;
#endif
            
            BBState bbstate = bb_part_local(fl, vcml, oml, /*o*/ &pnl, rw, vw, /*w*/ &p0l);

#ifdef debug_output
            if (bbstate != BB_NOBOUNCE)
            {
#define db(...) fprintf (fdebug, __VA_ARGS__)
                db("%+.10e %+.10e %+.10e %+.10e %+.10e %+.10e ", p0l.r[X], p0l.r[Y], p0l.r[Z], p0l.v[X], p0l.v[Y], p0l.v[Z]);
                db("%+.10e %+.10e %+.10e %+.10e %+.10e %+.10e ", p1l.r[X], p1l.r[Y], p1l.r[Z], p1l.v[X], p1l.v[Y], p1l.v[Z]);
                db("%+.10e %+.10e %+.10e %+.10e %+.10e %+.10e ", rw[X], rw[Y], rw[Z], vw[X], vw[Y], vw[Z]);
                db("%+.10e %+.10e %+.10e %+.10e %+.10e %+.10e ", pnl.r[X], pnl.r[Y], pnl.r[Z], pnl.v[X], pnl.v[Y], pnl.v[Z]);

                switch (bbstate)
                {
                case BB_SUCCESS:
                    ++nbounced; db(":success:\n");
                    break;
                case BB_RESCUED:
                    ++nrescued; db(":rescued:\n");
                    break;
                case BB_INSIDE:
                    ++still_in; db(":inside:\n");
                    break;
                case BB_FAILED:
                    ++failed;   db(":failed:\n");
                    break;
                }
            }
#endif

            
            r2global(shst->e0, shst->e1, shst->e2, shst->com, pnl.r, /**/ pn.r);
            v2global(shst->e0, shst->e1, shst->e2,            pnl.v, /**/ pn.v); 
            
            /* transfer momentum */
            
            dP[X] = dP[Y] = dP[Z] = 0;
            dL[X] = dL[Y] = dL[Z] = 0;
                
            lin_mom_solid(p1.v, pn.v, /**/ dP);
                
            ang_mom_solid(shst->com, p1.r, pn.r, p1.v, pn.v, /**/ dL);
                
            for (int d = 0; d < 3; ++d)
            {
                shst->fo[d] += dP[d];
                shst->to[d] += dL[d];
            }

            pp[ip] = pn;
        }
#ifdef debug_output
        if ((++step) % steps_per_dump == 0)
        printf("%d rescued, %d boounced, %d still in, %d failed\n\n", nrescued, nbounced, still_in, failed);

        fclose(fdebug);
#endif
    }

    __device__ void warpReduceSumf3(float *x)
    {
        for (int offset = warpSize>>1; offset > 0; offset >>= 1)
        {
            x[X] += __shfl_down(x[X], offset);
            x[Y] += __shfl_down(x[Y], offset);
            x[Z] += __shfl_down(x[Z], offset);
        }
    }

    __global__ void bounce_kernel(const Force *ff, const int np, /**/ Particle *pp, Solid *sdev)
    {
        const int pid = threadIdx.x + blockDim.x * blockIdx.x;

        float dP[3] = {0.f, 0.f, 0.f};
        float dL[3] = {0.f, 0.f, 0.f};

        if (pid < np)
        {
            Particle p0l, p1, pn, pnl;
            float vcml[3], oml[3], fl[3], rw[3], vw[3];
            
            p1 = pp[pid];
            pn = p1;

            r2local(sdev->e0, sdev->e1, sdev->e2, sdev->com, pn.r, /**/ pnl.r);
            v2local(sdev->e0, sdev->e1, sdev->e2,            pn.v, /**/ pnl.v);
                
            v2local(sdev->e0, sdev->e1, sdev->e2,  sdev->v, /**/ vcml);
            v2local(sdev->e0, sdev->e1, sdev->e2, sdev->om, /**/  oml);
                
            v2local(sdev->e0, sdev->e1, sdev->e2, ff[pid].f, /**/ fl);
                
            BBState bbstate = bb_part_local(fl, vcml, oml, /*o*/ &pnl, rw, vw, /*w*/ &p0l);

#ifdef debug_output
            if (bbstate != BB_NOBOUNCE) atomicAdd(bbinfosdev + bbstate, 1);
#endif
            
            r2global(sdev->e0, sdev->e1, sdev->e2, sdev->com, pnl.r, /**/ pn.r);
            v2global(sdev->e0, sdev->e1, sdev->e2,            pnl.v, /**/ pn.v); 
                
            /* transfer momentum */

            lin_mom_solid(p1.v, pn.v, /**/ dP);
                
            ang_mom_solid(sdev->com, p1.r, pn.r, p1.v, pn.v, /**/ dL);
            
            pp[pid] = pn;
        }

        /* momentum reduction */
        
        warpReduceSumf3(dP);
        warpReduceSumf3(dL);

        const float normdP = fmaxf(fmaxf(fabsf(dP[X]), fabsf(dP[Y])), fabsf(dP[Z]));
        const float normdL = fmaxf(fmaxf(fabsf(dL[X]), fabsf(dL[Y])), fabsf(dL[Z]));

        const bool warp_contribute = (normdP > 1e-12) && (normdL > 1e-12);
        
        if (warp_contribute && ((threadIdx.x & (warpSize - 1)) == 0))
        {
            atomicAdd(sdev->fo + X, dP[X]);
            atomicAdd(sdev->fo + Y, dP[Y]);
            atomicAdd(sdev->fo + Z, dP[Z]);

            atomicAdd(sdev->to + X, dL[X]);
            atomicAdd(sdev->to + Y, dL[Y]);
            atomicAdd(sdev->to + Z, dL[Z]);
        }
    }

    void bounce_nohost(const Force *ff, const int np, /**/ Particle *pp, Solid *sdev)
    {
#ifdef debug_output
        if (step % steps_per_dump == 0)
        {
            const int zeros[5] = {0, 0, 0, 0, 0};
            CC(cudaMemcpyToSymbol(bbinfosdev, zeros, 5*sizeof(int)));
        }
#endif

        bounce_kernel <<<k_cnf(np)>>> (ff, np, /**/ pp, sdev);

#ifdef debug_output
        if ((++step) % steps_per_dump == 0)
        {
            int bbinfos[5];
            CC(cudaMemcpyFromSymbol(bbinfos, bbinfosdev, 5*sizeof(int)));
            
            printf("%d rescued, %d boounced, %d still in, %d failed\n\n", bbinfos[BB_RESCUED], bbinfos[BB_SUCCESS], bbinfos[BB_INSIDE], bbinfos[BB_FAILED]);
        }
#endif

    }
}
