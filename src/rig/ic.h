enum {X, Y, Z};

//#define DEBUG_MSG
    
static int read_coms(const char *fname, /**/ float* coms) {
    int nsolids = 0;
    FILE *f = fopen(fname, "r"); 

    if (f == NULL) 
    ERR("Could not open %s.\n", fname);
    
    float x, y, z;
    int i = 0;
        
    while (fscanf(f, "%f %f %f\n", &x, &y, &z) == 3) {
        coms[3*i + X] = x;
        coms[3*i + Y] = y;
        coms[3*i + Z] = z;
        i++;
        assert(i < MAX_SOLIDS);
    }
    nsolids = i;
    return nsolids;
}

/* bbox: minx, maxx, miny, maxy, minz, maxz */
static int duplicate_PBC(const float3 minbb, const float3 maxbb, int n, /**/ float *coms) {
    struct f3 {float x[3];};
    const int Lg[3] = {XS * m::dims[X], YS * m::dims[Y], ZS * m::dims[Z]};
    int id = n;
    for (int j = 0; j < n; ++j) {
        f3 r0 = {coms[3*j + X], coms[3*j + Y], coms[3*j + Z]};
        std::vector<f3> dupls;
        dupls.push_back(r0);

        auto duplicate = [&](int d, int sign) {
#ifdef spdir
            if (d == spdir) return;
#endif
            auto dupls2 = dupls;
            for (f3& r : dupls2)
            r.x[d] += Lg[d] * sign;
            dupls.insert(dupls.end(), dupls2.begin(), dupls2.end());      
        };

        if (r0.x[0] + minbb.x < 0) duplicate(0, +1);
        if (r0.x[1] + minbb.y < 0) duplicate(1, +1);
        if (r0.x[2] + minbb.z < 0) duplicate(2, +1);

        if (r0.x[0] + maxbb.x >= Lg[0]) duplicate(0, -1);
        if (r0.x[1] + maxbb.y >= Lg[1]) duplicate(1, -1);
        if (r0.x[2] + maxbb.z >= Lg[2]) duplicate(2, -1);

        // k from 1: do not reinsert the original
        for (int k = 1; k < (int) dupls.size(); ++k) {
            for (int d = 0; d < 3; ++d)
            coms[3*id + d] = dupls[k].x[d];

            ++id;
        }
    }
    return id;
}

static void make_local(const int n, /**/ float *coms) {
    const int L[3] = {XS, YS, ZS};
    int mi[3];
    for (int c = 0; c < 3; ++c) mi[c] = (m::coords[c] + 0.5) * L[c];

    for (int j = 0; j < n; ++j)
    for (int d = 0; d < 3; ++d)
    coms[3*j + d] -= mi[d];
}

static void count_pp_inside(const Particle *s_pp, const int n, const float *coms, const int ns,
                            const int *tt, const float *vv, const int nt,
                            /**/ int *tags, int *rcounts) {
    for (int j = 0; j < ns; ++j) rcounts[j] = 0;

    const float R = (XS > YS) ? (XS > ZS ? XS : ZS) : (YS > ZS ? YS : ZS);
    
    for (int ip = 0; ip < n; ++ip) {
        const Particle p = s_pp[ip]; const float *r0 = p.r;
        int tag = -1;
        for (int j = 0; j < ns; ++j) {
            const float *com = coms + 3*j;
            const float r[3] = {r0[X]-com[X], r0[Y]-com[Y], r0[Z]-com[Z]};

            const float r2 = r[X]*r[X] + r[Y]*r[Y] + r[Z]*r[Z];     

            if (r2 < R*R && collision::inside_1p(r, vv, tt, nt)) {
                ++rcounts[j];
                tags[ip] = tag = j;
            }
        }

        if (tag == -1)
        tags[ip] = tag;
    }

#ifdef DEBUG_MSG
    for (int j = 0; j < ns; ++j)
    MSG("Found %d particles in solid %d", rcounts[j], j);
#endif
}

static void elect(const int *rcounts, const int ns, /**/ int *root, int *idmax) {
    int localmax[2] = {0, m::rank}, globalmax[2] = {0, m::rank}, idmax_ = 0;

    for (int j = 0; j < ns; ++j)
    if (localmax[0] < rcounts[j]) {
        localmax[0] = rcounts[j];
        idmax_ = j;
    }
    
    MPI_Allreduce(localmax, globalmax, 1, MPI_2INT, MPI_MAXLOC, m::cart);

    *root = globalmax[1];
    *idmax = idmax_;
}

static void kill(const int idmax, const int *tags, /**/ int *s_n, Particle *s_pp, int *r_n, Particle *r_pp) {
    int scount = 0, rcount = 0;

    for (int ip = 0; ip < *s_n; ++ip) {
        Particle p = s_pp[ip];
        const int tag = tags[ip];

        if (tag == -1)         // solvent
        s_pp[scount++] = p;
        else if (tag == idmax) // elected solid
        r_pp[rcount++] = p;
    }
        
    *s_n = scount;
    *r_n = rcount;
}

