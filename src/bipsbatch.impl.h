namespace bipsbatch {
void interactions(Frag infos[26], /**/ float *ff) {
    static unsigned int hstart_padded[27];
    int nt; /* threads */
    int i;
    CC(cudaMemcpyToSymbolAsync(ffrag, infos, sizeof(Frag) * 26, 0, H2D));
    hstart_padded[0] = 0;
    for (i = 0; i < 26; ++i) hstart_padded[i + 1] = hstart_padded[i] + 16 * (((unsigned int)infos[i].ndst + 15) / 16);
    CC(cudaMemcpyToSymbolAsync(start, hstart_padded, sizeof(hstart_padded), 0, H2D));
    nt = 2 * hstart_padded[26];
    dSync();
    if (nt) force<<<k_cnf(nt)>>>(ff);
}
}
