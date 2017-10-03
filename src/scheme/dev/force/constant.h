__global__ void force(float mass, Fparams fpar, int n, const Particle*, /**/ Force *ff) {
    enum {X, Y, Z};
    int pid = threadIdx.x + blockDim.x * blockIdx.x;
    if (pid >= n) return;
    float *f = ff[pid].f;
    f[X] += mass * fpar.a;
    f[Y] += mass * fpar.b;
    f[Z] += mass * fpar.c;
}
