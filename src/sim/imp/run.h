static float get_dt0(Sim *s) {
    TimeStep *time_step;    
    time_step = s->time.step;
    return time_step_dt0(time_step);
}

static float get_dt(Sim *s, TimeLine *time) {
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

static void run_eq(float te, Sim *s) { /* equilibrate */
    float dt;
    s->equilibrating = true;
    bool wall0 = false;
    dt = get_dt0(s);
    while (time_line_get_current(s->time.t) < te) {
        UC(step(s->time.t, dt, wall0, 0.0, s));
        time_line_advance(dt, s->time.t);
        dt = get_dt(s, s->time.t);
    }
    UC(distribute_flu(/**/ s));
}

static void dump_history(const Config *cfg, const char *fname) {
    FILE *f;
    UC(efopen(fname, "w", &f));
    UC(conf_write_history(cfg, f));
    UC(efclose(f));
}

static void pre_run(const Config *cfg, Sim *s) {
    UC(bforce_set_conf(cfg, s->bforce));

    UC(dump_history(cfg, "conf.history.cfg"));
    UC(dump_strt_templ(s));

    compute_hematocrit(s);
    
    s->equilibrating = false;
}

static void run(float ts, float te, Sim *s) {
    float dt;

    /* ts, te: time start and end */
    dt = get_dt0(s);
    while (time_line_get_current(s->time.t) < te) {
        UC(step(s->time.t, dt, s->opt.wall, ts, s));
        time_line_advance(dt, s->time.t);
        dt = get_dt(s, s->time.t);
    }
    UC(distribute_flu(/**/ s));
}

