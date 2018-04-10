void forces(float dt, TimeLine *time, Sim *s) {
    Flu *flu = &s->flu;
    Rbc *rbc = &s->rbc;
    Rig *rig = &s->rig;
    Wall *wall = &s->wall;
    bool fluss;
    Opt *opt = &s->opt;

    NVTX_PUSH("forces");
    
    fluss = opt->fluss && time_line_cross(time, opt->freq_parts);

    UC(clear_forces(flu->ff, flu->q.n));
    if (active_rig(s))  UC(clear_forces  (rig->ff, rig->q.n));
    if (active_rbc(s))  UC(clear_forces  (rbc->ff, rbc->q.n));
    if (fluss)          UC(clear_stresses(flu->ss, flu->q.n));
    
    UC(forces_dpd(fluss, flu));
    if (active_walls(s) && wall->q.n) forces_wall(fluss, wall, s);
    if (active_rbc(s)) forces_rbc(dt, opt, rbc);

    UC(forces_objects(s));
    
    dSync();

    NVTX_POP();
}
