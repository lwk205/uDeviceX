namespace sub {
void copy_starts(rex::TicketPack tp, /**/ rex::TicketPinned ti) {
    CC(cudaMemcpyAsync(ti.starts, tp.tstarts, sizeof(int) * 27, D2H));
}

void copy_offset(int nw, rex::TicketPack tp, rex::TicketPinned ti) {
    CC(cudaMemcpyAsync(ti.counts, tp.offsets + 26 * nw, sizeof(int) * 26, D2H));
}

}
