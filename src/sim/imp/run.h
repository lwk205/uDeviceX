static float get_dt0(Sim *s) {
    TimeStep *time_step;    
    time_step = s->time_step;
    return time_step_dt0(time_step);
}

static float get_dt(Sim* s, Time* time) {
    /* Possibility to adapt dt only after equilibration */
    if (s->equilibrating)
        return time_step_dt0(s->time_step);
    else {
        const Flu *flu = &s->flu;
        const Rbc *rbc = &s->rbc;
        const Opt *opt = &s->opt;

        time_step_accel_reset(s->time_step_accel);
        if (flu->q.n)
            time_step_accel_push(s->time_step_accel, flu->mass, flu->q.n, flu->ff);
        if (opt->rbc && rbc->q.n)
            time_step_accel_push(s->time_step_accel, rbc->mass, rbc->q.n, rbc->ff);

        const float dt = time_step_dt(s->time_step, s->cart, s->time_step_accel);

        if (time_cross(time, opt->freq_parts))
            time_step_log(s->time_step);

        return dt;
    }
}

static void run_eq(Time *time, float te, Sim *s) { /* equilibrate */
    float dt;
    s->equilibrating = true;
    bool wall0 = false;
    dt = get_dt0(s);    
    while (time_current(time) < te) {
        UC(step(time, dt, wall0, 0.0, s));
        time_next(time, dt);
        dt = get_dt(s, time);
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
    UC(dump_strt_templ(s->coords, &s->wall, s));

    s->equilibrating = false;         
}

static void run(const Config *cfg, Time *time, float ts, float te, Sim *s) {
    float dt;

    /* ts, te: time start and end */
    dt = get_dt0(s);
    while (time_current(time) < te) {
        UC(step(time, dt, s->opt.wall, ts, s));
        time_next(time, dt);
        dt = get_dt(s, time);
    }
    UC(distribute_flu(/**/ s));
}
