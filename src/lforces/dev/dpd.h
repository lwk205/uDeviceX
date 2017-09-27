static __device__ float3 dpd0(uint aid, uint bid, float rnd) {
    float fx, fy, fz;
    forces::Pa a, b;
    forces::Fo f;

    cloud_get2(aid, &a);
    cloud_get2(bid, &b);
    forces::f32f(&fx, &fy, &fz, /**/ &f);
    forces::gen(a, b, rnd, &fx, &fy, &fz);
    return make_float3(fx, fy, fz);
}

static __device__ float random(uint i, uint j) {
    return rnd::mean0var1ii(info.seed, xmin(i, j), xmax(i, j));
}
static __device__ float3 dpd(uint aid, uint bid, uint dpid, uint spid) {
    float rnd;
    rnd = random(spid, dpid); /* (sic) */
    return dpd0(aid, bid, rnd);
}
