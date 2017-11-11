static int unpack_bulk_pp(const Pack *p, /**/ Particle *pp) {
    int nc = p->hpp.counts[frag_bulk];
    void *src = p->dpp.data[frag_bulk];
    size_t sz = nc * p->hpp.bsize;
    
    if (nc)
        CC(d::MemcpyAsync(pp, src, sz, D2D));
    return nc;
}

static void unpack_bulk_ii(const Pack *p, /**/ int *ii) {
    int nc = p->hii.counts[frag_bulk];
    void *src = p->hii.data[frag_bulk];
    memcpy(ii, src, nc * sizeof(int));
}

void unpack_bulk(const Pack *p, /**/ rbc::main::Quants *q) {
    int nc, nv, n;
    nv = q->nv;

    nc = unpack_bulk_pp(p, /**/ q->pp);
    n = nc * nv;

    if (rbc_ids)
        unpack_bulk_ii(p, /**/ q->ii);

    q->nc = nc;
    q->n = n;
}

static int unpack_halo_pp(int nc0, int nv, const hBags *hpp, /**/ Particle *pp) {
    void *src;
    Particle *dst;
    int i, s, c, n;
    size_t sz, bsz;
    bsz = hpp->bsize;
    
    s = nc0;
    for (i = 0; i < NFRAGS; ++i) {
        c   = hpp->counts[i];
        src = hpp->data[i];
        dst = pp + s * nv;
        
        if (c) {
            n = c * nv;
            sz = c * bsz;
            CC(d::MemcpyAsync(dst, src, sz, H2D));
            KL(dev::shift_one_frag, (k_cnf(n)), (n, i, /**/ dst));
        }
        s += c;
    }
    return s;
}

static void unpack_halo_ii(int nc0, const hBags *hii, /**/ int *ii) {
    void *src;
    int i, s, c;

    s = nc0;
    for (i = 0; i < NFRAGS; ++i) {
        c   = hii->counts[i];
        src = hii->data[i];        
        memcpy(ii + s, src, c * sizeof(int));
        s += c;
    }
}

void unpack_halo(const Unpack *u, /**/ rbc::main::Quants *q) {
    int nc0, nc, nv;

    nc0 = q->nc;
    nv  = q->nv;

    nc = unpack_halo_pp(nc0, nv, &u->hpp, /**/ q->pp);

    if (rbc_ids)
        unpack_halo_ii(nc0, &u->hii, /**/ q->ii);

    q->n = nc * nv;
    q->nc = nc;
}
