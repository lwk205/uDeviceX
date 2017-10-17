#define REMOTE (true)
#define LOCAL (false)

void ini_counts(/**/ Clist *c) {
    if (c->ncells) CC(d::MemsetAsync(c->counts, 0, c->ncells * sizeof(int)));
}

static void subindex(int n, const Particle *pp, int3 dims, /**/ int *cc, uchar4 *ee) {
    if (n) KL(dev::subindex, (k_cnf(n)), (dims, n, pp, /*io*/ cc, /**/ ee));
}

void subindex_local(int n, const Particle *pp, /**/ Clist *c, Map *t) {
    subindex(n, pp, c->dims, /**/ c->counts, t->eelo);
}

void subindex_remote(int n, const Particle *pp, /**/ Clist *c, Map *t) {
    subindex(n, pp, c->dims, /**/ c->counts, t->eere);
}

void build_map(int nlo, int nre, /**/ Clist *c, Map *t) {
    int nc, *cc, *ss;
    uchar4 *eelo, *eere;
    uint *ii = t->ii;
    int3 dims = c->dims;
    nc = c->ncells;
    cc = c->counts;
    ss = c->starts;
    eelo = t->eelo;
    eere = t->eere;
        
    scan::scan(cc, nc, /**/ ss, /*w*/ &t->scan);

    /* used for debugging purpose */    
    // if (nlo + nre) KL(dev::ini_ids, (k_cnf(nlo+nre)), (nlo+nre, /**/ ii));
   
    if (nlo) KL(dev::get_ids, (k_cnf(nlo)), (LOCAL,  dims, nlo, ss, eelo, /**/ ii));
    if (nre) KL(dev::get_ids, (k_cnf(nre)), (REMOTE, dims, nre, ss, eere, /**/ ii));    
}

void gather_pp(const Particle *pplo, const Particle *ppre, const Map *t, int nout, /**/ Particle *ppout) {
    if (nout) KL(dev::gather, (k_cnf(nout)), (pplo, ppre, t->ii, nout, /**/ ppout));
}

void gather_ii(const int *iilo, const int *iire, const Map *t, int nout, /**/ int *iiout) {
    if (nout) KL(dev::gather, (k_cnf(nout)), (iilo, iire, t->ii, nout, /**/ iiout));
}

void build(int nlo, int nout, const Particle *pplo, /**/ Particle *ppout, Clist *c, Map *t) {
    build(nlo, 0, nout, pplo, NULL, /**/ ppout, c, t);
}

void build(int nlo, int nre, int nout, const Particle *pplo, const Particle *ppre, /**/ Particle *ppout, Clist *c, Map *t) {
    ini_counts(/**/ c);
    subindex_local (nlo, pplo, /**/ c, t);
    subindex_remote(nre, ppre, /**/ c, t);
    build_map(nlo, nre, /**/ c, t);    
    gather_pp(pplo, ppre, t, nout, ppout);
}

#undef REMOTE
#undef LOCAL
