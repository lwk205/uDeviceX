enum {
    NCELLSWALL =
    (XS + 2*XWM) *
    (YS + 2*YWM) *
    (ZS + 2*ZWM)
};

enum {
    MAXNWALL = NCELLSWALL * numberdensity
};

void gen(Wall *w) { /* generate */
    run_eq(wall_creation);
    if (walls) {
        dSync();
        UC(gen(m::cart, /**/ w->sdf));
        MC(m::Barrier(m::cart));
        inter::create_walls(MAXNWALL, w->sdf, /*io*/ &flu.q, /**/ &w->q);
    }
    inter::freeze(m::cart, w->sdf, /*io*/ &flu.q, /**/ &rig.q, &rbc.q);
    clear_vel();

    if (multi_solvent) {
        Particle *pp = flu.q.pp;
        int n = flu.q.n;
        int *cc = flu.q.cc;
        Particle *pp_hst = a::pp_hst;
        int *cc_hst = flu.q.cc_hst;
        inter::color_dev(pp, n, /*o*/ cc, /*w*/ pp_hst, cc_hst);
    }
}

void sim_gen() {
    flu::gen_quants(&flu.q);
    flu::build_cells(&flu.q);
    if (global_ids)    flu::gen_ids  (m::cart, flu.q.n, &flu.q);
    if (rbcs) {
        rbc::main::gen_quants(m::cart, "rbc.off", "rbcs-ic.txt", /**/ &rbc.q);
        rbc::force::gen_ticket(rbc.q, &rbc.tt);

        if (multi_solvent) gen_colors(&rbc, &colorer, /**/ &flu);
    }
    MC(m::Barrier(m::cart));

    long nsteps = (long)(tend / dt);
    MSG("will take %ld steps", nsteps);
    if (walls || solids) {
        solids0 = false;  /* global */
        gen(/**/ &wall);
        dSync();
        if (walls && wall.q.n) wall::gen_ticket(wall.q, &wall.t);
        solids0 = solids;
        if (rbcs && multi_solvent) gen_colors(&rbc, &colorer, /**/ &flu);
        run(wall_creation, nsteps);
    } else {
        solids0 = solids;
        run(            0, nsteps);
    }
    /* final strt dump*/
    if (strt_dumps) dump_strt(restart::FINAL);
}

void sim_strt() {
    long nsteps = (long)(tend / dt);

    /*Q*/
    flu::strt_quants(restart::BEGIN, &flu.q);
    flu::build_cells(&flu.q);

    if (rbcs) rbc::main::strt_quants("rbc.off", restart::BEGIN, &rbc.q);
    dSync();

    if (solids) rig::strt_quants(restart::BEGIN, &rig.q);

    if (walls) wall::strt_quants(MAXNWALL, &wall.q);

    /*T*/
    if (rbcs)            rbc::force::gen_ticket(rbc.q, &rbc.tt);
    if (walls && wall.q.n) wall::gen_ticket(wall.q, &wall.t);

    MC(m::Barrier(m::cart));
    if (walls) {
        dSync();
        UC(gen(m::cart, /**/ wall.sdf));
        MC(m::Barrier(m::cart));
    }

    solids0 = solids;

    MSG("will take %ld steps", nsteps - wall_creation);
    run(wall_creation, nsteps);

    if (strt_dumps) dump_strt(restart::FINAL);
}
