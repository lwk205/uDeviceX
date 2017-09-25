namespace rig {
namespace sub {
namespace ic {

static void ini0(const Mesh m, int nsolid, int rcount, int idmax, int root, float *coms, /**/
                 int *ns, int *nps, float *rr0, Solid *ss, Particle *r_pp)
{
    Solid model;
    // share model to everyone
    int npsolid = 0;
    if (m::rank == root)
    {
        npsolid = rcount;
        if (!npsolid) ERR("No particles remaining in root node.\n");
        for (int d = 0; d < 3; ++d)
            model.com[d] = coms[idmax*3 + d];

        rig::ini(r_pp, npsolid, solid_mass, model.com, m, /**/ rr0, &model);

        if (empty_solid_particles)
            empty_solid(m.nt, m.tt, m.vv, /* io */ rr0, &npsolid);
    }

    MC(MPI_Bcast(&npsolid,       1,   MPI_INT, root, m::cart) );
    MC(MPI_Bcast(rr0,  3 * npsolid, MPI_FLOAT, root, m::cart) );
    MC(MPI_Bcast(&model, 1, datatype::solid, root, m::cart) );

    // filter coms to keep only the ones in my domain
    int id = 0;
    for (int j = 0; j < nsolid; ++j) {
        const float *com = coms + 3*j;

        if (-XS/2 <= com[X] && com[X] < XS/2 &&
            -YS/2 <= com[Y] && com[Y] < YS/2 &&
            -ZS/2 <= com[Z] && com[Z] < ZS/2 ) {
            ss[id] = model;

            for (int d = 0; d < 3; ++d)
            ss[id].com[d] = com[d];

            ++id;
        }
    }

    *ns = nsolid = id;
    *nps = npsolid;

    set_ids(nsolid, /**/ ss);
}

static void ini1(const Mesh m, int nsolid, float *coms, /**/
                 int *ns, int *nps, float *rr0, Solid *ss, int *s_n, Particle *s_pp, Particle *r_pp,
                 /*w*/ int *tags, int *rcounts)
{
    int root, idmax;
    elect(rcounts, nsolid, /**/ &root, &idmax);
    MC(MPI_Bcast(&idmax, 1, MPI_INT, root, m::cart));

    int rcount = 0;
    kill(idmax, tags, /**/ s_n, s_pp, &rcount, r_pp);
    DBG("after kill: %d", rcount);

    share(root, /**/ r_pp, &rcount);
    DBG("after share: %d", rcount);

    ini0(m, nsolid, rcount, idmax, root, coms, /**/ ns, nps, rr0, ss, r_pp);
}

static void ini2(const Mesh m, int nsolid, float *coms, /**/
                 int *ns, int *nps, float *rr0, Solid *ss, int *s_n, Particle *s_pp, Particle *r_pp,
                 /*w*/ int *tags, int *rcounts) {
    count_pp_inside(s_pp, *s_n, coms, nsolid, m.tt, m.vv, m.nt, /**/ tags, rcounts);
    ini1(m, nsolid, coms, /**/ ns, nps, rr0, ss, s_n, s_pp, r_pp, /*w*/ tags, rcounts);
}

static void ini3(const Mesh m, int nsolid, float *coms, /**/
                 int *ns, int *nps, float *rr0, Solid *ss, int *s_n, Particle *s_pp, Particle *r_pp) {
    int *tags = new int[*s_n];
    int *rcounts = new int[nsolid];
    ini2(m, nsolid, coms, /**/ ns, nps, rr0, ss, s_n, s_pp, r_pp, /*w*/ tags, rcounts);
    delete[] rcounts;
    delete[] tags;
}

static void ini4(const char *fname, const Mesh m, /**/
                 int *ns, int *nps, float *rr0, Solid *ss, int *s_n, Particle *s_pp, Particle *r_pp,
                 /*w*/ float *coms) {
    float3 minbb, maxbb;
    int nsolid = read_coms(fname, /**/ coms);
    if (nsolid == 0) ERR("No solid provided.\n");
    mesh::get_bbox(m.vv, m.nv, /**/ &minbb, &maxbb);
    nsolid = duplicate_PBC(minbb, maxbb, nsolid, /**/ coms);
    make_local(nsolid, /**/ coms);
    ini3(m, nsolid, coms, /**/ ns, nps, rr0, ss, s_n, s_pp, r_pp);
}

void ini(const char *fname, const Mesh m, /**/
         int *ns, int *nps, float *rr0, Solid *ss, int *s_n, Particle *s_pp, Particle *r_pp) {
    float *coms = new float[MAX_SOLIDS * 3 * 10];
    ini4(fname, m, /**/ ns, nps, rr0, ss, s_n, s_pp, r_pp, /*w*/ coms);
    delete[] coms;
}


} // ic
} // rig
} // sub
