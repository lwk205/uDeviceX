__global__ void rot_referential(const int ns, Solid *ss) {
    if (threadIdx.x < ns) {
        Solid s = ss[threadIdx.x];
        rot_e(s.om, /**/ s.e0);
        rot_e(s.om, /**/ s.e1);
        rot_e(s.om, /**/ s.e2);
            
        gram_schmidt(/**/ s.e0, s.e1, s.e2);
        ss[threadIdx.x] = s;
    }
}

static __device__ void warpReduceSumf3(float *x) {
    for (int offset = warpSize>>1; offset > 0; offset >>= 1) {
        x[X] += __shfl_down(x[X], offset);
        x[Y] += __shfl_down(x[Y], offset);
        x[Z] += __shfl_down(x[Z], offset);
    }
}

__global__ void add_f_to(const int nps, const Particle *pp, const Force *ff, /**/ Solid *ss) {
    const int gid = blockIdx.x * blockDim.x + threadIdx.x;
    const int sid = blockIdx.y;

    const int i = sid * nps + gid;
        
    Force    f = {0, 0, 0};
    Particle p = {0, 0, 0, 0, 0, 0};

    if (gid < nps) {
        f = ff[i];
        p = pp[i];
    }

    const float dr[3] = {p.r[X] - ss[sid].com[X],
                         p.r[Y] - ss[sid].com[Y],
                         p.r[Z] - ss[sid].com[Z]};
        
    float to[3] = {dr[Y] * f.f[Z] - dr[Z] * f.f[Y],
                   dr[Z] * f.f[X] - dr[X] * f.f[Z],
                   dr[X] * f.f[Y] - dr[Y] * f.f[X]};
        
    warpReduceSumf3(f.f);
    warpReduceSumf3(to);

    if ((threadIdx.x & (warpSize - 1)) == 0) {
        atomicAdd(ss[sid].fo + X, f.f[X]);
        atomicAdd(ss[sid].fo + Y, f.f[Y]);
        atomicAdd(ss[sid].fo + Z, f.f[Z]);

        atomicAdd(ss[sid].to + X, to[X]);
        atomicAdd(ss[sid].to + Y, to[Y]);
        atomicAdd(ss[sid].to + Z, to[Z]);
    }
}

__global__ void reinit_ft(const int nsolid, Solid *ss) {
    const int gid = blockIdx.x * blockDim.x + threadIdx.x;

    if (gid < nsolid) {
        Solid *s = ss + gid;
        s->fo[X] = s->fo[Y] = s->fo[Z] = 0.f;
        s->to[X] = s->to[Y] = s->to[Z] = 0.f;
    }
}

__global__ void update_om_v(const int ns, Solid *ss) {
    if (threadIdx.x < ns) {
        Solid s = ss[threadIdx.x];
        const float *A = s.Iinv, *b = s.to;

        const float dom[3] = {A[XX]*b[X] + A[XY]*b[Y] + A[XZ]*b[Z],
                              A[YX]*b[X] + A[YY]*b[Y] + A[YZ]*b[Z],
                              A[ZX]*b[X] + A[ZY]*b[Y] + A[ZZ]*b[Z]};
            
        s.om[X] += dom[X]*dt;
        s.om[Y] += dom[Y]*dt;
        s.om[Z] += dom[Z]*dt;

        // assume always rotating around z axis
        if (pin_axis) {
            s.om[X] = s.om[Y] = 0.f;
        }

        if (pin_com) {
            s.v[X] = s.v[Y] = s.v[Z] = 0.f;
        } else {
            const float sc = dt/s.mass;
                
            s.v[X] += s.fo[X] * sc;
            s.v[Y] += s.fo[Y] * sc;
            s.v[Z] += s.fo[Z] * sc;
        }

        ss[threadIdx.x] = s;
    }
}

__global__ void compute_velocity(const Solid *ss, const int ns, const int nps, /**/ Particle *pp) {
    const int pid = threadIdx.x + blockIdx.x * blockDim.x;
    const int sid = blockIdx.y;

    const int i = sid * nps + pid;
        
    const Solid s = ss[sid];
        
    const float omx = s.om[X], omy = s.om[Y], omz = s.om[Z];
        
    if (pid < nps) {
        float *r0 = pp[i].r, *v0 = pp[i].v;

        const float x = r0[X]-s.com[X];
        const float y = r0[Y]-s.com[Y];
        const float z = r0[Z]-s.com[Z];
            
        v0[X] = s.v[X] + omy*z - omz*y;
        v0[Y] = s.v[Y] + omz*x - omx*z;
        v0[Z] = s.v[Z] + omx*y - omy*x;
    }
}

__global__ void update_com(const int ns, Solid *ss) {
    const int sid = threadIdx.x / 3;
    const int c   = threadIdx.x % 3;

    if (sid < ns)
    ss[sid].com[c] += ss[sid].v[c]*dt;
}

__global__ void update_r(const float *rr0, const int nps, const Solid *ss, const int nsolid, /**/ Particle *pp) {
    const int pid = threadIdx.x + blockIdx.x * blockDim.x;
    const int sid = blockIdx.y;

    const int i = sid * nps + pid;

    const Solid s = ss[sid];
        
    if (pid < nps) {
        float *r0 = pp[i].r;
        const float *ro = &rr0[3*pid];
        const float x = ro[X], y = ro[Y], z = ro[Z];

        r0[X] = s.com[X] + x*s.e0[X] + y*s.e1[X] + z*s.e2[X];
        r0[Y] = s.com[Y] + x*s.e0[Y] + y*s.e1[Y] + z*s.e2[Y];
        r0[Z] = s.com[Z] + x*s.e0[Z] + y*s.e1[Z] + z*s.e2[Z];
    }
}

__global__ void update_mesh(const Solid *ss_dev, const float *vv, const int nv, /**/ Particle *pp) {
    const int sid = blockIdx.y; // solid Id
    const Solid *s = ss_dev + sid;

    const int i = threadIdx.x + blockIdx.x * blockDim.x;
    const int vid = sid * nv + i;
        
    if (i < nv) {
        const float x = vv[3*i + X];
        const float y = vv[3*i + Y];
        const float z = vv[3*i + Z];

        const Particle p0 = pp[vid];
        Particle p;

        p.r[X] = x * s->e0[X] + y * s->e1[X] + z * s->e2[X] + s->com[X];
        p.r[Y] = x * s->e0[Y] + y * s->e1[Y] + z * s->e2[Y] + s->com[Y];
        p.r[Z] = x * s->e0[Z] + y * s->e1[Z] + z * s->e2[Z] + s->com[Z];
                
        p.v[X] = (p.r[X] - p0.r[X]) / dt;
        p.v[Y] = (p.r[Y] - p0.r[Y]) / dt;
        p.v[Z] = (p.r[Z] - p0.r[Z]) / dt;

        pp[vid] = p;
    }
}
