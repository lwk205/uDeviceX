static void pack_pp(int nfrags, int nw, const PaWrap *ww, Map map, /**/ Pap26 buf) {
    int i, stride;
    stride = nfrags + 1;
    const PaWrap *w;
    PackHelper ph;
    
    for (i = 0; i < nw; ++i) {
        w = ww + i;
        ph.starts  = map.starts  + i * stride;
        ph.offsets = map.offsets + i * stride;
        memcpy(ph.indices, map.ids, nfrags * sizeof(int*));

        KL(dev::pack_pp, (14 * 16, 128), (w->pp, ph, /**/ buf));
    }
}

void pack(int nw, const PaWrap *ww, /**/ Pack *p) {
    Pap26 wrap;
    bag2Sarray(p->dpp, &wrap);
    pack_pp(NFRAGS, nw, ww, p->map, /**/ wrap);
}

void download(int nw, Pack *p) {
    if (!nw) return;
    download_counts(nw, NFRAGS, p->map, /**/ p->hpp.counts);
}

static void clear_forces(int nfrags, /**/ PackF *p) {
    int i, c;
    size_t sz;
    for (i = 0; i < nfrags; ++i) {
        c = p->hff.counts[i];
        sz = c * p->hff.bsize;
        if (c) CC(d::MemsetAsync(p->dff.data[i], 0, sz));
    }   
}

Fop26 reini_ff(const Unpack *u, PackF *pf) {
    size_t sz = NBAGS * sizeof(int);
    memcpy(pf->hff.counts, u->hpp.counts, sz);
    clear_forces(NFRAGS, /**/ pf);

    Fop26 wrap;
    bag2Sarray(pf->dff, &wrap);
    return wrap;
}

void download_ff(PackF *p) {
    int i, c;
    size_t sz;
    data_t *src, *dst;
    for (i = 0; i < NFRAGS; ++i) {
        c = p->hff.counts[i];
        sz = c * p->hff.bsize;
        src = p->dff.data[i];
        dst = p->hff.data[i];
        if (c) CC(d::MemcpyAsync(dst, src, sz, D2H));
    }
    dSync();
}
