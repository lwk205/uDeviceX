static void forces_cnt(ObjInter *oi, int nw, PaWrap *pw, FoWrap *fw) {
    cnt_build_cells(nw, pw, /**/ oi->cnt);
    cnt_bulk(oi->cntparams, oi->cnt, nw, pw, fw);
}

static void forces_fsi(ObjInter *oi, int nw, PaWrap *pw, FoWrap *fw) {
    fsi_bulk(oi->fsiparams, oi->fsi, nw, pw, fw);
}

void forces_objects(Sim *sim) {
    PaArray parray;
    const Opt *opt = &sim->opt;
    PaWrap pw[MAX_OBJ_TYPES];
    FoWrap fw[MAX_OBJ_TYPES];
    int nw = 0;
    ObjInter *oi = &sim->objinter;
    Flu *f = &sim->flu;
    Rbc *r = &sim->rbc;
    Rig *s = &sim->rig;
    ObjExch *e = &oi->e;
    
    if (active_rig(sim)) {
        pw[nw] = {s->q.n, s->q.pp};
        fw[nw] = {s->q.n, s->ff};
        ++nw;
    }
    if (active_rbc(sim)) {
        pw[nw] = {r->q.n, r->q.pp};
        fw[nw] = {r->q.n, r->ff};
        ++nw;
    }

    if (!nw) return;
    if (nw > MAX_OBJ_TYPES) ERR("Too many objects in wrappers : %d/%d", nw, MAX_OBJ_TYPES);

    /* Prepare and send the data */
    
    UC(eobj_build_map(nw, pw, /**/ e->p));
    UC(eobj_pack(nw, pw, /**/ e->p));
    UC(eobj_download(nw, /**/ e->p));

    UC(eobj_post_send(e->p, e->c));
    UC(eobj_post_recv(e->c, e->u));

    /* bulk interactions */
    
    UC(parray_push_pp(f->q.pp, &parray));
    if (opt->flucolors)
        parray_push_cc(f->q.cc, &parray);

    if (opt->fsi)
        fsi_bind_solvent(parray, f->ff, f->q.n, f->q.cells.starts, /**/ oi->fsi);

    if (opt->cnt) UC(forces_cnt(oi, nw, pw, fw));
    if (opt->fsi) UC(forces_fsi(oi, nw, pw, fw));

    /* recv data and halo interactions  */

    UC(eobj_wait_send(e->c));
    UC(eobj_wait_recv(e->c, e->u));

    int26 hcc = eobj_get_counts(e->u);
    Pap26 hpp = eobj_upload_shift(e->u);
    Fop26 hff = eobj_reini_ff(e->u, e->pf);

    if (opt->fsi) UC(fsi_halo(oi->fsiparams, oi->fsi, hpp, hff, hcc.d));
    if (opt->cnt) UC(cnt_halo(oi->cntparams, oi->cnt, nw, pw, fw, hpp, hff, hcc.d));

    /* send the forces back */ 
    
    UC(eobj_download_ff(e->pf));

    UC(eobj_post_send_ff(e->pf, e->c));
    UC(eobj_post_recv_ff(e->c, e->uf));

    UC(eobj_wait_send_ff(e->c));
    UC(eobj_wait_recv_ff(e->c, e->uf));

    UC(eobj_unpack_ff(e->uf, e->p, nw, /**/ fw));
}

