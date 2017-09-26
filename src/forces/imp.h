namespace forces {
struct DPDparam { float gamma, a, rnd; };
struct Fo { float *x, *y, *z; }; /* force */

static __device__ bool seteq(int a, int b,   int x, int y) {
    /* true if sets {a, b} and {x, y} are equal */
    return (a == x && b == y) || (a == y && b == x);
}
static __device__ void copy_color(int ka, int kb, int k, /**/ int *ca, int *cb) {
    /* k: master kind: copy color from this kind */
    if      (ka == k) *cb = *ca;
    else if (kb == k) *ca = *cb;
    else assert(0);
}

enum {LJ_NONE, LJ_ONE, LJ_TWO}; /* lj hack */
static __device__ float wrf(const int s, float x) {
    if (s == 0) return x;
    if (s == 1) return sqrtf(x);
    if (s == 2) return sqrtf(sqrtf(x));
    if (s == 3) return sqrtf(sqrtf(sqrtf(x)));
    return powf(x, 1.f/s);
}

static __device__ float cap(float x, float lo, float hi) {
    if      (x > hi) return hi;
    else if (x < lo) return lo;
    else             return x;
}

static const float EPS = 1e-6;
enum {NORM_OK, NORM_BIG, NORM_SMALL};
static __device__ int norm(/*io*/ float *px, float *py, float *pz, /**/ float *pr, float *pinvr) {
    /* normalize r = [x, y, z], sets |r| and 1/|r| if not big */
    float x, y, z, invr, r;
    float r2;
    x = *px; y = *py; z = *pz;

    r2 = x*x + y*y + z*z;
    if      (r2 >= 1 )   return NORM_BIG;
    else if (r2 < EPS) {
        *pr = *px = *py = *pz = 0;
        return NORM_SMALL;
    } else {
        invr = rsqrtf(r2);
        r = r2 * invr;
        x *= invr; y *= invr; z *= invr;
        *px = x; *py = y; *pz = z; *pr = r; *pinvr = invr;
        return NORM_OK;
    }
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

static __device__ void dpd(float x, float y, float z,
                             float vx, float vy, float vz,
                             DPDparam p, int ljkind, /**/ Fo f) {
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
        *f.x = *f.y = *f.z = 0;
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

    *f.x = f0*x;
    *f.y = f0*y;
    *f.z = f0*z;
}

static __device__ void gen0(Pa *A, Pa *B, DPDparam p, int ljkind, /**/ Fo f) {
    dpd(A->x -  B->x,    A->y -  B->y,  A->z -  B->z,
        A->vx - B->vx,   A->vy - B->vy, A->vz - B->vz,
        p, ljkind,
        f);
}

static __device__ void gen1(Pa *A, Pa *B, int ca, int cb, int ljkind, float rnd,
                            /**/ Fo f) {
    /* dispatch on color */
    DPDparam p;
    if         (!multi_solvent) {
        p.gamma = gdpd_b;
        p.a     = adpd_b;
    } else if (ca == BLUE_COLOR && cb == BLUE_COLOR) {
        p.gamma = gdpd_b;
        p.a     = adpd_b;
    } else if (ca == RED_COLOR && cb == RED_COLOR) {
        p.gamma = gdpd_r;
        p.a     = adpd_r;
    } else if (seteq(ca, cb,   BLUE_COLOR, RED_COLOR)) {
        p.gamma = gdpd_br;
        p.a     = adpd_br;
    } else {
        printf("unknown color pair: %ld %ld\n", ca, cb);
        assert(0);
    }
    p.rnd = rnd;
    gen0(A, B, p, ljkind, /**/ f);
}

static __device__ void gen(Pa A, Pa B, float rnd, /**/ float *fx, float *fy, float *fz) {
    /* dispatch on kind and pack force */
    int ljkind; /* call LJ? */
    int ka, kb;
    int ca, cb; /* corrected colors */
    Fo f;
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
        ca = cb = BLUE_COLOR;   ljkind = LJ_ONE;
    } else {
        printf("unknown kind pair: %ld %ld\n", ka, kb);
        assert(0);
    }
    f.x = fx; f.y = fy; f.z = fz;
    gen1(&A, &B, ca, cb, ljkind, rnd, /**/ f);
}

} /* namespace */
