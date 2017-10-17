void halo(Pap26 PP, Fop26 FF, int counts[26]) {
    int i, s, n;
    int27 starts;
    starts.d[0] = 0;
    for (i = s = 0; i < 26; ++i) starts.d[i + 1] = (s += counts[i]);
    n = starts.d[26];

    CC(cudaMemcpyToSymbolAsync(dev::h::pp, PP.d, 26*sizeof(PP.d[0]), 0, H2D));
    CC(cudaMemcpyToSymbolAsync(dev::h::ff, FF.d, 26*sizeof(FF.d[0]), 0, H2D));
    KL(dev::halo, (k_cnf(n)), (starts, n, g::rgen->get_float()));
}
