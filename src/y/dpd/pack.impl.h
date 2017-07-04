namespace dpd {
void pack(Particle *p, int n, int *cellsstart, int *cellscount) {

    nlocal = n;
    if (firstpost) {
        {
            static int cellpackstarts[27];
            cellpackstarts[0] = 0;
            for (int i = 0, s = 0; i < 26; ++i)
            cellpackstarts[i + 1] =
                (s += sendhalos[i]->dcellstarts->S * (sendhalos[i]->expected > 0));
            ncells = cellpackstarts[26];
            CC(cudaMemcpyToSymbol(k_halo::cellpackstarts, cellpackstarts,
                                  sizeof(cellpackstarts), 0, H2D));
        }

        {
            static k_halo::CellPackSOA cellpacks[26];
            for (int i = 0; i < 26; ++i) {
                cellpacks[i].start = sendhalos[i]->tmpstart->D;
                cellpacks[i].count = sendhalos[i]->tmpcount->D;
                cellpacks[i].enabled = sendhalos[i]->expected > 0;
                cellpacks[i].scan = sendhalos[i]->dcellstarts->D;
                cellpacks[i].size = sendhalos[i]->dcellstarts->S;
            }
            CC(cudaMemcpyToSymbol(k_halo::cellpacks, cellpacks,
                                  sizeof(cellpacks), 0, H2D));
        }
    }

    if (ncells)
    k_halo::
        count_all<<<k_cnf(ncells)>>>(cellsstart, cellscount, ncells);

    k_halo::scan_diego<32><<<26, 32 * 32>>>();

    if (firstpost)
    post_expected_recv();
    else {
        MPI_Status statuses[26 * 2];
        MC(l::m::Waitall(nactive, sendcellsreq, statuses));
        MC(l::m::Waitall(nsendreq, sendreq, statuses));
        MC(l::m::Waitall(nactive, sendcountreq, statuses));
    }

    if (firstpost) {
        {
            static int *srccells[26];
            for (int i = 0; i < 26; ++i) srccells[i] = sendhalos[i]->dcellstarts->D;

            CC(cudaMemcpyToSymbol(k_halo::srccells, srccells, sizeof(srccells),
                                  0, H2D));

            static int *dstcells[26];
            for (int i = 0; i < 26; ++i)
            dstcells[i] = sendhalos[i]->hcellstarts->DP;

            CC(cudaMemcpyToSymbol(k_halo::dstcells, dstcells, sizeof(dstcells),
                                  0, H2D));
        }

        {
            static int *srccells[26];
            for (int i = 0; i < 26; ++i) srccells[i] = recvhalos[i]->hcellstarts->DP;
            CC(cudaMemcpyToSymbol(k_halo::srccells, srccells, sizeof(srccells),
                                  sizeof(srccells), H2D));

            static int *dstcells[26];
            for (int i = 0; i < 26; ++i) dstcells[i] = recvhalos[i]->dcellstarts->D;
            CC(cudaMemcpyToSymbol(k_halo::dstcells, dstcells, sizeof(dstcells),
                                  sizeof(dstcells), H2D));
        }
    }

    if (ncells) k_halo::copycells<0><<<k_cnf(ncells)>>>(ncells);
    _pack_all(p, n, firstpost);

}
}
