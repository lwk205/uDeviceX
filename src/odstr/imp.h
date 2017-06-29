void Distr::waitall(MPI_Request *reqs) {
    MPI_Status statuses[128]; /* big number */
    l::m::Waitall(26, reqs, statuses) ;
}

void Distr::post_recv(MPI_Comm cart, int rank[],
                      MPI_Request *size_req, MPI_Request *mesg_req) {
    for(int i = 1, c = 0; i < 27; ++i)
    l::m::Irecv(r.size + i, 1, MPI_INTEGER, rank[i],
                BT_C_ODSTR + r.tags[i], cart, size_req + c++);

    for(int i = 1, c = 0; i < 27; ++i)
    l::m::Irecv(r.hst[i], MAX_PART_NUM, MPI_FLOAT, rank[i],
                BT_P_ODSTR + r.tags[i], cart, mesg_req + c++);
}

void Distr::halo(Particle *pp, int n) {
    CC(cudaMemset(s.size_dev, 0,  27*sizeof(s.size_dev[0])));
    dev::halo<<<k_cnf(n)>>>(pp, n, /**/ s.iidx, s.size_dev);
}

void Distr::scan(int n) {
    dev::scan<<<1, 32>>>(n, s.size_dev, /**/ s.strt, s.size_pin->DP);
    dSync();
}

void Distr::pack(Particle *pp, int n) {
    dev::pack<<<k_cnf(3*n)>>>((float2*)pp, s.iidx, s.strt, /**/ s.dev);
    dSync();
}

int Distr::send_sz(MPI_Comm cart, int rank[], MPI_Request *req) {
    for(int i = 0; i < 27; ++i) s.size[i] = s.size_pin->D[i];
    for(int i = 1, cnt = 0; i < 27; ++i)
    l::m::Isend(s.size + i, 1, MPI_INTEGER, rank[i],
                BT_C_ODSTR + i, cart, &req[cnt++]);
    return s.size[0]; /* `n' bulk */
}

void Distr::send_msg(MPI_Comm cart, int rank[], MPI_Request *req) {
    for(int i = 1, cnt = 0; i < 27; ++i)
    l::m::Isend(s.hst[i], s.size[i] * 6, MPI_FLOAT, rank[i],
                BT_P_ODSTR + i, cart, &req[cnt++]);
}

void Distr::recv_count(int *nhalo) {
    int i;
    static int size[27], strt[28];

    size[0] = strt[0] = 0;
    for (i = 1; i < 27; ++i)    size[i] = r.size[i];
    for (i = 1; i < 28; ++i)    strt[i] = strt[i - 1] + size[i - 1];
    CC(cudaMemcpy(r.strt,    strt,    sizeof(strt),    H2D));
    *nhalo = strt[27];
}

void Distr::unpack(int n, /*io*/ int *counts, /*o*/ uchar4 *subi, Particle *pp_re) {
    dev::unpack <<<k_cnf(3*n)>>> (r.dev, r.strt, /**/ (float2*) pp_re);
    dev::subindex_remote <<<k_cnf(n)>>> (n, r.strt, /*io*/ (float2*) pp_re, counts, /**/ subi);
}

void Distr::cancel_recv(MPI_Request *size_req, MPI_Request *mesg_req) {
    for(int i = 0; i < 26; ++i) l::m::Cancel(size_req + i) ;
    for(int i = 0; i < 26; ++i) l::m::Cancel(mesg_req + i) ;
}
