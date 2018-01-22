void body_force(long it, const BForce *bforce, Sim *s) {
    Flu *flu = &s->flu;
    Rbc *rbc = &s->rbc;
    Rig *rig = &s->rig;

    if (pushflu)
        UC(bforce_apply(it, s->coords, flu_mass, bforce, flu->q.n, flu->q.pp, /**/ flu->ff));
    if (pushsolid && s->solids0)
        UC(bforce_apply(it, s->coords, solid_mass, bforce, rig->q.n, rig->q.pp, /**/ rig->ff));
    if (pushrbc && rbcs)
        UC(bforce_apply(it, s->coords, rbc_mass, bforce, rbc->q.n, rbc->q.pp, /**/ rbc->ff));
}

void forces_rbc (Rbc *r) {
    rbc_force_apply(r->q, r->tt, r->params, /**/ r->ff);
    if (RBC_STRETCH) rbc_stretch_apply(r->q.nc, r->stretch, /**/ r->ff);
}

void clear_forces(Force* ff, int n) {
    if (n) DzeroA(ff, n);
}

void forces_wall(Wall *w, Sim *s) {
    using namespace wall;
    Cloud co, cs, cr;
    Flu *flu = &s->flu;
    Rbc *rbc = &s->rbc;
    Rig *rig = &s->rig;
    ini_cloud(flu->q.pp, &co);
    ini_cloud(rig->q.pp, &cs);
    ini_cloud(rbc->q.pp, &cr);
    if (multi_solvent) ini_cloud_color(flu->q.cc, &co);
    
    if (flu->q.n)               color::wall_force(w->vview, s->coords, w->sdf, &w->q, &w->t, flu->q.n, co, /**/ flu->ff);
    if (s->solids0 && rig->q.n) grey::wall_force(w->vview, s->coords, w->sdf, &w->q, &w->t, rig->q.n, cs, /**/ rig->ff);
    if (rbcs && rbc->q.n)       grey::wall_force(w->vview, s->coords, w->sdf, &w->q, &w->t, rbc->q.n, cr, /**/ rbc->ff);
}
