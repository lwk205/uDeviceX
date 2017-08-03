struct ParticlesWrap {
    const Particle *p;
    Force *f;
    int n;
    ParticlesWrap() : p(NULL), f(NULL), n(0) {}
    ParticlesWrap(const Particle *const p, const int n, Force *f)
        : p(p), n(n), f(f) {}
};
