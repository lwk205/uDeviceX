#include <cstdio>
#include "common.h"
#include "common.cuda.h"
#include "scan/int.h"
#include "clist/int.h"

namespace clist {
namespace dev {
#include "clist/dev.h"
}

static void scan(const int *counts, int n, /**/ int *starts) {
    scan::Work ws;
    scan::alloc_work(n, /**/ &ws);
    scan::scan(counts, n, /**/ starts, /*w*/ &ws);
    scan::free_work(&ws);
}

void build(const Particle *pp, int n,
           const int xcells, const int ycells, const int zcells,
           const float xstart, const float ystart, const float zstart,
           /**/ int *starts, int *counts) {
    if (!n) return;

    const int ncells = xcells * ycells * zcells;
    if (!ncells) return;

    const int3 cells = make_int3(xcells, ycells, zcells);
    const int3 domainstart = make_int3(xstart, ystart, zstart);
    
    int *ids;
    Particle *ppd;
    CC(cudaMalloc(&ids, n*sizeof(ids[0])));
    CC(cudaMalloc(&ppd, n*sizeof(ppd[0])));

    CC(cudaMemsetAsync(counts, 0, ncells * sizeof(int)));

    dev::get_counts <<<k_cnf(n)>>> (pp, n, cells, domainstart, /**/ counts);

    scan(counts, ncells, /**/ starts);
    
    CC(cudaMemsetAsync(counts, 0, ncells * sizeof(int)));

    dev::get_ids <<<k_cnf(n)>>> (pp, starts, n, cells, domainstart, /**/ counts, ids);

    dev::gather <<<k_cnf(n)>>> (pp, ids, n, /**/ ppd);
    
    CC(cudaFree(ids));
    CC(cudaFree(ppd));
}
}
