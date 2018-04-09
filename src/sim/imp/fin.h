static void fin_flu_exch(/**/ FluExch *e) {
    UC(eflu_pack_fin(/**/ e->p));
    UC(eflu_comm_fin(/**/ e->c));
    UC(eflu_unpack_fin(/**/ e->u));
}

static void fin_obj_exch(/**/ ObjExch *e) {
    UC(eobj_pack_fin(/**/ e->p));
    UC(eobj_comm_fin(/**/ e->c));
    UC(eobj_unpack_fin(/**/ e->u));
    UC(eobj_packf_fin(/**/ e->pf));
    UC(eobj_unpackf_fin(/**/ e->uf));
}

static void fin_mesh_exch(/**/ Mexch *e) {
    UC(emesh_pack_fin(/**/ e->p));
    UC(emesh_comm_fin(/**/ e->c));
    UC(emesh_unpack_fin(/**/ e->u));
}

static void fin_bb_exch(/**/ BBexch *e) {
    UC(fin_mesh_exch(/**/ e));
    
    UC(emesh_packm_fin(/**/ e->pm));
    UC(emesh_commm_fin(/**/ e->cm));
    UC(emesh_unpackm_fin(/**/ e->um));
}

static void fin_flu_distr(/**/ FluDistr *d) {
    UC(dflu_pack_fin(/**/ d->p));
    UC(dflu_comm_fin(/**/ d->c));
    UC(dflu_unpack_fin(/**/ d->u));
}

static void fin_rbc_distr(/**/ RbcDistr *d) {
    UC(drbc_pack_fin(/**/ d->p));
    UC(drbc_comm_fin(/**/ d->c));
    UC(drbc_unpack_fin(/**/ d->u));
}

static void fin_rig_distr(/**/ RigDistr *d) {
    UC(drig_pack_fin(/**/ d->p));
    UC(drig_comm_fin(/**/ d->c));
    UC(drig_unpack_fin(/**/ d->u));
}

static void fin_colorer(Colorer *c) {
    UC(fin_mesh_exch(/**/ &c->e));
    Dfree(c->pp);
    Dfree(c->minext);
    Dfree(c->maxext);
}

static void fin_outflow(Outflow *o) {
    UC(outflow_fin(/**/ o));
}

static void fin_denoutflow(DCont *d, DContMap *m) {
    UC(den_fin(d));
    UC(den_map_fin(m));
}

static void fin_inflow(Inflow *i) {
    UC(inflow_fin(/**/ i));
}


static void fin_flu(const Opt *opt, Flu *f) {
    UC(flu_fin(&f->q));
    UC(fluforces_bulk_fin(/**/ f->bulk));
    UC(fluforces_halo_fin(/**/ f->halo));
 
    UC(fin_flu_distr(/**/ &f->d));
    UC(fin_flu_exch(/**/ &f->e));

    UC(Dfree(f->ff));
    EFREE(f->ff_hst);

    if (opt->fluss) {
        UC(Dfree(f->ss));
        EFREE(f->ss_hst);        
    }
}

static void fin_rbc(const Opt *opt, Rbc *r) {
    UC(rbc_fin(&r->q));
    UC(rbc_force_fin(r->force));

    UC(fin_rbc_distr(/**/ &r->d));
        
    Dfree(r->ff);
    UC(triangles_fin(r->tri));

    if (opt->dump_rbc_com) UC(rbc_com_fin(/**/ r->com));
    if (opt->rbcstretch)   UC(rbc_stretch_fin(/**/ r->stretch));
    UC(rbc_params_fin(r->params));
    UC(mesh_read_fin(r->cell));
    UC(mesh_write_fin(r->mesh_write));
}

static void fin_rig(Rig *s) {
    UC(rig_fin(&s->q));
    UC(scan_fin(/**/ s->ws));
    Dfree(s->ff);
    EFREE(s->ff_hst);

    UC(fin_rig_distr(/**/ &s->d));
    UC(mesh_write_fin(s->mesh_write));
    UC(rig_fin_pininfo(s->pininfo));
}

static void fin_bounce_back(BounceBack *bb) {
    UC(meshbb_fin(/**/ bb->d));
    Dfree(bb->mm);
    UC(fin_bb_exch(/**/ &bb->e));
}

static void fin_wall(Wall *w) {
    UC(sdf_fin(w->sdf));
    UC(wall_fin_quants(&w->q));
    UC(wall_fin_ticket(w->t));
    UC(wvel_fin(w->vel));
    UC(wvel_step_fin(w->velstep));
}
    
static void fin_objinter(const Opt *opt, ObjInter *o) {
    UC(fin_obj_exch(&o->e));
    if (opt->cnt) cnt_fin(o->cnt);
    if (opt->fsi) fsi_fin(o->fsi);
}

static void fin_vcon(Vcon *c) {
    UC(vcont_fin(c->vcont));
}

static void fin_pair_params(Sim *s) {
    UC(pair_fin(s->flu.params));
    UC(pair_fin(s->objinter.cntparams));
    UC(pair_fin(s->objinter.fsiparams));
}

static void fin_dump(const Opt *opt, Dump *d) {
    if (opt->dump_field) UC(io_field_fin(d->iofield));
    if (opt->dump_parts) UC(io_rig_fin(d->iorig));
    UC(io_bop_fin(d->bop));
    UC(diag_part_fin(d->diagpart));
    EFREE(d->pp);
}

static void fin_time(Time *t) {
    UC(time_step_fin(t->step));
    UC(time_step_accel_fin(t->accel));    
}

void sim_fin(Sim *s) {
    const Opt *opt = &s->opt;
    
    if (opt->rbc || opt->rig)
        UC(fin_objinter(&s->opt, &s->objinter));

    if (opt->vcon)       UC(fin_vcon(/**/ &s->vcon));
    if (opt->outflow)    UC(fin_outflow(/**/ s->outflow));
    if (opt->inflow)     UC(fin_inflow (/**/ s->inflow ));
    if (opt->denoutflow) UC(fin_denoutflow(/**/ s->denoutflow, s->mapoutflow));
    
    if (opt->wall) UC(fin_wall(&s->wall));

    UC(fin_flu(opt, &s->flu));

    if (opt->flucolors && opt->rbc)
        UC(fin_colorer(/**/ &s->colorer));

    if (opt->rig) {
        UC(fin_rig(/**/ &s->rig));
        
        if (opt->rig_bounce)
            UC(fin_bounce_back(&s->bb));
    }
    
    if (opt->rbc) UC(fin_rbc(opt, /**/ &s->rbc));

    UC(bforce_fin(s->bforce));
    
    UC(fin_dump(opt, &s->dump));
    UC(scheme_restrain_fin(s->restrain));
    UC(coords_fin(/**/ s->coords));
    UC(fin_time(&s->time));
    UC(fin_pair_params(s));
    UC(inter_color_fin(s->gen_color));
    UC(dbg_fin(s->dbg));

    MC(m::Comm_free(&s->cart));
    EFREE(s);
}
