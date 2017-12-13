static void garea_volume(rbc::Quants q, /**/ float *a, float *v) {
    int nt, nv, nc;
    const Particle *pp;
    const int4 *tri;
    float hst[2], *dev;

    Dalloc(&dev, 2);

    nt = q.nt; nv = q.nv; nc = q.nc;
    pp = q.pp;
    tri = q.tri;
    area_volume::main(nt, nv, nc, pp, tri, /**/ dev);
    cD2H(hst, dev, 2);
    Dfree(dev);

    *a = hst[0]; *v = hst[1];
}

static void dump(Coords coords, rbc::Quants q, rbc::force::TicketT t) {
    int n;
    Particle *pp;
    float area, volume, area0, volume0;
    static int i = 0;
    n = q.nc * q.nv;
    UC(emalloc(n*sizeof(Particle), (void**)&pp));
    cD2H(pp, q.pp, q.n);
    io::mesh::rbc(m::cart, coords, pp, q.tri_hst, q.nc, q.nv, q.nt, i++);
    rbc::force::stat(/**/ &area0, &volume0);
    garea_volume(q, /**/ &area, &volume);
    MSG("av: %g %g", area/area0, volume/volume0);
    diagnostics(m::cart, n, pp, i);
    free(pp);
}

static int body_force(Coords coords, rbc::Quants q, Force *f) {
    BForce bf;
    BForce_cste par;
    BForce_v v;
    par.a = make_float3(FORCE_PAR_A, 0, 0);
    ini(par, /**/ &bf);
    get_view(0, bf, /**/ &v);
    
    UC(body_force(coords, rbc_mass, v, q.n, q.pp, /**/ f));
    return 0;
}

static void run0(Coords coords, rbc::Quants q, rbc::force::TicketT t, rbc::stretch::Fo* stretch, Force *f) {
    long i;
    long nsteps = (long)(tend / dt);
    MSG("will take %ld steps", nsteps);
    for (i = 0; i < nsteps; i++) {
        Dzero(f, q.n);
        rbc::force::apply(q, t, /**/ f);
        stretch::apply(q.nc, stretch, /**/ f);
        if (pushrbc) body_force(coords, q, /**/ f);
        scheme::move::main(rbc_mass, q.n, f, q.pp);
        if (i % part_freq  == 0) dump(coords, q, t);
#ifdef RBC_CLEAR_VEL
        scheme::move::clear_vel(q.n, /**/ q.pp);
#endif
    }
}

static void run1(Coords coords, rbc::Quants q, rbc::force::TicketT t,
                 rbc::stretch::Fo *stretch) {
    Force *f;
    Dalloc(&f, q.n);
    Dzero(f, q.n);
    run0(coords, q, t, stretch, f);
    Dfree(f);
}

static void run2(Coords coords, const char *cell, const char *ic, rbc::Quants q) {
    rbc::stretch::Fo *stretch;
    rbc::force::TicketT t;
    rbc::main::gen_quants(m::cart, cell, ic, /**/ &q);
    UC(stretch::ini("rbc.stretch", q.nv, /**/ &stretch));
    rbc::force::gen_ticket(q, &t);
    run1(coords, q, t, stretch);
    stretch::fin(stretch);
    rbc::force::fin_ticket(&t);
}

void run(Coords coords, const char *cell, const char *ic) {
    rbc::Quants q;
    rbc::main::ini(&q);
    run2(coords, cell, ic, q);
    rbc::main::fin(&q);
}
