struct Sdf;
struct Wvel_v;
struct Coords;
struct Particle;

void bounce_back(const Wvel_v *wv, const Coords *c, const Sdf *sdf, int n, /**/ Particle *pp);
