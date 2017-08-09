namespace cnt {
void bulk(std::vector<ParticlesWrap> wr) {
    if (wr.size() == 0) return;

    for (int i = 0; i < (int) wr.size(); ++i) {
        ParticlesWrap it = wr[i];
        KL(k_cnt::bulk, (k_cnf(3 * it.n)),
           ((float2 *)it.p, it.n, cellsentries->S, wr.size(), (float *)it.f,
            rgen->get_float(), i));
    }
}
}
