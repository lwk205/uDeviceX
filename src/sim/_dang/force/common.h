void body_force(float driving_force0) {
    if (pushflow)
        KL(dev::body_force, (k_cnf(o::q.n)), (1, o::q.pp, o::ff, o::q.n, driving_force0));

    if (pushsolid && solids0)
        KL(dev::body_force, (k_cnf(s::q.n)), (solid_mass, s::q.pp, s::ff, s::q.n, driving_force0));

    if (pushrbc && rbcs)
        KL(dev::body_force, (k_cnf(r::q.n)), (rbc_mass, r::q.pp, r::ff, r::q.n, driving_force0));
}
  
void forces_rbc() {
    if (rbcs)
        rbc::forces(r::q, r::tt, /**/ r::ff);
}

void clear_forces(Force* ff, int n) {
    if (n) DzeroA(ff, n);
}

void forces_wall() {
    if (o::q.n)            wall::interactions(w::qsdf, w::q, w::t, SOLVENT_TYPE, o::q.pp, o::q.n,   /**/ o::ff);
    if (solids0 && s::q.n) wall::interactions(w::qsdf, w::q, w::t, SOLID_TYPE, s::q.pp, s::q.n, /**/ s::ff);
    if (rbcs && r::q.n)    wall::interactions(w::qsdf, w::q, w::t, SOLID_TYPE, r::q.pp, r::q.n, /**/ r::ff);
}

void forces_cnt(std::vector<ParticlesWrap> *w_r) {
    cnt::build(*w_r); /* build cells */
    cnt::bulk(*w_r);
}

void forces_fsi(SolventWrap *w_s, std::vector<ParticlesWrap> *w_r) {
    fsi::bind(*w_s);
    fsi::bulk(*w_r);
}
