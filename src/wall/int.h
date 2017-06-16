struct Quants {
    float4 *pp;
    int n;
// };
// struct Ticket {
    Logistic::KISS *rnd;
    Clist *cells;
    Texo<int> texstart;
    Texo<float4> texpp;
};

void alloc_quants(Quants *q) {
    q->n = 0;
    q->pp = NULL;
    q->rnd   = new Logistic::KISS;
    q->cells = new Clist(XS + 2 * XWM, YS + 2 * YWM, ZS + 2 * ZWM);
}

void free_quants(Quants *q) {
    if (q->pp) CC(cudaFree(q->pp));
    delete q->cells;
    delete q->rnd;
}

int create(int n, Particle* pp, Quants *q) {
    Particle *frozen;
    CC(cudaMalloc(&frozen, sizeof(Particle) * MAX_PART_NUM));

    n = sub::init(pp, n, frozen, &q->n);
    sub::build_cells(q->n, /**/ frozen, q->cells);

    MSG0("consolidating wall particles");

    CC(cudaMalloc(&q->pp, q->n * sizeof(float4)));

    if (q->n > 0)
    sub::dev::strip_solid4 <<<k_cnf(q->n)>>> (frozen, q->n, /**/ q->pp);

    q->texstart.setup(q->cells->start, q->cells->ncells);
    q->texpp.setup(q->pp, q->n);
    
    CC(cudaFree(frozen));
    return n;
}

void interactions(const Quants q, const int type, const Particle *pp, const int n, Force *ff) {
    sub::interactions(type, pp, n, q.texstart, q.texpp, q.n, /**/ q.rnd, ff);
}
