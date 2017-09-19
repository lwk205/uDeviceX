void unpack_bulk(const Pack *p, /**/ rbc::Quants *q) {
    int nc, nv, n;
    nc = p->hpp.counts[frag_bulk];
    nv = q->nv;
    n = nc * nv;
    
    if (n) CC(d::MemcpyAsync(q->pp, p->dpp.data[frag_bulk], n * sizeof(Particle), D2D));
    q->nc = nc;
    q->n = n;
}

void unpack_halo(const Unpack *u, /**/ rbc::Quants *q) {
    int nc, nv, n, i, s, nctot;
    size_t size;
    nv = q->nv;
    s = q->n;
    nctot = q->nc;
    
    for (i = 0; i < NFRAGS; ++i) {
        nc = u->hpp.counts[i];
        n  = nc * nv; 
        size = n * sizeof(Particle);
        if (nc) {
            CC(d::MemcpyAsync(q->pp + s, u->hpp.data[i], size, H2D));
            KL(dev::shift_one_frag, (k_cnf(n)), (n, i, /**/ q->pp + s));
        }
        s += n;
        nctot += nc;
    }
    q->n = s;
    q->nc = nctot;
}
