namespace rex {
void _not_nan(float*, int) {};
void bind_solutes(std::vector<ParticlesWrap> wsolutes_) {wsolutes = wsolutes_;}
void _adjust_packbuffers() {
    int s = 0;
    for (int i = 0; i < 26; ++i) s += 32 * ((local[i]->capacity() + 31) / 32);
    packbuf->resize(s);
    host_packbuf->resize(s);
}

void ini() {
    iterationcount = -1;
    packstotalstart = new DeviceBuffer<int>(27);
    host_packstotalstart = new PinnedHostBuffer<int>(27);
    host_packstotalcount = new PinnedHostBuffer<int>(26);

    packscount = new DeviceBuffer<int>;
    packsstart = new DeviceBuffer<int>;
    packsoffset = new DeviceBuffer<int>;

    packbuf = new DeviceBuffer<Particle>;
    host_packbuf = new PinnedHostBuffer<Particle>;

    for (int i = 0; i < SE_HALO_SIZE; i++) local[i] = new LocalHalo;
    for (int i = 0; i < SE_HALO_SIZE; i++) remote[i] = new RemoteHalo;

    MC(l::m::Comm_dup(m::cart, &cart));
    for (int i = 0; i < 26; ++i) {
        int d[3] = {(i + 2) % 3 - 1, (i / 3 + 2) % 3 - 1, (i / 9 + 2) % 3 - 1};

        recv_tags[i] = (2 - d[0]) % 3 + 3 * ((2 - d[1]) % 3 + 3 * ((2 - d[2]) % 3));

        int coordsneighbor[3];
        for (int c = 0; c < 3; ++c) coordsneighbor[c] = m::coords[c] + d[c];

        MC(l::m::Cart_rank(cart, coordsneighbor, dstranks + i));

        int estimate = 1;
        remote[i]->preserve_resize(estimate);
        local[i]->resize(estimate);
        local[i]->update();

        CC(cudaMemcpyToSymbol(k_rex::ccapacities,
                              &local[i]->scattered_indices->C, sizeof(int),
                              sizeof(int) * i, H2D));
        CC(cudaMemcpyToSymbol(k_rex::scattered_indices,
                              &local[i]->scattered_indices->D, sizeof(int *),
                              sizeof(int *) * i, H2D));
    }

    _adjust_packbuffers();

    CC(cudaEventCreateWithFlags(&evPpacked,
                                cudaEventDisableTiming | cudaEventBlockingSync));
    CC(cudaEventCreateWithFlags(&evAcomputed,
                                cudaEventDisableTiming | cudaEventBlockingSync));
}

void _wait(std::vector<MPI_Request> &v) {
    MPI_Status statuses[v.size()];
    if (v.size()) MC(l::m::Waitall(v.size(), &v.front(), statuses));
    v.clear();
}

void _postrecvC() {
    for (int i = 0; i < 26; ++i) {
        MPI_Request reqC;
        MC(l::m::Irecv(recv_counts + i, 1, MPI_INTEGER, dstranks[i],
                     BT_C_REX + recv_tags[i], cart, &reqC));
        reqrecvC.push_back(reqC);
    }
}

void _postrecvP() {
    for (int i = 0; i < 26; ++i) {
        MPI_Request reqP;
        remote[i]->pmessage.resize(remote[i]->expected());
        MC(l::m::Irecv(&remote[i]->pmessage.front(), remote[i]->expected() * 6,
                     MPI_FLOAT, dstranks[i], BT_P_REX + recv_tags[i],
                     cart, &reqP));
        reqrecvP.push_back(reqP);
    }
}

void _postrecvA() {
    for (int i = 0; i < 26; ++i) {
        MPI_Request reqA;

        MC(l::m::Irecv(local[i]->result->D, local[i]->result->S * 3,
                     MPI_FLOAT, dstranks[i], BT_A_REX + recv_tags[i],
                     cart, &reqA));
        reqrecvA.push_back(reqA);
    }
}

void _pack_attempt() {


    if (packscount->S)
    CC(cudaMemsetAsync(packscount->D, 0, sizeof(int) * packscount->S));

    if (packsoffset->S)
    CC(cudaMemsetAsync(packsoffset->D, 0, sizeof(int) * packsoffset->S));

    if (packsstart->S)
    CC(cudaMemsetAsync(packsstart->D, 0, sizeof(int) * packsstart->S));

    k_rex::ini<<<1, 1>>>();
    for (int i = 0; i < (int) wsolutes.size(); ++i) {
        ParticlesWrap it = wsolutes[i];
        if (it.n) {
            CC(cudaMemcpyToSymbolAsync(k_rex::coffsets, packsoffset->D + 26 * i,
                                       sizeof(int) * 26, 0, D2D));
            k_rex::scatter_indices<<<k_cnf(it.n)>>>
                ((float2 *)it.p, it.n, /**/ packscount->D + i * 26);
        }
        k_rex::tiny_scan<<<1, 32>>>
            (packscount->D + i * 26, packsoffset->D + 26 * i,
             packsoffset->D + 26 * (i + 1), packsstart->D + i * 27);
    }

    CC(cudaMemcpyAsync(host_packstotalcount->D,
                       packsoffset->D + 26 * wsolutes.size(), sizeof(int) * 26,
                       H2H));

    k_rex::tiny_scan<<<1, 32>>>
        (packsoffset->D + 26 * wsolutes.size(), NULL, NULL, packstotalstart->D);

    CC(cudaMemcpyAsync(host_packstotalstart->D, packstotalstart->D,
                       sizeof(int) * 27, H2H));

    CC(cudaMemcpyToSymbolAsync(k_rex::cbases, packstotalstart->D,
                               sizeof(int) * 27, 0, D2D));
    for (int i = 0; i < (int) wsolutes.size(); ++i) {
        ParticlesWrap it = wsolutes[i];

        if (it.n) {
            CC(cudaMemcpyToSymbolAsync(k_rex::coffsets, packsoffset->D + 26 * i,
                                       sizeof(int) * 26, 0, D2D));
            CC(cudaMemcpyToSymbolAsync(k_rex::ccounts, packscount->D + 26 * i,
                                       sizeof(int) * 26, 0, D2D));
            CC(cudaMemcpyToSymbolAsync(k_rex::cpaddedstarts,
                                       packsstart->D + 27 * i, sizeof(int) * 27, 0,
                                       D2D));

            k_rex::pack<<<14 * 16, 128>>>
                ((float2 *)it.p, it.n, (float2 *)packbuf->D, packbuf->C, i);
        }
    }

    CC(cudaEventRecord(evPpacked));


}

void pack_p() {
    if (wsolutes.size() == 0) return;

    ++iterationcount;

    packscount->resize(26 * wsolutes.size());
    packsoffset->resize(26 * (wsolutes.size() + 1));
    packsstart->resize(27 * wsolutes.size());

    _pack_attempt();
}

void post_p() {
    if (wsolutes.size() == 0) return;



    // consolidate the packing
    {
        CC(cudaEventSynchronize(evPpacked));

        if (iterationcount == 0)
        _postrecvC();
        else
        _wait(reqsendC);

        for (int i = 0; i < 26; ++i) send_counts[i] = host_packstotalcount->D[i];

        bool packingfailed = false;

        for (int i = 0; i < 26; ++i)
        packingfailed |= send_counts[i] > local[i]->capacity();

        if (packingfailed) {
            for (int i = 0; i < 26; ++i) local[i]->resize(send_counts[i]);

            int newcapacities[26];
            for (int i = 0; i < 26; ++i) newcapacities[i] = local[i]->capacity();

            CC(cudaMemcpyToSymbolAsync(k_rex::ccapacities, newcapacities,
                                       sizeof(newcapacities), 0,
                                       H2D));

            int *newindices[26];
            for (int i = 0; i < 26; ++i) newindices[i] = local[i]->scattered_indices->D;

            CC(cudaMemcpyToSymbolAsync(k_rex::scattered_indices, newindices,
                                       sizeof(newindices), 0, H2D));

            _adjust_packbuffers();

            _pack_attempt();

            dSync(); /* was CC(cudaStreamSynchronize(stream)); */
        }

        for (int i = 0; i < 26; ++i) local[i]->resize(send_counts[i]);

        _postrecvA();

        if (iterationcount == 0) {
            _postrecvP();
        } else
        _wait(reqsendP);

        if (host_packstotalstart->D[26]) {
            CC(cudaMemcpyAsync(host_packbuf->D, packbuf->D,
                               sizeof(Particle) * host_packstotalstart->D[26],
                               H2H));
        }
        dSync(); /* was CC(cudaStreamSynchronize(downloadstream)); */
    }

    // post the sending of the packs
    {
        reqsendC.resize(26);

        for (int i = 0; i < 26; ++i)
        MC(l::m::Isend(send_counts + i, 1, MPI_INTEGER, dstranks[i],
                     BT_C_REX + i, cart, &reqsendC[i]));

        for (int i = 0; i < 26; ++i) {
            int start = host_packstotalstart->D[i];
            int count = send_counts[i];
            int expected = local[i]->expected();

            MPI_Request reqP;

            _not_nan((float *)(host_packbuf->D + start), count * 6);

            MC(l::m::Isend(host_packbuf->D + start, expected * 6, MPI_FLOAT,
                         dstranks[i], BT_P_REX + i, cart, &reqP));
            reqsendP.push_back(reqP);

            if (count > expected) {
                MPI_Request reqP2;
                MC(l::m::Isend(host_packbuf->D + start + expected,
                             (count - expected) * 6, MPI_FLOAT, dstranks[i],
                             BT_P2_REX + i, cart, &reqP2));

                reqsendP.push_back(reqP2);
            }
        }
    }
}

void recv_p() {
    if (wsolutes.size() == 0) return;

    _wait(reqrecvC);
    _wait(reqrecvP);

    for (int i = 0; i < 26; ++i) {
        int count = recv_counts[i];
        int expected = remote[i]->expected();

        remote[i]->pmessage.resize(max(1, count));
        remote[i]->preserve_resize(count);
        MPI_Status status;

        if (count > expected)
        MC(MPI_Recv(&remote[i]->pmessage.front() + expected,
                    (count - expected) * 6, MPI_FLOAT, dstranks[i],
                    BT_P2_REX + recv_tags[i], cart, &status));

        memcpy(remote[i]->hstate.D, &remote[i]->pmessage.front(),
               sizeof(Particle) * count);
        _not_nan((float *)remote[i]->hstate.D, count * 6);
    }

    _postrecvC();

    for (int i = 0; i < 26; ++i)
    CC(cudaMemcpyAsync(remote[i]->dstate.D, remote[i]->hstate.D,
                       sizeof(Particle) * remote[i]->hstate.S,
                       H2D));
}

void halo() {
    if (wsolutes.size() == 0) return;

    if (iterationcount) _wait(reqsendA);

    ParticlesWrap halos[26];

    for (int i = 0; i < 26; ++i)
    halos[i] = ParticlesWrap(remote[i]->dstate.D, remote[i]->dstate.S,
                             remote[i]->result.DP);

    dSync(); /* was CC(cudaStreamSynchronize(uploadstream)); */

    /** here was visitor  **/
    if (fsiforces)     fsi::halo(halos);
    if (contactforces) cnt::halo(halos);
    /***********************/



    CC(cudaEventRecord(evAcomputed));

    for (int i = 0; i < 26; ++i) local[i]->update();

    _postrecvP();
}

void post_f() {
    if (wsolutes.size() == 0) return;

    CC(cudaEventSynchronize(evAcomputed));

    reqsendA.resize(26);
    for (int i = 0; i < 26; ++i)
    MC(l::m::Isend(remote[i]->result.D, remote[i]->result.S * 3,
                 MPI_FLOAT, dstranks[i], BT_A_REX + i, cart,
                 &reqsendA[i]));
}

void recv_f() {
    if (wsolutes.size() == 0) return;

    {
        float *recvbags[26];

        for (int i = 0; i < 26; ++i) recvbags[i] = (float *)local[i]->result->DP;

        CC(cudaMemcpyToSymbolAsync(k_rex::recvbags, recvbags, sizeof(recvbags),
                                   0, H2D));
    }

    _wait(reqrecvA);

    for (int i = 0; i < (int) wsolutes.size(); ++i) {
        ParticlesWrap it = wsolutes[i];

        if (it.n) {
            CC(cudaMemcpyToSymbolAsync(k_rex::cpaddedstarts,
                                       packsstart->D + 27 * i, sizeof(int) * 27, 0,
                                       D2D));
            CC(cudaMemcpyToSymbolAsync(k_rex::ccounts, packscount->D + 26 * i,
                                       sizeof(int) * 26, 0, D2D));
            CC(cudaMemcpyToSymbolAsync(k_rex::coffsets, packsoffset->D + 26 * i,
                                       sizeof(int) * 26, 0, D2D));

            k_rex::unpack<<<16 * 14, 128>>>((float *)it.f, it.n);
        }

    }
}

void fin() {
    MC(l::m::Comm_free(&cart));

    CC(cudaEventDestroy(evPpacked));
    CC(cudaEventDestroy(evAcomputed));

    delete packstotalstart;
    delete host_packstotalstart;
    delete host_packstotalcount;

    delete packscount;
    delete packsstart;
    delete packsoffset;
    delete packbuf;
    delete host_packbuf;

    for (int i = 0; i < SE_HALO_SIZE; i++) delete local[i];
    for (int i = 0; i < SE_HALO_SIZE; i++) delete remote[i];
}
}
