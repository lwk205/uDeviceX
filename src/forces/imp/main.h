struct DPDparam { float gamma, a, rnd; };
enum {LJ_NONE, LJ_ONE, LJ_TWO}; /* lj hack */
static __device__ float wrf(const int s, float x) {
    if (s == 0) return x;
    if (s == 1) return sqrtf(x);
    if (s == 2) return sqrtf(sqrtf(x));
    if (s == 3) return sqrtf(sqrtf(sqrtf(x)));
    return powf(x, 1.f/s);
}

static __device__ float lj(float invr, float ljsi) {
    float t2, t4, t6, f;
    t2 = ljsi * ljsi * invr * invr;
    t4 = t2 * t2;
    t6 = t4 * t2;
    f = ljepsilon * 24 * invr * t6 * (2 * t6 - 1);
    f = cap(f, 0, 1e4);
    return f;
}

static __device__ void force0(float x, float y, float z,
                              float vx, float vy, float vz,
                              DPDparam p, int ljkind, /**/ Fo *f) {
    float invr, r;
    float wc, wr; /* conservative and random kernels */
    float rm; /* 1 minus r */
    float ev; /* (e dot v) */
    float gamma, sigma;
    float f0;
    float a;
    int vnstat; /* vector normalization status */

    vnstat = norm(/*io*/ &x, &y, &z, /*o*/ &r, &invr);
    if (vnstat == NORM_BIG) {
        f->x = f->y = f->z = 0;
        return;
    }

    rm = max(1 - r, 0.0f);
    wc = rm;
    wr = wrf(S_LEVEL, rm);
    ev = x*vx + y*vy + z*vz;

    gamma = p.gamma;
    a     = p.a;
    sigma = sqrtf(2*gamma*kBT / dt);
    f0  = (-gamma * wr * ev + sigma * p.rnd) * wr;
    f0 +=                                  a * wc;

    if (vnstat == NORM_OK && ljkind != LJ_NONE) {
        const float ljsi = LJ_ONE ? ljsigma : 2 * ljsigma;
        f0 += lj(invr, ljsi);
    }

    f->x = f0*x;
    f->y = f0*y;
    f->z = f0*z;
}

static __device__ void force1(Pa *A, Pa *B, DPDparam p, int ljkind, /**/ Fo *f) {
    force0(A->x -  B->x,    A->y -  B->y,  A->z -  B->z,
           A->vx - B->vx,   A->vy - B->vy, A->vz - B->vz,
           p, ljkind,
           f);
}

static __device__ void fill_g(float g[N_COLOR][N_COLOR]) {
    enum {B = BLUE_COLOR, R = RED_COLOR, S = SOLID_COLOR, W = WALL_COLOR};

    g[B][B] = gdpd_b; g[R][R] = gdpd_r; g[S][S] = gdpd_b;

    g[B][R] = g[R][B] = gdpd_br;
    g[B][S] = g[S][B] = gdpd_b;
    g[B][W] = g[W][B] = gdpd_b;

    g[R][S] = g[S][R] = gdpd_r;
    g[R][W] = g[W][R] = gdpd_r;

    g[S][W] = g[W][S] = gdpd_b;
}

static __device__ void fill_a(float a[N_COLOR][N_COLOR]) {
    enum {B = BLUE_COLOR, R = RED_COLOR, S = SOLID_COLOR, W = WALL_COLOR};

    a[B][B] = adpd_b; a[R][R] = adpd_r; a[S][S] = adpd_b;

    a[B][R] = a[R][B] = adpd_br;
    a[B][S] = a[S][B] = adpd_b;
    a[B][W] = a[W][B] = adpd_b;

    a[R][S] = a[S][R] = adpd_r;
    a[R][W] = a[W][R] = adpd_r;

    a[S][W] = a[W][S] = adpd_b;
}

static __device__ void color2par(int ca, int cb, /**/ DPDparam *p) {
    enum {B = BLUE_COLOR, R = RED_COLOR, S = SOLID_COLOR, W = WALL_COLOR};
    float g[N_COLOR][N_COLOR], a[N_COLOR][N_COLOR];

    fill_g(/**/ g); fill_a(/**/ a);
    if         (!multi_solvent) {
        p->gamma = g[B][B];
        p->a     = a[B][B];
    } else {
        p->gamma = g[ca][cb];
        p->a     = a[ca][cb];
    }
}

static __device__ void force2(Pa *A, Pa *B, int ca, int cb, int ljkind, float rnd,
                              /**/ Fo *f) {
    DPDparam p;
    color2par(ca, cb, &p);
    p.rnd = rnd;
    force1(A, B, p, ljkind, /**/ f);
}

static __device__ void force(Pa A, Pa B, float rnd, /**/ Fo *f) {
    /* dispatch on kind and pack force */
    int ljkind; /* call LJ? */
    int ka, kb;
    int ca, cb; /* corrected colors */
    enum {O = SOLVENT_KIND, S = SOLID_KIND, W = WALL_KIND};
    ka = A.kind; kb = B.kind;
    ca = A.color; cb = B.color;
    ljkind = LJ_NONE;

    if        (ka == O && kb == O) {
        /* no correction */
    } else if (ka == S   && kb == S) {
        ca = cb = BLUE_COLOR;   ljkind = LJ_TWO;
    } else if (seteq(ka, kb,  O, S)) {
        copy_color(ka, kb, O, /**/ &ca, &cb);
    } else if (seteq(ka, kb,  O, W)) {
        copy_color(ka, kb, O, /**/ &ca, &cb);
    } else if (seteq(ka, kb,  S, W)) {
        ca = cb = BLUE_COLOR;
    } else {
        printf("unknown kind pair: %ld %ld\n", ka, kb);
        assert(0);
    }
    force2(&A, &B, ca, cb, ljkind, rnd, /**/ f);
}