static void share_parts(const int root, /**/ Particle *pp, int *n) {
    // set to global coordinates and then convert back to local
    const int L[3] = {XS, YS, ZS};
    int mi[3];
    for (int c = 0; c < 3; ++c) mi[c] = (m::coords[c] + 0.5) * L[c];

    for (int i = 0; i < *n; ++i)
    for (int c = 0; c < 3; ++c)
    pp[i].r[c] += mi[c];

    std::vector<int> counts(m::size), displs(m::size);
    std::vector<Particle> recvbuf(MAX_PSOLID_NUM);

    if (*n >= MAX_PSOLID_NUM)
    ERR("Number of solid particles too high for the buffer\n");
        
    MC( MPI_Gather(n, 1, MPI_INT, counts.data(), 1, MPI_INT, root, m::cart) );

    if (m::rank == root)
    {
        displs[0] = 0;
        for (int j = 0; j < m::d-1; ++j)
        displs[j+1] = displs[j] + counts[j];
    }

    MC( MPI_Gatherv(pp, *n, datatype::particle, recvbuf.data(), counts.data(), displs.data(), datatype::particle, root, m::cart) );

    if (m::rank == root) {
        *n = displs.back() + counts.back();
        for (int i = 0; i < *n; ++i) pp[i] = recvbuf[i];
    }
        
    for (int i = 0; i < *n; ++i)
    for (int c = 0; c < 3; ++c)
    pp[i].r[c] -= mi[c];
}

static void empty_solid(const Mesh m, /* io */ float *rr0, int *npsolid) {
    const int n0 = *npsolid;
    int j = 0;
    for (int i = 0; i < n0; ++i) {
        const float *r0 = rr0 + 3*i;
        const float d = mesh::dist_from_mesh(m, r0);
        //if (d> 5) ERR("d = %f", d);
        if (d <= 1) {
            rr0[3*j + X] = r0[X];
            rr0[3*j + Y] = r0[Y];
            rr0[3*j + Z] = r0[Z];
            ++j;
        }
    }
    if (j == 0) ERR("No particle remaining in solid template\n");
    MSG("Template solid: keep %d out of %d particles", j, n0);
    *npsolid = j;
}

void set_ids(const int ns, Solid *ss_hst) {
    int id = 0;
    MC( MPI_Exscan(&ns, &id, 1, MPI_INT, MPI_SUM, m::cart) );

    for (int j = 0; j < ns; ++j)
    ss_hst[j].id = id++;
}

void ini(const char *fname, const Mesh m, /**/ int *ns, int *nps, float *rr0, Solid *ss, int *s_n, Particle *s_pp, Particle *r_pp)
{
    int npsolid = 0;
    float3 minbb, maxbb;
    float *coms = new float[MAX_SOLIDS * 3 * 10];
        
    int nsolid = read_coms(fname, coms);
    
    if (nsolid == 0) ERR("No solid provided.\n");
    
    mesh::get_bbox(m.vv, m.nv, /**/ &minbb, &maxbb);
        
    nsolid = duplicate_PBC(minbb, maxbb, nsolid, /**/ coms);

    make_local(nsolid, coms);

    const int npp0 = *s_n;
    int *tags = new int[npp0];
    int *rcounts = new int[nsolid];

    count_pp_inside(s_pp, *s_n, coms, nsolid, m.tt, m.vv, m.nt, /**/ tags, rcounts);

    int root, idmax;
    elect(rcounts, nsolid, /**/ &root, &idmax);
    
    MC( MPI_Bcast(&idmax, 1, MPI_INT, root, m::cart) );

    int rcount = 0;

    kill(idmax, tags, /**/ s_n, s_pp, &rcount, r_pp);

    delete[] rcounts;
    delete[] tags;

    share_parts(root, /**/ r_pp, &rcount);

    Solid model;

    // share model to everyone
    
    if (m::rank == root)
    {
        npsolid = rcount;
            
        for (int d = 0; d < 3; ++d)
        model.com[d] = coms[idmax*3 + d];
    
        solid::ini(r_pp, npsolid, solid_mass, model.com, m, /**/ rr0, &model);

        empty_solid(m, /* io */ rr0, &npsolid);
    }
    
    MC( MPI_Bcast(&npsolid,       1,   MPI_INT, root, m::cart) );
    MC( MPI_Bcast(rr0,  3 * npsolid, MPI_FLOAT, root, m::cart) );
    MC( MPI_Bcast(&model, 1, datatype::solid, root, m::cart) );
    
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

    delete[] coms;
}
