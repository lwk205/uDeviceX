namespace distr {
namespace flu {

void ini(int maxdensity, Pack *p);
void ini(MPI_Comm comm, /**/ Comm *c);
void ini(int maxdensity, Unpack *u);

void fin(Pack *p);
void fin(Comm *c);
void fin(Unpack *u);

/* map */
void build_map(int n, const PartList lp, Pack *p);

using namespace ::flu;

/* pack */
void pack(const Quants *q, /**/ Pack *p);

void download(int n, Pack *p);

/* communication */
void post_recv(Comm *c, Unpack *u);
void post_send(Pack *p, Comm   *c);
void wait_recv(Comm *c, Unpack *u);
void wait_send(Comm *c);

/* unpack */
void unpack(/**/ Unpack *u);

/* cell lists */
void bulk(/**/ Quants *q);
void halo(const Unpack *u, /**/ Quants *q);
void gather(const Pack *p, const Unpack *u, /**/ Quants *q);

} // flu
} // distr
