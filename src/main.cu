#include <cstdio>
#include <mpi.h>
#include <map>
#include <conf.h>
#include "m.h"     /* MPI */
#include "common.h"
#include "bund.h"
#include "glb.h"

void mpi_init(int argc, char **argv) {
    MC(MPI_Init(&argc, &argv));
    MC(l::m::Comm_rank(MPI_COMM_WORLD,   &m::rank));
    MC(l::m::Cart_create(MPI_COMM_WORLD,
                       m::d, m::dims, m::periods, m::reorder,   &m::cart));
    MC(l::m::Cart_coords(m::cart, m::rank, m::d,   m::coords));
}

int main(int argc, char **argv) {
    m::dims[0] = m::dims[1] = m::dims[2] = 1;
    for (int iarg = 1; iarg < argc && iarg <= 3; iarg++)
    m::dims[iarg - 1] = atoi(argv[iarg]);

    mpi_init(argc, argv);

    // panda specific for multi-gpu testing
    //int device = m::rank % 2 ? 0 : 2;
    int device = 0;
    CC(cudaSetDevice(device));
  
    glb::sim(); /* simulation level globals */

    sim::init();
    sim::run();
    sim::close();
  
    MC(l::m::Finalize());
}
