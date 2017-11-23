#define CODE "rbc"
#define IDEXT "ids"

static void setup_from_strt(int nv, const int id, /**/ Particle *pp, int *nc, int *n, /*w*/ Particle *pp_hst) {
    restart::read_pp(CODE, id, pp_hst, n);
    *nc = *n / nv;
    
    if (*n) cH2D(pp, pp_hst, *n);
}

static void ids_from_strt(const int id, /**/ int *ii) {
    int nc;
    restart::read_ii(CODE, IDEXT, id, ii, &nc);
}

void strt_quants(const char *cell, const int id, Quants *q) {
    int md, nt, nv;
    md = RBCmd;
    nt = RBCnt;
    nv = RBCnv;
    setup(md, nt, nv, cell, /**/
          q->shape.anti, q->shape.edg, &q->shape.totArea,
          q->tri_hst, q->tri, q->adj0, q->adj1);
    setup_from_strt(nv, id, /**/ q->pp, &q->nc, &q->n, /*w*/ q->pp_hst);

    if (rbc_ids)
        ids_from_strt(id, /**/ q->ii);
}

static void strt_dump(const int id, const int n, const Particle *pp, /*w*/ Particle *pp_hst) {
    if (n) cD2H(pp_hst, pp, n);

    restart::write_pp(CODE, id, pp_hst, n);
}

static void strt_dump_ii(const int id, const int nc, const int *ii) {
    restart::write_ii(CODE, IDEXT, id, ii, nc);
}


void strt_dump(const int id, const Quants q) {
    strt_dump(id, q.n, q.pp, /*w*/ q.pp_hst);

    if (rbc_ids)
        strt_dump_ii(id, q.nc, q.ii);
}

#undef CODE
#undef IDEXT
