namespace dpd {
void init0() {
    firstpost = true;
    nactive = 26;
    safety_factor =
        getenv("HEX_COMM_FACTOR") ? atof(getenv("HEX_COMM_FACTOR")) : 1.2;

    MC(l::m::Comm_dup(m::cart, &cart));

    for (int i = 0; i < 26; ++i) {
        int d[3] = {(i + 2) % 3 - 1, (i / 3 + 2) % 3 - 1, (i / 9 + 2) % 3 - 1};
        recv_tags[i] = (2 - d[0]) % 3 + 3 * ((2 - d[1]) % 3 + 3 * ((2 - d[2]) % 3));
        int coordsneighbor[3];
        for (int c = 0; c < 3; ++c) coordsneighbor[c] = m::coords[c] + d[c];
        MC(l::m::Cart_rank(cart, coordsneighbor, dstranks + i));
        halosize[i].x = d[0] != 0 ? 1 : XS;
        halosize[i].y = d[1] != 0 ? 1 : YS;
        halosize[i].z = d[2] != 0 ? 1 : ZS;

        int nhalocells = halosize[i].x * halosize[i].y * halosize[i].z;

        int estimate = numberdensity * safety_factor * nhalocells;
        estimate = 32 * ((estimate + 31) / 32);

        recvhalos[i]->setup(estimate, nhalocells);
        sendhalos[i]->setup(estimate, nhalocells);
    }

    CC(cudaHostAlloc((void **)&required_send_bag_size_host, sizeof(int) * 26,
                     cudaHostAllocMapped));
    CC(cudaHostGetDevicePointer(&required_send_bag_size,
                                required_send_bag_size_host, 0));
    CC(cudaEventCreateWithFlags(&evfillall, cudaEventDisableTiming));
    CC(cudaEventCreateWithFlags(&evdownloaded,
                                cudaEventDisableTiming | cudaEventBlockingSync));
}

void init1_one(int i) {
          int d[3] = {(i + 2) % 3 - 1, (i / 3 + 2) % 3 - 1, (i / 9 + 2) % 3 - 1};

        int coordsneighbor[3];
        for (int c = 0; c < 3; ++c)
        coordsneighbor[c] = (m::coords[c] + d[c] + m::dims[c]) % m::dims[c];

        int indx[3];
        for (int c = 0; c < 3; ++c)
        indx[c] = min(m::coords[c], coordsneighbor[c]) * m::dims[c] +
            max(m::coords[c], coordsneighbor[c]);

        int interrank_seed_base =
            indx[0] + m::dims[0] * m::dims[0] * (indx[1] + m::dims[1] * m::dims[1] * indx[2]);

        int interrank_seed_offset;

        {
            bool isplus =
                d[0] + d[1] + d[2] > 0 ||
                d[0] + d[1] + d[2] == 0 &&
                (d[0] > 0 || d[0] == 0 && (d[1] > 0 || d[1] == 0 && d[2] > 0));

            int mysign = 2 * isplus - 1;

            int v[3] = {1 + mysign * d[0], 1 + mysign * d[1], 1 + mysign * d[2]};

            interrank_seed_offset = v[0] + 3 * (v[1] + 3 * v[2]);
        }

        int interrank_seed = interrank_seed_base + interrank_seed_offset;

        interrank_trunks[i] = new l::rnd::d::KISS(390 + interrank_seed, interrank_seed + 615, 12309, 23094);
        int dstrank = dstranks[i];

        if (dstrank != m::rank)
        interrank_masks[i] = min(dstrank, m::rank) == m::rank;
        else {
            int alter_ego =
                (2 - d[0]) % 3 + 3 * ((2 - d[1]) % 3 + 3 * ((2 - d[2]) % 3));
            interrank_masks[i] = min(i, alter_ego) == i;
        }
}

void init1() {
  int i;
  for (i = 0; i < 26; ++i) init1_one(i);
}

void ini() {
    local_trunk = new l::rnd::d::KISS(0, 0, 0, 0);
    for (int i = 0; i < 26; i++) recvhalos[i] = new RecvHalo;
    for (int i = 0; i < 26; i++) sendhalos[i] = new SendHalo;

    init0();
    init1();
}
}
