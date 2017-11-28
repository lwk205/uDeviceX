namespace distr {
namespace flu {

using namespace comm;

struct LivingParts {
    bool deadlist;
    const Particle *pp;
    const int *tags;
};

struct Pack {
    Map map;
    dBags dpp, dii, dcc;
    hBags hpp, hii, hcc;
    int nbulk;
};

struct Comm {
    Stamp pp, ii, cc;
};

struct Unpack {
    hBags hpp, hii, hcc;
    Particle *ppre;
    int *iire, *ccre;
    int nhalo;
};

} // flu
} // distr
