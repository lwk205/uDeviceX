namespace gen {
void gen_rig_from_solvent(MPI_Comm comm, int nt, int nv, const int4 *tt, const float *vv, /* io */ Particle *opp, int *on,
                          /* o */ int *ns, int *nps, int *n, float *rr0_hst, Solid *ss_hst, Particle *pp_hst);

void set_rig_ids(int n, /**/ Solid * ss);
} // gen
