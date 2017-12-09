/* domain sizes */

static __device__ int xdomain(const Coords c) {
    return XS * c.xd;
}

static __device__ int ydomain(const Coords c) {
    return YS * c.yd;
}

static __device__ int zdomain(const Coords c) {
    return ZS * c.zd;
}

/* [l]ocal to [c]enter */

static __device__ float xl2xc(const Coords c, float xl) {
    return xl + XS * (c.xd - 2.f * c.xc - 1) / 2;
}

static __device__ float yl2yc(const Coords c, float yl) {
    return yl + YS * (c.yd - 2.f * c.yc - 1) / 2;
}

static __device__ float zl2zc(const Coords c, float zl) {
    return zl + ZS * (c.zd - 2.f * c.zc - 1) / 2;
}

static __device__ void local2center(const Coords c, float3 rl, /**/ float3 *rc) {
    rc->x = xl2xc(c, rl.x);
    rc->y = yl2yc(c, rl.y);
    rc->z = zl2zc(c, rl.z);
}

/* [c]enter to [l]ocal  */

static __device__ float xc2xl(const Coords c, float xc) {
    return xc - XS * (c.xd - 2.f * c.xc - 1) / 2;
}

static __device__ float yc2yl(const Coords c, float yc) {
    return yc - YS * (c.yd - 2.f * c.yc - 1) / 2;
}

static __device__ float zc2zl(const Coords c, float zc) {
    return zc - ZS * (c.zd - 2.f * c.zc - 1) / 2;
}

static __device__ void center2local(Coords c, float3 rc, /**/ float3 *rl) {
    rl->x = xc2xl(c, rc.x);
    rl->y = yc2yl(c, rc.y);
    rl->z = zc2zl(c, rc.z);
}

/* [l]ocal to [g]lobal */

static __device__ float xl2xg(const Coords c, float xl) {
    return (c.xc + 0.5f) * XS + xl;
}

static __device__ float yl2yg(const Coords c, float yl) {
    return (c.yc + 0.5f) * YS + yl;
}

static __device__ float zl2zg(const Coords c, float zl) {
    return (c.zc + 0.5f) * ZS + zl;
}

static __device__ void local2global(const Coords c, float3 rl, /**/ float3 *rg) {
    rg->x = xl2xg(c, rl.x);
    rg->y = yl2yg(c, rl.y);
    rg->z = zl2zg(c, rl.z);
}

/* [g]lobal to [l]ocal */

static __device__ float xg2xl(const Coords c, float xg) {
    return xg - (c.xc + 0.5f) * XS;
}

static __device__ float yg2yl(const Coords c, float yg) {
    return yg - (c.yc + 0.5f) * YS;
}

static __device__ float zg2zl(const Coords c, float zg) {
    return zg - (c.zc + 0.5f) * ZS;
}

static __device__ void global2local(const Coords c, float3 rg, /**/ float3 *rl) {
    rl->x = xg2xl(c, rg.x);
    rl->y = yg2yl(c, rg.y);
    rl->z = zg2zl(c, rg.z);
}

