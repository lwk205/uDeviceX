typedef const sdf::tex3Dca<float> TexSDF_t;
void build_cells(const int n, float4 *pp4, clist::Clist *cells);

void gen_quants(TexSDF_t texsdf, /**/ int *o_n, Particle *o_pp, int *w_n, float4 **w_pp);
void strt_quants(int *w_n, float4 **w_pp);

void gen_ticket(const int w_n, float4 *w_pp, clist::Clist *cells, clist::Ticket *tcells, Texo<int> *texstart, Texo<float4> *texpp);

namespace grey {
void pair(TexSDF_t texsdf, hforces::Cloud cloud, const int n, const Texo<int> texstart,
                  const Texo<float4> texpp, const int w_n, /**/ rnd::KISS *rnd, Force *ff);
}

namespace color {
void pair(TexSDF_t texsdf, hforces::Cloud cloud, const int n, const Texo<int> texstart,
                  const Texo<float4> texpp, const int w_n, /**/ rnd::KISS *rnd, Force *ff);
}

void strt_dump_templ(const int n, const float4 *pp);
