static void get_stress_view(const RbcForce *f, /**/ StressFul_v *v) {
    *v = f->sinfo.sful;
}

static void get_stress_view(const RbcForce *f, /**/ StressFree_v *v) {
    *v = f->sinfo.sfree;
}

static void get_rnd_view(const RbcForce *f, /**/ Rnd0_v *v) {
    *v = f->rinfo.rnd0;
}

static void get_rnd_view(const RbcForce *f, /**/ Rnd1_v *v) {
    *v = f->rinfo.rnd1;
}

template <typename Stress_v, typename Rnd_v>
static void apply(float dt, RbcParams_v parv, int nc, int nv, int md,
                  const Particle *pp,
                  const Adj_v *adj_v, Stress_v sv, Rnd_v rv,
                  float *av, /**/ Force *ff) {
    if (!d::is_device_pointer(ff))  ERR("`ff` is not a device pointer");
    KL(rbc_bending_dev::force, (k_cnf(nc*nv*md)), (dt, parv, md, nv, nc, pp,
                                                 *adj_v, sv, rv, av, /**/ (float*)ff));
}

template <typename Stress_v>
static void dispatch_rnd(float dt, RbcParams_v parv, int nc, int nv, int md,
                         const Particle *pp,
                         const Adj_v *adj_v, RbcForce *t, Stress_v sv,
                         float *av, /**/ Force *ff){
    if (is_rnd(t)) {
        Rnd1_v rv;
        get_rnd_view(t, &rv);
        apply(dt, parv, nc, nv, md, pp, adj_v, sv, rv, av, /**/ ff);
    }
    else {
        Rnd0_v rv;
        get_rnd_view(t, &rv);
        apply(dt, parv, nc, nv, md, pp, adj_v, sv, rv, av, /**/ ff);
    }
}


void rbc_bending_apply(RbcForce *t, const RbcParams *par, float dt, const RbcQuants *q, /**/ Force *ff) {
    RbcParams_v parv;
    float *av;
    if (q->nc <= 0) return;
    if (!d::is_device_pointer(q->pp))  ERR("`q->pp` is not a device pointer");

    parv = rbc_params_get_view(par);

    if (is_stress_free(t)) {
        StressFree_v si;
        get_stress_view(t, &si);
        dispatch_rnd(dt, parv, q->nc, q->nv, q->md, q->pp,
                     t->adj_v, t, si, av, /**/ ff);
    }
    else {
        StressFul_v si;
        get_stress_view(t, &si);
        dispatch_rnd(dt, parv, q->nc, q->nv, q->md, q->pp,
                     t->adj_v, t, si, av, /**/ ff);
    }    
}
