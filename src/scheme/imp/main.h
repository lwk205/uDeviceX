void move(float mass, Particle *pp, Force *ff, int n) {
    KL(dev::update, (k_cnf(n)), (mass, pp, ff, n));
}

void clear_vel(Particle *pp, int n) {
    KL(dev::clear_vel, (k_cnf(n)), (pp, n));
}

void force(float mass, Fparams fpar, int n, const Particle *pp, /**/ Force* ff) {
    KL(dev::force, (k_cnf(n)), (mass, fpar, n, pp, /**/ ff));
}
