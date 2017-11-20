void ini_map(int nfrags, const int capacity[], /**/ Map *m) {
    CC(d::Malloc((void**) &m->counts,  nfrags      * sizeof(int)));
    CC(d::Malloc((void**) &m->starts, (nfrags + 1) * sizeof(int)));

    CC(d::alloc_pinned((void**) &m->hcounts,  nfrags      * sizeof(int)));
    
    int i, c;
    for (i = 0; i < nfrags; ++i) {
        c = capacity[i];
        if (c) CC(d::Malloc((void**) &m->ids[i], c * sizeof(int)));
    }
    
}

void fin_map(int nfrags, /**/ Map *m) {
    CC(d::Free(m->counts));
    CC(d::Free(m->starts));
    CC(d::FreeHost(m->hcounts));    
    for (int i = 0; i < nfrags; ++i)
        CC(d::Free(m->ids[i]));
}

void reini_map(int nfrags, /**/ Map m) {
    CC(d::MemsetAsync(m.counts, 0, nfrags * sizeof(int)));
}

void download_counts(int nfrags, /**/ Map *m) {
    CC(d::MemcpyAsync(m->hcounts, m->counts, nfrags * sizeof(int), D2H));
}

void ini_host_map(int nfrags, const int capacity[], /**/ Map *m) {
    emalloc( nfrags      * sizeof(int), (void**) &m->counts);
    emalloc((nfrags + 1) * sizeof(int), (void**) &m->starts);

    int i, c;
    for (i = 0; i < nfrags; ++i) {
        c = capacity[i];
        if (c)
            emalloc(c * sizeof(int), (void**) &m->ids[i]);
    }
}

void fin_host_map(int nfrags, /**/ Map *m) {
    free(m->counts);
    free(m->starts);    
    for (int i = 0; i < nfrags; ++i)
        free(m->ids[i]);
}

void reini_host_map(int nfrags, /**/ Map m) {
    memset(m.counts, 0, nfrags * sizeof(int));
}

void mapD2H(int nfrags, const Map *d, /**/ Map *h) {
    CC(d::MemcpyAsync(h->counts, d->counts,  nfrags      * sizeof(int), D2H));
    CC(d::MemcpyAsync(h->starts, d->starts, (nfrags + 1) * sizeof(int), D2H));

    dSync();
    
    int i, c;
    for (i = 0; i < nfrags; ++i) {
        c = h->counts[i];
        if (c)
            CC(d::MemcpyAsync(h->ids[i], d->ids[i], c * sizeof(int), D2H));
    }
}
