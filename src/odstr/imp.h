void waitall(MPI_Request *reqs) {
    MPI_Status statuses[26];
    l::m::Waitall(26, reqs, statuses) ;
}

void post_recv(const MPI_Comm cart, const int rank[],
               MPI_Request *size_req, MPI_Request *mesg_req, Recv *r) {
    for(int i = 1, c = 0; i < 27; ++i)
    l::m::Irecv(r->size + i, 1, MPI_INTEGER, rank[i],
                BT_C_ODSTR + r->tags[i], cart, size_req + c++);

    for(int i = 1, c = 0; i < 27; ++i)
    l::m::Irecv(r->pp.hst[i], MAX_PART_NUM, MPI_FLOAT, rank[i],
                BT_P_ODSTR + r->tags[i], cart, mesg_req + c++);
}

void post_recv_ii(const MPI_Comm cart, const int rank[], MPI_Request *ii_req, Recv *r) {
    for(int i = 1, c = 0; i < 27; ++i)
    l::m::Irecv(r->ii.hst[i], MAX_PART_NUM, MPI_INT, rank[i],
                BT_I_ODSTR + r->tags[i], cart, ii_req + c++);
}

void halo(const Particle *pp, int n, Send *s) {
    CC(cudaMemset(s->size_dev, 0,  27*sizeof(s->size_dev[0])));
    dev::halo<<<k_cnf(n)>>>(pp, n, /**/ s->iidx, s->size_dev);
}

void scan(int n, Send *s) {
    dev::scan<<<1, 32>>>(n, s->size_dev, /**/ s->strt, s->size_pin->DP);
    dSync();
}

void pack_pp(const Particle *pp, int n, Send *s) {
    dev::pack<float2, 3> <<<k_cnf(3*n)>>>((float2*)pp, s->iidx, s->strt, /**/ s->pp.dev);
}

void pack_ii(const int *ii, int n, Send *s) {
    dev::pack<int, 1> <<<k_cnf(n)>>>(ii, s->iidx, s->strt, /**/ s->ii.dev);
}

int send_sz(MPI_Comm cart, const int rank[], /**/ Send *s, MPI_Request *req) {
    for(int i = 0; i < 27; ++i) s->size[i] = s->size_pin->D[i];
    for(int i = 1, cnt = 0; i < 27; ++i)
    l::m::Isend(s->size + i, 1, MPI_INTEGER, rank[i],
                BT_C_ODSTR + i, cart, &req[cnt++]);
    return s->size[0]; /* `n' bulk */
}

void send_pp(MPI_Comm cart, const int rank[], /**/ Send *s, MPI_Request *req) {
    for(int i = 1, cnt = 0; i < 27; ++i)
    l::m::Isend(s->pp.hst[i], s->size[i] * 6, MPI_FLOAT, rank[i],
                BT_P_ODSTR + i, cart, &req[cnt++]);
}

void send_ii(MPI_Comm cart, const int rank[], /**/ Send *s, MPI_Request *req) {
    for(int i = 1, cnt = 0; i < 27; ++i)
    l::m::Isend(s->ii.hst[i], s->size[i], MPI_INT, rank[i],
                BT_I_ODSTR + i, cart, &req[cnt++]);
}

void recv_count(/**/ Recv *r, int *nhalo) {
    int i;
    static int size[27], strt[28];

    size[0] = strt[0] = 0;
    for (i = 1; i < 27; ++i)    size[i] = r->size[i];
    for (i = 1; i < 28; ++i)    strt[i] = strt[i - 1] + size[i - 1];
    CC(cudaMemcpy(r->strt,    strt,    sizeof(strt),    H2D));
    *nhalo = strt[27];
}

void unpack_pp(const int n, /**/ Recv *r, Particle *pp_re) {
    dev::unpack<float2, 3> <<<k_cnf(3*n)>>> (r->pp.dev, r->strt, /**/ (float2*) pp_re);
}

void unpack_ii(const int n, /**/ Recv *r, int *ii_re) {
    dev::unpack<int, 1> <<<k_cnf(n)>>> (r->ii.dev, r->strt, /**/ ii_re);
}

// TODO rm this
void Distr::subindex_remote(int n, /*io*/ Particle *pp_re, int *counts, /**/ uchar4 *subi) {
    dev::subindex_remote <<<k_cnf(n)>>> (n, r.strt, /*io*/ (float2*) pp_re, counts, /**/ subi);
}

// TODO rm this
void Distr::cancel_recv(MPI_Request *size_req, MPI_Request *mesg_req) {
    for(int i = 0; i < 26; ++i) l::m::Cancel(size_req + i) ;
    for(int i = 0; i < 26; ++i) l::m::Cancel(mesg_req + i) ;
}
