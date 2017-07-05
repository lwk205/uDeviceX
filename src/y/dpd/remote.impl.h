namespace dpd {
void upd_bag(SendHalo* sendhalos[]) {
    for (int i = 0; i < 26; ++i) {
        frag::start.d[i] = sendhalos[i]->tmpstart->D;
        frag::count.d[i] = sendhalos[i]->tmpcount->D;
        frag::scan.d[i] = sendhalos[i]->dcellstarts->D;
        frag::capacity.d[i] = sendhalos[i]->dbuf->C;
        frag::indices.d[i] = sendhalos[i]->scattered_entries->D;
        frag::pp.d[i] = sendhalos[i]->dbuf->D;
    }
}

void post_expected_recv(MPI_Comm cart, RecvHalo* recvhalos[]) {
    for (int i = 0, c = 0; i < 26; ++i) {
        if (recvhalos[i]->expected)
        MC(l::m::Irecv(recvhalos[i]->hbuf->D, recvhalos[i]->expected,
                       Particle::datatype(), dstranks[i], BT_P_DPD + recv_tags[i],
                       cart, recvreq + c++));
    }
    for (int i = 0, c = 0; i < 26; ++i)
    if (recvhalos[i]->expected)
    MC(l::m::Irecv(recvhalos[i]->hcellstarts->D,
                   recvhalos[i]->hcellstarts->S, MPI_INTEGER, dstranks[i],
                   BT_CS_DPD + recv_tags[i], cart, recvcellsreq + c++));
  
    for (int i = 0, c = 0; i < 26; ++i)
    if (recvhalos[i]->expected)
    MC(l::m::Irecv(recv_counts + i, 1, MPI_INTEGER, dstranks[i],
                   BT_C_DPD + recv_tags[i], cart, recvcountreq + c++));
    else
    recv_counts[i] = 0;
}

void cancel_recv() {
    int i;
    for (i = 0; i < 26; ++i) MC(MPI_Cancel(recvreq + i));
    for (i = 0; i < 26; ++i) MC(MPI_Cancel(recvcellsreq + i));
    for (i = 0; i < 26; ++i) MC(MPI_Cancel(recvcountreq + i));
}

void wait_send() {
    MPI_Status ss[26 * 2];
    MC(l::m::Waitall(26, sendcellsreq, ss));
    MC(l::m::Waitall(nsendreq, sendreq, ss));
    MC(l::m::Waitall(26, sendcountreq, ss));
}

void fin(bool first) {
    CC(cudaFreeHost(required_send_bag_size));
    if (!first) {
        wait_send();
        cancel_recv();
    }
    CC(cudaEventDestroy(evfillall));
    CC(cudaEventDestroy(evdownloaded));
}
}
