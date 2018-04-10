static void dump_part(Sim *s) {
    const Flu *flu = &s->flu;
    const Rig *rig = &s->rig;
    const Opt *opt = &s->opt;
    IoBop *bop = s->dump.bop;
    int id_bop = s->dump.id_bop;
    cD2H(flu->q.pp_hst, flu->q.pp, flu->q.n);
    if (opt->fluids) {
        cD2H(flu->q.ii_hst, flu->q.ii, flu->q.n);
        io_bop_ids(s->cart, flu->q.n, flu->q.ii_hst, "id_solvent", id_bop, bop);
    }
    if (opt->flucolors) {
        cD2H(flu->q.cc_hst, flu->q.cc, flu->q.n);
        io_bop_colors(s->cart, flu->q.n, flu->q.cc_hst, "colors_solvent", id_bop, bop);
    }
    if (opt->fluss) {
        cD2H(flu->ss_hst, flu->ss, 6 * flu->q.n);
        io_bop_stresses(s->cart, flu->q.n, flu->ss_hst, "stress_solvent", id_bop, bop);
    }

    if (opt->dump_forces) {
        cD2H(flu->ff_hst, flu->ff, flu->q.n);
        io_bop_parts_forces(s->cart, s->coords, flu->q.n, flu->q.pp_hst, flu->ff_hst, "solvent", id_bop, bop);
    } else {
        io_bop_parts(s->cart, s->coords, flu->q.n, flu->q.pp_hst, "solvent", id_bop, bop);
    }

    if (active_rig(s)) {
        cD2H(rig->q.pp_hst, rig->q.pp, rig->q.n);
        if (opt->dump_forces) {
            cD2H(rig->ff_hst, rig->ff, rig->q.n);
            io_bop_parts_forces(s->cart, s->coords, flu->q.n, rig->q.pp_hst, rig->ff_hst, "solid", id_bop, bop);
        } else {
            io_bop_parts(s->cart, s->coords, flu->q.n, rig->q.pp_hst, "solid", id_bop, bop);
        }
    }
    s->dump.id_bop = ++id_bop;
}

static void dump_rbcs(Sim *s) {
    const Rbc *r = &s->rbc;
    cD2H(s->dump.pp, r->q.pp, r->q.n);
    UC(mesh_write_particles(r->mesh_write, s->cart, s->coords, r->q.nc, s->dump.pp, s->dump.id_rbc++));
}

static void dump_rbc_coms(Sim *s) {
    float3 *rr, *vv;
    Rbc *r = &s->rbc;
    int nc = r->q.nc;
    UC(rbc_com_apply(r->com, nc, r->q.pp, /**/ &rr, &vv));
    UC(io_com_dump(s->cart, s->coords, s->dump.id_rbc_com++, nc, r->q.ii, rr));
}

static void dump_grid(const Sim *s) {
    const Flu *flu = &s->flu;
    cD2H(s->dump.pp, flu->q.pp, flu->q.n);
    UC(io_field_dump_pp(s->coords, s->cart, s->dump.iofield, flu->q.n, s->dump.pp));
}

void dump_diag_after(TimeLine *time, bool solid0, Sim *s) { /* after wall */
    const Rig *rig = &s->rig;
    const Opt *o = &s->opt;
    if (solid0 && (time_line_cross(time, o->freq_parts))) {
        io_rig_dump(s->coords, time_line_get_current(time), rig->q.ns, rig->q.ss_dmp, rig->q.ss_dmp_bb, s->dump.iorig);
        cD2H(s->dump.pp, rig->q.i_pp, rig->q.ns * rig->q.nv);
        UC(mesh_write_particles(rig->mesh_write, s->cart, s->coords, rig->q.ns, s->dump.pp, s->dump.id_rig_mesh++));
    }
}

static int download_pp(Sim *s) { /* device to host  data transfer */
    int np = 0;
    Flu *flu = &s->flu;
    Rbc *rbc = &s->rbc;
    Rig *rig = &s->rig;

    if (flu->q.n) {
        cD2H(s->dump.pp + np, flu->q.pp, flu->q.n);    np += flu->q.n;
    }
    if (active_rig(s) && rig->q.n) {
        cD2H(s->dump.pp + np, rig->q.pp, rig->q.n);    np += rig->q.n;
    }
    if (active_rbc(s) && rbc->q.n) {
        cD2H(s->dump.pp + np, rbc->q.pp, rbc->q.n);    np += rbc->q.n;
    }
    return np;
}

static void diag(float time, Sim *s) {
    if (time < 0) ERR("time = %g < 0", time);
    int n;
    n = download_pp(s);
    UC(diag_part_apply(s->dump.diagpart, s->cart, time, n, s->dump.pp));
}

static void dump_strt_templ(Sim *s) { /* template dumps (wall, solid) */
    Rig *rig = &s->rig;
    Wall  *w = &s->wall;
    const Opt *opt = &s->opt;
    const char *base = opt->strt_base_dump;
    if (opt->dump_strt) {
        if (opt->wall) wall_strt_dump_templ(s->cart, base, &w->q);
        if (opt->rig)  rig_strt_dump_templ(s->cart, base, &rig->q);
    }
}

static void dump_strt0(int id, Sim *s) {
    Flu *flu = &s->flu;
    Rbc *rbc = &s->rbc;
    Rig *rig = &s->rig;
    const Opt *opt = &s->opt;
    const char *base = opt->strt_base_dump;
    flu_strt_dump(s->cart, base, id, &flu->q);
    if (opt->rbc) rbc_strt_dump(s->cart, base, id, &rbc->q);
    if (opt->rig) rig_strt_dump(s->cart, base, id, &rig->q);
}

static void dump_strt(Sim *s) {
    dump_strt0(s->dump.id_strt++, s);
}

static void dump_strt_final(Sim *s) {
    dump_strt0(RESTART_FINAL, s);
}

static void dump_diag(TimeLine *time, Sim *s) {
    const Opt *o = &s->opt;
    if (time_line_cross(time, o->freq_parts)) {
        if (o->dump_parts) dump_part(s);
        if (active_rbc(s)) dump_rbcs(s);
        UC(diag(time_line_get_current(time), s));
    }
    if (o->dump_field && time_line_cross(time, o->freq_field))
        dump_grid(s);
    if (o->dump_strt  && time_line_cross(time, o->freq_strt))
        dump_strt(s);
    if (o->dump_rbc_com && time_line_cross(time, o->freq_rbc_com))
        dump_rbc_coms(s);
}
