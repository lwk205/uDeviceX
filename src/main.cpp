#include <stdio.h>

#include <conf.h>
#include "inc/conf.h"

#include "utils/msg.h"
#include "mpi/glb.h"
#include "d/api.h"

#include "sim/imp.h"

int main(int argc, char **argv) {
    sim::Sim *sim;

    m::ini(&argc, &argv);
    msg_ini(m::rank);
    msg_print("mpi size: %d", m::size);
    d::ini();
    
    sim::sim_ini(argc, argv, /**/ &sim);
    if (RESTART) sim::sim_strt(sim);
    else         sim::sim_gen(sim);
    sim::sim_fin(sim);
    m::fin();
    msg_print("end");
}
