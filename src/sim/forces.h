void body_force(float driving_force0) {
    if (pushflow)
        KL(dev::body_force, (k_cnf(o::q.n)), (1, o::q.pp, o::ff, o::q.n, driving_force0));

    if (pushsolid && solids0)
        KL(dev::body_force, (k_cnf(s::q.n)), (solid_mass, s::q.pp, s::ff, s::q.n, driving_force0));

    if (pushrbc && rbcs)
        KL(dev::body_force, (k_cnf(r::q.n)), (rbc_mass, r::q.pp, r::ff, r::q.n, driving_force0));
}
  
void forces_rbc() {
    if (rbcs) rbc::forces(r::q, r::tt, /**/ r::ff);
}

void clear_forces(Force* ff, int n) {
    if (n) CC(cudaMemsetAsync(ff, 0, sizeof(Force) * n));
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

void forces(bool wall0) {
    SolventWrap w_s(o::q.pp, o::q.n, o::ff, o::q.cells->start, o::q.cells->count);
    std::vector<ParticlesWrap> w_r;
    if (solids0) w_r.push_back(ParticlesWrap(s::q.pp, s::q.n, s::ff));
    if (rbcs)    w_r.push_back(ParticlesWrap(r::q.pp, r::q.n, r::ff));
    clear_forces(o::ff, o::q.n);
    if (solids0) clear_forces(s::ff, s::q.n);
    if (rbcs)    clear_forces(r::ff, r::q.n);

    forces_dpd();
    if (wall0) forces_wall();
    forces_rbc();

    if (contactforces) forces_cnt(&w_r);
    if (fsiforces)     forces_fsi(&w_s, &w_r);

    x::rex(w_r); /* fsi::halo(), cnt::halo() */
    dSync();
}
