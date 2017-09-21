static void pack_pp(const Map m, int nc, int nv, const Particle *pp, /**/ dBags bags) {
    Sarray<Particle*, NBAGS> wrap;
    bag2Sarray(bags, &wrap);

    enum {THR=128};
    dim3 thrd(THR, 1);
    dim3 blck(ceiln(nv, THR), nc);
        
    KL((dev::pack_pp_packets), (blck, thrd), (nv, pp, m, /**/ wrap));
}

void pack_pp(int nc, int nv, const Particle *pp, /**/ Pack *p) {
    pack_pp(p->map, nc, nv,  pp, /**/ p->dpp);
}

void download(Pack *p) {
    CC(d::Memcpy(p->hpp.counts, p->map.counts, NBAGS * sizeof(int), D2H));
}
