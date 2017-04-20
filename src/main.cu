#include <cstdio>
#include <mpi.h>
#include <map>
#include ".conf.h" /* configuration file (copy from .conf.test.h) */
#include "m.h"     /* MPI */
#include "common.h"
#include "bund.h"
#include "glb.h"

void mpi_init(int argc, char **argv) {
  MC(MPI_Init(&argc, &argv));
  MC(MPI_Comm_rank(MPI_COMM_WORLD,   &m::rank));
  MC(MPI_Cart_create(MPI_COMM_WORLD,
		     m::d, m::dims, m::periods, m::reorder,   &m::cart));
  MC(MPI_Cart_coords(m::cart, m::rank, m::d,   m::coords));
}

int main(int argc, char **argv) {
  m::dims[0] = m::dims[1] = m::dims[2] = 1;
  for (int iarg = 1; iarg < argc && iarg <= 3; iarg++)
    m::dims[iarg - 1] = atoi(argv[iarg]);

  int device = 2;
  CC(cudaSetDevice(device));

  mpi_init(argc, argv);
  glb::sim(); /* simulation level globals */

  sim::init();
  sim::run();
  sim::close();
  
  MC(MPI_Finalize());
}
