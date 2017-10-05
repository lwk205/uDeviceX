namespace exch {
namespace mesh {

struct Pack {
    Map map;
    float3 *minext, *maxext;
    dBags dpp;
    hBags hpp;
};

struct Comm {
    Stamp pp;
};

struct Unpack {
    hBags hpp;
    dBags dpp;
};

/* optional structures for sending momenta back */

struct MMap { /* map for compression of Momentum (support structure only) */
    int *cc, *ss, *subids;
};

struct PackM {
    MMap maps[NFRAGS];
    int *cchst, *ccdev; /* helper to collect counts */
    dBags dmm, dii;
    hBags hmm, hii;
};

struct CommM {
    Stamp mm, ii;
};

struct UnpackM {
    dBags dmm, dii;
    hBags hmm, hii;
};

typedef Sarray<MMap, 26> MMap26;

} // mesh
} // exch
