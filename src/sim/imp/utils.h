static double compute_volume_rbc(MPI_Comm comm, const Rbc *r) {
    double loc, tot, V0;
    long nc;
    nc = r->q.nc;
    V0 = rbc_params_get_tot_volume(r->params);

    tot = 0;
    loc = nc * V0;
    MC(m::Allreduce(&loc, &tot, 1, MPI_DOUBLE, MPI_SUM, comm));
    
    return tot;
}

static void utils_compute_hematocrit(const Sim *s) {
    const Opt *opt = &s->opt;
    double Vdomain, Vrbc, Ht;
    if (!opt->rbc) return;

    if (opt->wall) {
        enum {NSAMPLES = 100000};
        Vdomain = sdf_compute_volume(s->cart, s->params.L, s->wall.sdf, NSAMPLES);
    }
    else {
        const Coords *c = s->coords;
        Vdomain = xdomain(c) * ydomain(c) * zdomain(c);
    }

    Vrbc = compute_volume_rbc(s->cart, &s->rbc);
    
    Ht = Vrbc / Vdomain;

    msg_print("Geometry volume: %g", Vdomain);
    msg_print("Hematocrit: %g", Ht);
}

static void utils_dump_history(const Config *cfg, const char *fname) {
    FILE *f;
    UC(efopen(fname, "w", &f));
    UC(conf_write_history(cfg, f));
    UC(efclose(f));
}

static float utils_get_dt0(Sim *s) {
    TimeStep *time_step;    
    time_step = s->time.step;
    return time_step_dt0(time_step);
}

static float utils_get_dt(Sim *s, TimeLine *time) {
    /* Possibility to adapt dt only after equilibration */
    if (s->equilibrating)
        return time_step_dt0(s->time.step);
    else {
        const Flu *flu = &s->flu;
        const Rbc *rbc = &s->rbc;
        const Opt *opt = &s->opt;

        time_step_accel_reset(s->time.accel);
        if (flu->q.n)
            time_step_accel_push(s->time.accel, flu->mass, flu->q.n, flu->ff);
        if (opt->rbc && rbc->q.n)
            time_step_accel_push(s->time.accel, rbc->mass, rbc->q.n, rbc->ff);

        const float dt = time_step_dt(s->time.step, s->cart, s->time.accel);

        if (time_line_cross(time, opt->freq_parts))
            time_step_log(s->time.step);

        return dt;
    }
}

static bool active_walls(const Sim *s) {
    return !s->equilibrating && s->opt.wall;
}

static bool active_rbc(const Sim *s) {
    return s->opt.rbc;
}

static bool active_rig(const Sim *s) {
    return !s->equilibrating && s->opt.rig;
}
