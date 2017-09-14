
void distribute_flu() {
    using namespace o;
    
    build_map(q.n, q.pp, /**/ &d.p);
    pack_pp(q.pp, q.n, /**/ &d.p);
    if (global_ids)    pack_ii(qi.ii, q.n, /**/ &d.p);
    if (multi_solvent) pack_cc(qc.ii, q.n, /**/ &d.p);
    download(q.n, /**/ &d.p);

    post_send(&d.p, &d.c);
    post_recv(&d.c, &d.u);

    distr::flu::bulk(/**/ &q);
    
    wait_send(&d.c);
    wait_recv(&d.c, &d.u);
    
    unpack_pp(/**/ &d.u);
    if (global_ids)    unpack_ii(/**/ &d.u);
    if (multi_solvent) unpack_cc(/**/ &d.u);
    
    halo(&d.u, /**/ &q);
    gather(&d.p, &d.u, /**/ &q, &qi, &qc);
    
    flu::get_ticketZ(q, /**/ &tz);    
}
