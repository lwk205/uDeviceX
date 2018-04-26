template <typename T>
static void remove(T *data, int nv, int *e, int nc) {
    int c; /* c: cell */
    for (c = 0; c < nc; c++) cA2A(data + nv*c, data + nv*e[c], nv);
}

static void remove_rbcs(RbcQuants *q, const Sdf *qsdf) {
    int stay[MAX_CELL_NUM];
    int nc0;
    q->nc = sdf_who_stays(qsdf, q->n, q->pp, nc0 = q->nc, q->nv, /**/ stay);
    q->n = q->nc * q->nv;
    remove(q->pp, q->nv, stay, q->nc);
    msg_print("%d/%d RBCs survived", q->nc, nc0);
}

static void create_solids(const Coords *coords, bool empty_pp, int numdensity, float rig_mass, const RigPinInfo *pi,
                          MPI_Comm cart, const MeshRead *mesh, FluQuants* qflu, RigQuants* qrig) {
    UC(rig_gen_freeze(coords, empty_pp, numdensity, rig_mass, pi, cart, mesh, /*io*/ qflu->pp, &qflu->n, /**/ qrig));
    MC(m::Barrier(cart));
    msg_print("created %d rigid object(s).", qrig->ns);
}

static void remove_solids(RigQuants *q, const Sdf *sdf) {
    int stay[MAX_SOLIDS];
    int ns0;
    int nip = q->ns * q->nv;
    q->ns = sdf_who_stays(sdf, nip, q->i_pp, ns0 = q->ns, q->nv, /**/ stay);
    q->n  = q->ns * q->nps;
    remove(q->pp,       q->nps,      stay, q->ns);
    remove(q->pp_hst,   q->nps,      stay, q->ns);

    remove(q->ss,       1,           stay, q->ns);
    remove(q->ss_hst,   1,           stay, q->ns);

    remove(q->i_pp,     q->nv, stay, q->ns);
    remove(q->i_pp_hst, q->nv, stay, q->ns);
    msg_print("sim.impl: %d/%d Solids survived", q->ns, ns0);
}

void inter_freeze(const Coords *coords, MPI_Comm cart, InterWalInfos w, InterFluInfos f, InterRbcInfos r, InterRigInfos s) {
    MC(m::Barrier(cart));
    if (s.active)             rig_gen_mesh(coords, cart, s.mesh, "rigs-ic.txt", /**/ s.q);
    if (w.active && r.active) remove_rbcs(r.q, w.sdf);
    if (w.active && s.active) remove_solids(s.q, w.sdf);
    if (s.active)             create_solids(coords, s.empty_pp, s.numdensity, s.mass, s.pi, cart, s.mesh, f.q, s.q);
    if (r.active)             rbc_gen_freeze(cart, r.q);
}
