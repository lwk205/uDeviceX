static void ini_dump(FILE **f) {
    *f = NULL;
    if (m::rank) return;
    UC(efopen(DUMP_BASE "/vcont.txt", "w", /**/ f));
    fprintf(*f, "#vx vy vz fx fy fz\n");
    MSG("Velocity controller: dump to " DUMP_BASE "/vcont.txt");
}

static void fin_dump(FILE *f) {
    if (m::rank) return;
    UC(efclose(f));
}

static void reini_sampler(/**/ PidVCont *c) {
    int3 L = c->L;
    int ncells = L.x * L.y * L.z;

    if (ncells) CC(d::MemsetAsync(c->gridvel, 0, ncells * sizeof(float3)));

    c->nsamples = 0;
}

void ini(MPI_Comm comm, int3 L, float3 vtarget, float factor, /**/ PidVCont *c) {
    int ncells, nchunks;
    c->L = L;
    c->target = vtarget;
    c->current = make_float3(0, 0, 0);
    c->factor = factor;
    c->Kp = 2;
    c->Ki = 1;
    c->Kd = 8;

    MC(m::Comm_dup(comm, &c->comm));

    ncells = L.x * L.y * L.z;
    CC(d::Malloc((void **) &c->gridvel, ncells * sizeof(float3)));

    nchunks = ceiln(ncells, 32);
    
    CC(d::alloc_pinned((void **) &c->avgvel, nchunks * sizeof(float3)));
    CC(d::HostGetDevicePointer((void **) &c->davgvel, c->avgvel, 0));

    c->f = c->sume = make_float3(0, 0, 0);
    c->olde = vtarget;

    MC(m::Allreduce(&ncells, &c->totncells, 1, MPI_INT, MPI_SUM, c->comm));
    
    reini_sampler(/**/ c);

    ini_dump(&c->fdump);
}

void fin(/**/ PidVCont *c) {
    CC(d::Free(c->gridvel));
    CC(d::FreeHost(c->avgvel));
    MC(m::Comm_free(&c->comm));
    fin_dump(c->fdump);
}

void sample(int n, const Particle *pp, const int *starts, const int *counts, /**/ PidVCont *c) {
    int3 L = c->L;
    
    dim3 block(8, 8, 1);
    dim3 grid(ceiln(L.x, block.x),
              ceiln(L.y, block.y),
              ceiln(L.z, block.z));

    KL(dev::sample, (grid, block), (L, starts, counts, (float2 *) pp, /**/ c->gridvel));
    
    c->nsamples ++;
}

float3 adjustF(/**/ PidVCont *c) {
    int3 L = c->L;
    int ncells, nchunks;
    ncells = L.x * L.y * L.z;
    nchunks = ceiln(ncells, 32);

    KL(dev::reduceByWarp, (nchunks, 32), (c->gridvel, ncells, /**/ c->davgvel));
    dSync();

    float3 vcur = make_float3(0, 0, 0), e, de;

    for (int i = 0; i < nchunks; ++i)
        add(c->avgvel + i, /**/ &vcur);

    MC(m::Allreduce(MPI_IN_PLACE, &vcur.x, 3, MPI_FLOAT, MPI_SUM, c->comm));

    const float fac = 1.0 / (c->totncells * c->nsamples);
    
    scal(fac, /**/ &vcur);

    c->current = vcur;

    diff(&c->target, &vcur, /**/ &e);
    diff(&e, &c->olde, /**/ &de);
    add(&e, /**/ &c->sume);

    c->f = make_float3(0, 0, 0);

    axpy(c->factor * c->Kp, &e,       /**/ &c->f);
    axpy(c->factor * c->Ki, &c->sume, /**/ &c->f);
    axpy(c->factor * c->Kd, &de,      /**/ &c->f);

    reini_sampler(/**/c);

    c->olde = e;
    return c->f;
}

void log(const PidVCont *c) {
    if (m::rank) return;
    float3 v = c->current;
    float3 f = c->f;
    fprintf(c->fdump, "%.g %.g %.g %.g %.g %.g\n",
            v.x, v.y, v.z, f.x, f.y, f.z);
}
