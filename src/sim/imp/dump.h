void dev2hst() { /* device to host  data transfer */
    int start = 0;
    cD2H(a::pp_hst + start, flu.q.pp, flu.q.n); start += flu.q.n;
    if (solids0) {
        cD2H(a::pp_hst + start, rig.q.pp, rig.q.n); start += rig.q.n;
    }
    if (rbcs) {
        cD2H(a::pp_hst + start, rbc.q.pp, rbc.q.n); start += rbc.q.n;
    }
}

void dump_part(int step) {
    cD2H(flu.q.pp_hst, flu.q.pp, flu.q.n);
    if (global_ids) {
        cD2H(flu.q.ii_hst, flu.q.ii, flu.q.n);
        bop::ids(m::cart, flu.q.ii_hst, flu.q.n, "id_solvent", step);
    }
    if (multi_solvent) {
        cD2H(flu.q.cc_hst, flu.q.cc, flu.q.n);
        bop::colors(m::cart, flu.q.cc_hst, flu.q.n, "colors_solvent", step);
    }

    if (force_dumps) {
        cD2H(flu.ff_hst, flu.ff, flu.q.n);
        bop::parts_forces(m::cart, flu.q.pp_hst, flu.ff_hst, flu.q.n, "solvent", step, /**/ &dumpt);
    } else {
        bop::parts(m::cart, flu.q.pp_hst, flu.q.n, "solvent", step, /**/ &dumpt);
    }

    if(solids0) {
        cD2H(rig.q.pp_hst, rig.q.pp, rig.q.n);
        if (force_dumps) {
            cD2H(rig.ff_hst, rig.ff, rig.q.n);
            bop::parts_forces(m::cart, rig.q.pp_hst, rig.ff_hst, rig.q.n, "solid", step, /**/ &dumpt);
        } else {
            bop::parts(m::cart, rig.q.pp_hst, rig.q.n, "solid", step, /**/ &dumpt);
        }
    }
}

void dump_rbcs(const Rbc *r) {
    static int id = 0;
    cD2H(a::pp_hst, r->q.pp, r->q.n);
    io::mesh::rbc(m::cart, a::pp_hst, r->q.tri_hst, r->q.nc, r->q.nv, r->q.nt, id++);
}

void dump_rbc_coms(Rbc *r) {
    static int id = 0;
    int nc = r->q.nc;
    rbc::com::get(r->q.nc, r->q.nv, r->q.pp, /**/ &r->com);
    dump_com(m::cart, id++, nc, r->q.ii, r->com.hrr);
}

void dump_grid() {
    QQ qq; /* pack for io/field_dumps */
    NN nn;
    qq.o = flu.q.pp; qq.s = rig.q.pp; qq.r = rbc.q.pp;
    nn.o = flu.q.n ; nn.s = rig.q.n ;  nn.r = rbc.q.n;
    fields_grid(m::cart, qq, nn, /*w*/ a::pp_hst);
}

void dump_diag_after(int it, bool wall0, bool solid0) { /* after wall */
    if (solid0 && it % part_freq == 0) {
        static int id = 0;
        rig_dump(it, rig.q.ss_dmp, rig.q.ss_dmp_bb, rig.q.ns, m::coords);

        cD2H(a::pp_hst, rig.q.i_pp, rig.q.ns * rig.q.nv);
        io::mesh::rig(m::cart, a::pp_hst, rig.q.htt, rig.q.ns, rig.q.nv, rig.q.nt, id++);
    }
}

void diag(int it) {
    int n = flu.q.n + rig.q.n + rbc.q.n; dev2hst();
    diagnostics(m::cart, n, a::pp_hst, it);
}

void dump_strt_templ() { /* template dumps (wall, solid) */
    if (strt_dumps) {
        if (walls) wall::strt_dump_templ(w::q);
        if (solids) rig::strt_dump_templ(rig.q);
    }
}

void dump_strt(int id) {
    flu::strt_dump(id, flu.q);
    if (rbcs)       rbc::main::strt_dump(id, rbc.q);
    if (solids)     rig::strt_dump(id, rig.q);
}

void dump_diag0(int it) { /* generic dump */
    if (it % part_freq  == 0) {
        if (part_dumps) dump_part(it);
        if (rbcs)       dump_rbcs(&rbc);
        diag(it);
    }
    if (field_dumps && it % field_freq == 0) dump_grid();
    if (strt_dumps  && it % strt_freq == 0)  dump_strt(it / strt_freq);
    if (rbc_com_dumps && it % rbc_com_freq == 0) dump_rbc_coms(&rbc);
}
