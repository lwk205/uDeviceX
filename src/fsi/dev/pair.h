static __device__ void pair0(const Pa a, const Pa b, float rnd, /**/ float *fx, float *fy, float *fz) {
    forces::Fo f;
    forces::gen(a, b, rnd, /**/ &f);
    *fx = f.x; *fy = f.y; *fz = f.z;
}

static __device__ void pair(const Pa a, const Pa b, float rnd, /**/
                            float *fx, float *fy, float *fz,
                            Fo f) {
    /* f[xyz]: local force; Fo f: remote force */
    float x, y, z; /* pair force */
    pair0(a, b, rnd, /**/ &x, &y, &z);
    *fx += x; *fy += y; *fz += z;
    atomicAdd(f.x, -x); atomicAdd(f.y, -y); atomicAdd(f.z, -z);
}
