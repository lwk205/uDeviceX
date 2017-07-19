void fin() {
    sdstr::fin();
    rdstr::fin();
    bbhalo::fin();
    cnt::fin();
    dump::fin(&dumpt);
    rex::fin();
    if (fsiforces)  fsi::fin();
    if (solids) mrescue::fin();

    sdf::free_quants(&w::qsdf);
    wall::free_quants(&w::q);
    wall::free_ticket(&w::t);

    delete dump_field;

    flu::free_quants(&o::q);
    flu::free_ticketZ(&o::tz);
    flu::free_ticketRND(&o::trnd);
    if (global_ids) flu::free_quantsI(&o::qi);

    dpdr::free_ticketcom(&o::h::tc);
    dpdr::free_ticketrnd(&o::h::trnd);
    dpdr::free_ticketSh(&o::h::ts);
    dpdr::free_ticketRh(&o::h::tr);
    
    odstr::free_ticketD(&o::td);
    odstr::free_ticketU(&o::tu);
    if (global_ids) odstr::free_ticketI(&o::ti);
    if (global_ids) odstr::free_ticketUI(&o::tui);
    odstr::free_work(&o::w);

    if (solids) {
        rig::free_quants(&s::q);
        rig::free_ticket(&s::t);
        CC(cudaFree(s::ff)); delete[] s::ff_hst;
    }

    if (rbcs) {
        rbc::free_quants(&r::q);
        rbc::destroy_textures(&r::tt);
        CC(cudaFree(r::ff));
    }
    datatype::fin();
}
