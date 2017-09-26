namespace mbounce {

struct TicketM { /* momentum ticket */
    Momentum *mm_dev, *mm_hst;
};

void alloc_ticketM(TicketM *t);
void free_ticketM(TicketM *t);

void bounce_hst(const Force *ff, int nt, int nv, const int4 *tt, const Particle *i_pp, const int *tcellstarts, const int *tcellcounts, const int *tids,
                const int n, const int totnt, /**/ Particle *pp, TicketM *t);

void bounce_dev(const Force *ff, int nt, int nv, const int4 *tt, const Particle *i_pp, const int *tcellstarts, const int *tcellcounts, const int *tids,
                const int n, const int totnt, /**/ Particle *pp, TicketM *t);

void bounce_rbc_hst(const Force *ff, const int4 *tt, int nt, int nv, const Particle *i_pp, const int *tcellstarts, const int *tcellcounts,
                    const int *tids, const int n, const int totnt, /**/ Particle *pp, TicketM *t);

void bounce_rbc_dev(const Force *ff, const int4 *tt, int nt, int nv, const Particle *i_pp, const int *tcellstarts, const int *tcellcounts,
                    const int *tids, const int n, const int totnt, /**/ Particle *pp, TicketM *t);

void collect_rig_hst(int nt, int ns, const TicketM *t, /**/ Solid *ss);
void collect_rig_dev(int nt, int ns, const TicketM *t, /**/ Solid *ss);

} // mbounce
