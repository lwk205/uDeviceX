struct PairParams;
struct PaArray;
struct FoArray;
struct WvelStep;
struct Coords;
struct Sdf;
struct int3;
struct float4;

// tag::struct[]
struct WallQuants {
    float4 *pp; /* particle positions xyzo xyzo ... */
    int n;      /* number of particles              */
    int3 L;     /* subdomain size                   */
};

struct WallTicket;
// end::struct[]

// tag::mem[]
void wall_ini_quants(int3 L, WallQuants*);
void wall_ini_ticket(int3 L, WallTicket**);

void wall_fin_quants(WallQuants*);
void wall_fin_ticket(WallTicket*);
// end::mem[]

// tag::gen[]
void wall_gen_quants(MPI_Comm, int maxn, const Sdf*, /* io */ int *o_n, Particle *o_pp,  /**/ WallQuants*);
void wall_gen_ticket(const WallQuants*, WallTicket*);
// end::gen[]

// tag::strt[]
void wall_strt_quants(const Coords*, int maxn, WallQuants*);
void wall_strt_dump_templ(const Coords*, const WallQuants*);
// end::strt[]

// tag::int[]
void wall_force(const PairParams*, const WvelStep *, const Coords*,
                Sdf*, const WallQuants*, const WallTicket*, int n, const PaArray*, const FoArray*);
// end::int[]
