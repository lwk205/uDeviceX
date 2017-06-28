void update_solid() {
    if (s::q.n) update_solid0();
}

void update_solvent() {
    if (o::q.n) dev::update<<<k_cnf(o::q.n)>>> (dpd_mass, o::q.pp, o::ff, o::q.n);
}

void update_rbc() {
    if (r::q.n) dev::update<<<k_cnf(r::q.n)>>> (rbc_mass, r::q.pp, r::ff, r::q.n);
}

void bounce() {
    if (o::q.n) sdf::sub::dev::bounce<<<k_cnf(o::q.n)>>>(w::qsdf.texsdf, (float2*)o::q.pp, o::q.n);
    //if (rbcs && r::n) k_sdf::bounce<<<k_cnf(r::n)>>>((float2*)r::pp, r::n);
}
