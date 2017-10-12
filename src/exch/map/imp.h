void ini_map(int nw, int nfrags, int cap[], /**/ Map *map) {
    int i, c;
    size_t sz;
    sz = (nw + 1) * (nfrags + 1) * sizeof(int);
    CC(d::Malloc((void**) &map->counts,  sz));
    CC(d::Malloc((void**) &map->starts,  sz));
    CC(d::Malloc((void**) &map->offsets, sz));

    for (i = 0; i < nfrags; ++i) {
        c = cap[i];
        sz = c * sizeof(int);
        CC(d::Malloc((void**) &map->ids[i], sz));
    }
}

void fin_map(int nfrags, Map *map) {
    CC(d::Free(map->counts));
    CC(d::Free(map->starts));
    CC(d::Free(map->offsets));

    for (int i = 0; i < nfrags; ++i)
        CC(d::Free(map->ids[i]));
}

void reini_map(int nw, int nfrags, /**/ Map map) {
    size_t sz;
    sz = (nw + 1) * (nfrags + 1) * sizeof(int);
    if (sz == 0) return;
    CC(d::MemsetAsync(map.counts,  0, sz));
    CC(d::MemsetAsync(map.starts,  0, sz));
    CC(d::MemsetAsync(map.offsets, 0, sz));
}

void scan_map(int nw, int nfrags, /**/ Map map) {
    int i, *cc, *ss, *oo, *oon, stride;
    stride = nfrags + 1;
    for (i = 0; i < nw; ++i) {
        cc  = map.counts  + i * stride;
        ss  = map.starts  + i * stride;
        oo  = map.offsets + i * stride;
        oon = map.offsets + (i + 1) * stride;
        KL(dev::scan2d, (1, 32), (cc, oo, /**/ oon, ss));
    }
}

void download_counts(int nw, Map map, /**/ int counts[]) {
    int *src;
    size_t sz = NBAGS * sizeof(int);
    src = map.offsets + nw * NBAGS;
    CC(d::Memcpy(counts, src, sz, D2H));
}
