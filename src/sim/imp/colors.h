/* set colors of particles according to the RBCs */

void gen_colors(const Rbc *r, Colorer *c, Flu *f) {
    int nm, nv, nmhalo;
    nm = r->q.nc;
    nv = r->q.nv;

    build_map(nm, nv, r->q.pp, /**/ &c->e.p);
    pack(nv, r->q.pp, /**/ &c->e.p);
    download(&c->e.p);

    UC(post_send(&c->e.p, &c->e.c));
    UC(post_recv(&c->e.c, &c->e.u));

    if (nm * nv) CC(d::MemcpyAsync(c->pp, r->q.pp, nm * nv * sizeof(Particle), D2D));
    
    wait_send(&c->e.c);
    wait_recv(&c->e.c, &c->e.u);

    unpack(nv, &c->e.u, /**/ &nmhalo, c->pp + nm * nv);
    nm += nmhalo;

    /* compute extents */
    minmax(c->pp, nv, nm, /**/ c->minext, c->maxext);
    collision::get_colors(f->q.pp, f->q.n,
                          c->pp, r->q.tri,
                          r->q.nt, nv, nm,
                          c->minext, c->maxext, /**/ f->q.cc);
}

void recolor_flux(Flu *f) {
    if (RECOLOR_FLUX)
        recolor::linear_flux(COL_FLUX_DIR, RED_COLOR, f->q.n, f->q.pp, /**/ f->q.cc);
}
