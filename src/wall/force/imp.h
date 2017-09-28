namespace wall {
struct Wa { /* local wall data */
    sdf::Tex_t sdf;
    Texo<int> start;
    Texo<float4> pp;
    int n;
};

namespace grey {
void force(hforces::Cloud cloud, int n, rnd::KISS *rnd, Wa wa, /**/ Force *ff);
}

namespace color {
void force(hforces::Cloud cloud, int n, rnd::KISS *rnd, Wa wa, /**/ Force *ff);
}

} /* namespace */
