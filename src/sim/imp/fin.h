static void fin_flu_exch(/**/ FluExch *e) {
    using namespace exch::flu;
    fin(/**/ &e->p);
    fin(/**/ &e->c);
    fin(/**/ &e->u);
}

static void fin_obj_exch(/**/ ObjExch *e) {
    using namespace exch::obj;
    fin(/**/ &e->p);
    fin(/**/ &e->c);
    fin(/**/ &e->u);
    fin(/**/ &e->pf);
    fin(/**/ &e->uf);
}

static void fin_mesh_exch(/**/ Mexch *e) {
    using namespace exch::mesh;
    fin(/**/ &e->p);
    fin(/**/ &e->c);
    fin(/**/ &e->u);
}

static void fin_bb_exch(/**/ BBexch *e) {
    fin_mesh_exch(/**/ e);
    
    using namespace exch::mesh;
    fin(/**/ &e->pm);
    fin(/**/ &e->cm);
    fin(/**/ &e->um);
}

static void fin_flu_distr(/**/ FluDistr *d) {
    using namespace distr::flu;
    fin(/**/ &d->p);
    fin(/**/ &d->c);
    fin(/**/ &d->u);
}

static void fin_rbc_distr(/**/ RbcDistr *d) {
    using namespace distr::rbc;
    fin(/**/ &d->p);
    fin(/**/ &d->c);
    fin(/**/ &d->u);
}

static void fin_rig_distr(/**/ RigDistr *d) {
    using namespace distr::rig;
    fin(/**/ &d->p);
    fin(/**/ &d->c);
    fin(/**/ &d->u);
}

static void fin_colorer(Colorer *c) {
    fin_mesh_exch(/**/ &c->e);
    Dfree(c->pp);
    Dfree(c->minext);
    Dfree(c->maxext);
}

static void fin_outflow(Outflow *o) {
    fin(/**/ o);
}

static void fin_denoutflow(DCont *d, DContMap *m) {
    UC(fin(d));
    UC(fin(m));
}

static void fin_inflow(Inflow *i) {
    fin(/**/ i);
}


static void fin_flu(Flu *f) {
    flu::fin(&f->q);
    fin(/**/ f->bulkdata);
    fin(/**/ f->halodata);
 
    fin_flu_distr(/**/ &f->d);
    fin_flu_exch(/**/ &f->e);

    UC(Dfree(f->ff));
    UC(efree(f->ff_hst));
}

static void fin_rbc(Rbc *r) {
    rbc::main::fin(&r->q);
    rbc::force::fin_ticket(&r->tt);

    fin_rbc_distr(/**/ &r->d);
        
    Dfree(r->ff);

    if (rbc_com_dumps) rbc::com::fin(/**/ &r->com);
    if (RBC_STRETCH)   rbc::stretch::fin(/**/ r->stretch);
}

static void fin_rig(Rig *s) {
    rig::fin(&s->q);
    scan::free_work(/**/ &s->ws);
    Dfree(s->ff);
    UC(efree(s->ff_hst));

    UC(fin_rig_distr(/**/ &s->d));
}

static void fin_bounce_back(BounceBack *bb) {
    meshbb::fin(/**/ &bb->d);
    Dfree(bb->mm);
    UC(fin_bb_exch(/**/ &bb->e));
}

static void fin_wall(Wall *w) {
    sdf::free_quants(w->qsdf);
    wall::free_quants(&w->q);
    wall::free_ticket(&w->t);
}
    
static void fin_objinter(ObjInter *o) {
    UC(fin_obj_exch(&o->e));
    if (contactforces) cnt::fin(&o->cnt);
    if (fsiforces)     fsi::fin(&o->fsi);
}

void fin() {

    bop::fin(&dumpt);
    if (rbcs || solids)
        fin_objinter(&objinter);

    if (VCON)    UC(fin(/**/ &vcont));
    if (OUTFLOW) UC(fin_outflow(/**/ outflow));
    if (INFLOW) UC(fin_inflow(/**/ inflow));
    
    if (walls) fin_wall(&wall);

    fin_flu(&flu);

    if (multi_solvent && rbcs)
        fin_colorer(/**/ &colorer);

    if (solids) {
        fin_rig(/**/ &rig);
        
        if (sbounce_back)
            fin_bounce_back(&bb);
    }

    UC(efree(a::pp_hst));
    
    if (rbcs) fin_rbc(/**/ &rbc);
    datatype::fin();
}
