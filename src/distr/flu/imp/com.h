void post_recv(Comm *c, Unpack *u) {
    UC(post_recv(&u->hpp, &c->pp));
    if (global_ids)    UC(post_recv(&u->hii, &c->ii));
    if (multi_solvent) UC(post_recv(&u->hcc, &c->cc));
}

void post_send(Pack *p, Comm *c) {
    UC(post_send(&p->hpp, &c->pp));
    if (global_ids)    UC(post_send(&p->hii, &c->ii));
    if (multi_solvent) UC(post_send(&p->hcc, &c->cc));
}

void wait_recv(Comm *c, Unpack *u) {
    UC(wait_recv(&c->pp, &u->hpp));
    if (global_ids)    UC(wait_recv(&c->ii, &u->hii));
    if (multi_solvent) UC(wait_recv(&c->cc, &u->hcc));
}

void wait_send(Comm *c) {
    UC(wait_send(&c->pp));
    if (global_ids)    UC(wait_send(&c->ii));
    if (multi_solvent) UC(wait_send(&c->cc));
}
