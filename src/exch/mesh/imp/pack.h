static void pack_mesh(int nv, const Particle *pp, Map map, /**/ Pap26 buf) {
    KL(dev::pack_mesh, (14 * 16, 128), (nv, pp, map, /**/ buf));
}

void pack(int nv, const Particle *pp, /**/ Pack *p) {
    Pap26 wrap;
    bag2Sarray(p->dpp, &wrap);
    pack_mesh(nv, pp, p->map, /**/ wrap);
}

void download(Pack *p) {
    download_counts(1, p->map, /**/ p->hpp.counts);
}



static void compress_mom(int nt, int nm, const Momentum *mm, /**/ MMap *m, int *ids, Momentum *mmc) {
    KL(dev::subindex_compress, (k_cnf(nt * nm)), (nt, nm, mm, /**/ m->cc, m->subids));

    enum {NWRP=4, WRPSZ=32};
    enum {BLCK=1, THRD=NWRP*WRPSZ};
    KL(dev::block_scan<NWRP>, (BLCK, THRD), (nm, m->cc, /**/ m->ss));

    KL(dev::compress, (k_cnf(nt * nm)), (nt, nm, mm, m->ss, m->subids, /**/ ids, mmc));
}

static void pack_mom(int nt, const int counts[NFRAGS], const Momentum *mm,
                     /**/ MMap maps[NFRAGS], int *ibuf[NFRAGS], Momentum *mbuf[NFRAGS]) {
    int i, s, c;

    for (i = s = 0; i < NFRAGS; ++i) {
        c = counts[i];
        compress_mom(nt, c, mm + nt * s, /**/ maps + i, ibuf[i], mbuf[i]);
    }
}

void packM(int nt, const int counts[NFRAGS], const Momentum *mm, /**/ PackM *p) {
    pack_mom(nt, counts, mm, /**/ p->maps, (int **) p->dii.data, (Momentum **) p->dmm.data);
}

void downloadM(const int counts[NFRAGS], PackM *p) {
    dSync();
}

