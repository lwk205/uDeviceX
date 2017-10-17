namespace rig
{
void reinit_ft(const int nsolid, /**/ Solid *ss);
void reinit_ft_hst(const int nsolid, /**/ Solid *ss);
void update(const Force *ff, const float *rr0, int n, int nsolid, /**/ Particle *pp, Solid *ss);
void generate(const Solid *ss, const int ns, const float *rr0, const int nps, /**/ Particle *pp);
void generate_hst(const Solid *ss, const int ns, const float *rr0, const int nps, /**/ Particle *pp);

void mesh2pp_hst(const Solid *ss_hst, const int ns, int nv, const float *vv, /**/ Particle *pp);

void update_mesh(const Solid *ss, const int ns, int nv, const float *vv, /**/ Particle *pp);
void dump(const int it, const Solid *ss, const Solid *ssbb, int nsolid, const int *mcoords);
}
