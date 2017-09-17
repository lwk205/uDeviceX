static void free_map(/**/ Map *m) {
    CC(d::Free(m->counts));
    CC(d::Free(m->starts));    
    for (int i = 0; i < NFRAGS; ++i)
        CC(d::Free(m->ids[i]));
}

void fin(Pack *p) {
    free_map(/**/ &p->map);
    fin(PINNED, NONE, /**/ &p->hpp, &p->dpp);

    if (global_ids)    fin(PINNED, NONE, /**/ &p->hii, &p->dii);
    if (multi_solvent) fin(PINNED, NONE, /**/ &p->hcc, &p->dcc);
}

void fin(Comm *c) {
    fin(&c->pp);

    if (global_ids)    fin(&c->ii);
    if (multi_solvent) fin(&c->cc);
}

void fin(Unpack *u) {
    fin(HST_ONLY, NONE, &u->hpp, NULL);
    if (global_ids)    fin(HST_ONLY, NONE, &u->hii, NULL);
    if (multi_solvent) fin(HST_ONLY, NONE, &u->hcc, NULL);

    CC(d::Free(u->ppre));
    if (global_ids)    CC(d::Free(u->iire));
    if (multi_solvent) CC(d::Free(u->ccre));
}
