namespace wall {
namespace sub {
namespace strt {

void read(/**/ float4 *pp, int *n) {
    Particle *pphst, *ppdev;
    pphst = new Particle[MAX_PART_NUM];
    restart::read_pp("wall", restart::TEMPL, /**/ pphst, n);

    if (*n) {
        CC(cudaMalloc(&ppdev, MAX_PART_NUM * sizeof(Particle)));
        cH2D(ppdev, pphst, *n);
        dev::particle2float4 <<<k_cnf(*n)>>> (ppdev, *n, /**/ pp);
        CC(cudaFree(ppdev));
    }
    delete[] pphst;
}

void write(const float4 *pp, const int n) {
    Particle *pphst, *ppdev;
    pphst = new Particle[MAX_PART_NUM];
    if (n) {
        CC(cudaMalloc(&ppdev, MAX_PART_NUM * sizeof(Particle)));
        dev::float42particle <<<k_cnf(n)>>> (pp, n, /**/ ppdev);
        cD2H(pphst, ppdev, n);
        CC(cudaFree(ppdev));
    }
    restart::write_pp("wall", restart::TEMPL, /**/ pphst, n);

    delete[] pphst;
}

} // strt
} // sub
} // wall
