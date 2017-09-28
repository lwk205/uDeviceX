namespace dev {

typedef double real;
typedef double3 real3;

static __device__ bool valid(real t) {return (t >= 0 && t <= dt);}

static __device__ bool cubic_root(real a, real b, real c, real d, /**/ real *h) {
    const real eps = 1e-6;
    real h1, h2, h3, b_, c_, d_;
    
    if (fabs(a) > eps) { // cubic
        b_ = b /= a;
        c_ = c /= a;
        d_ = d /= a;
            
        int nsol = roots::cubic(b_, c_, d_, &h1, &h2, &h3);

        if (valid(h1))             {*h = h1; return true;}
        if (nsol > 1 && valid(h2)) {*h = h2; return true;}
        if (nsol > 2 && valid(h3)) {*h = h3; return true;}
    }
    else if (fabs(b) > eps) { // quadratic
        if (!roots::quadratic(b, c, d, &h1, &h2)) return false;
        if (valid(h1)) {*h = h1; return true;}
        if (valid(h2)) {*h = h2; return true;}
    }
    else if (fabs(c) > eps) { // linear
        h1 = -d/c;
        if (valid(h1)) {*h = h1; return true;}
    }
    
    return false;
}

/* see Fedosov PhD Thesis */
static __device__ BBState intersect_triangle(const real3_t *s10, const real3_t *s20, const real3_t *s30,
                                             const real3_t *vs1, const real3_t *vs2, const real3_t *vs3,
                                             const rPa *p0,  /**/ real_t *h, real_t *u, real_t *v, real_t *s) {
    
    real3 a1, a2, at1, at2, n0, nt, ntt, dr0;

    diff(s20, s10, /**/ &a1);
    diff(s30, s10, /**/ &a2);

    diff(vs2, vs1, /**/ &at1);
    diff(vs3, vs1, /**/ &at2);

    /* n(t) = n + t*nt + t^2 * ntt */
    cross(&a1, &a2, /**/ &n0);
    cross(&at1, &at2, /**/ &ntt);

    nt.x = a1.y * at2.z - a1.z * at2.y  +  at1.y * a2.z - at1.z * a2.y;
    nt.y = a1.z * at2.x - a1.x * at2.z  +  at1.z * a2.x - at1.x * a2.z;
    nt.z = a1.x * at2.y - a1.y * at2.x  +  at1.x * a2.y - at1.y * a2.x;
    
    diff(&p0->r, s10, /**/ &dr0);
        
    /* check intersection with plane */
    {
        real3 r1, s11, n1, dr1;
        real b0, b1;
        
        apxb(&p0->r, dt, &p0->v, /**/ &r1);
        apxb(s10, dt, vs1, /**/ &s11);

        n1.x = n0.x + dt * (nt.x + dt * ntt.x);
        n1.y = n0.y + dt * (nt.y + dt * ntt.y);
        n1.z = n0.z + dt * (nt.z + dt * ntt.z);

        diff(&r1, &s11, /**/ &dr1);
        
        b0 = dot(&dr0, &n0);
        b1 = dot(&dr1, &n1);

        /* sign : which side does the particle belong? */
        *s = b0 > 0 ? 1 : -1;
        
        if (b0 * b1 > 0)
            return BB_NOCROSS;
    }

    /* find intersection time with plane */
    real hl;

    {
        real3 dv;
        real a, b, c, d;

        diff(&p0->v, vs1, /**/ &dv);
        
        a = dot(&ntt, &dv);
        b = dot(&ntt, &dr0) + dot(&nt, &dv);
        c = dot(&nt,  &dr0) + dot(&n0, &dv);
        d = dot(&n0, &dr0);        
        
        if (!cubic_root(a, b, c, d, &hl))
            return BB_HFAIL;
    }

    real3 rwl, g, a1_, a2_;
    real ga1, ga2, a11, a22, a12, fac, ul, vl;

    apxb(&p0->r, hl, &p0->v, /**/ &rwl);

    /* check if inside triangle */
    g.x = rwl.x - s10->x - hl * vs1->x;
    g.y = rwl.y - s10->y - hl * vs1->y;
    g.z = rwl.z - s10->z - hl * vs1->z;

    apxb(&a1, hl, &at1, /**/ &a1_);
    apxb(&a2, hl, &at2, /**/ &a2_);
            
    ga1 = dot(&g, &a1_);
    ga2 = dot(&g, &a2_);
    a11 = dot(&a1_, &a1_);
    a12 = dot(&a1_, &a2_);
    a22 = dot(&a2_, &a2_);

    fac = 1.0 / (a11*a22 - a12*a12);
            
    ul = (ga1 * a22 - ga2 * a12) * fac;
    vl = (ga2 * a11 - ga1 * a12) * fac;

    if ((ul < 0) || (vl < 0) || (ul+vl > 1))
        return BB_WTRIANGLE;

    *h = hl;
    *u = ul;
    *v = vl;
    
    return BB_SUCCESS;
}

} // dev
