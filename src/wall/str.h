void read(const int id, /**/ float4 *pp, int *n) {
    Particle *pphst, *ppdev;
    pphst = new Particle[MAX_PART_NUM];
    restart::read("wall", id, /**/ pphst, n);

    if (*n) {
        CC(cudaMalloc(&ppdev, MAX_PART_NUM * sizeof(Particle)));
        cH2D(ppdev, pphst, *n);
        dev::particle2float4 <<<k_cnf(*n)>>> (ppdev, *n, /**/ pp);
        CC(cudaFree(ppdev));
    }
    delete[] pphst;
}

void write(const int id, const float4 *pp, const int n) {
    Particle *pphst, *ppdev;
    pphst = new Particle[MAX_PART_NUM];
    if (n) {
        CC(cudaMalloc(&ppdev, MAX_PART_NUM * sizeof(Particle)));
        dev::float42particle <<<k_cnf(n)>>> (pp, n, /**/ ppdev);
        cD2H(pphst, ppdev, n);
        CC(cudaFree(ppdev));
    }
    restart::write("wall", id, /**/ pphst, n);

    delete[] pphst;
}
