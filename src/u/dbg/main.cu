#include <mpi.h>
#include <stdio.h>

#include <conf.h>
#include "inc/conf.h"

#include "utils/msg.h"
#include "mpi/wrapper.h" /* mini-MPI and -device */
#include "mpi/glb.h"

#include "d/api.h"

#include "utils/error.h"
#include "utils/cc.h"
#include "utils/kl.h"
#include "parser/imp.h"
#include "inc/type.h"
#include "inc/dev.h"
#include "dbg/imp.h"
#include "coords/ini.h"
#include "coords/imp.h"

const int n = 10;
Particle *pp;
Force *ff;

void alloc() {
    CC(d::Malloc((void**) &pp, n * sizeof(Particle)));
    CC(d::Malloc((void**) &ff, n * sizeof(Force)));
}

void free() {
    CC(d::Free(pp));
    CC(d::Free(ff));
}

namespace dev {

__global__ void fill_bugs(int3 L, Particle *pp, int n) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    Particle p;
    p.r[0] = p.r[1] = p.r[2] = 0;
    p.v[0] = p.v[1] = p.v[2] = 0;

    if (i >= n) return;
    if (i == 1) p.r[0] = 1.5 * L.x;  // invalid position
    if (i <  1) p.v[0] = 0.f / 0.f; // nan
    pp[i] = p;
}

__global__ void fill_bugs(Force *ff, int n) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    Force f;
    f.f[0] = f.f[1] = f.f[2] = 0;

    if (i >= n) return;
    if (i < 1) f.f[0] = 1.f / 0.f; // inf
    ff[i] = f;
}
} // dev

void fill_bugs(int3 L) {
    KL(dev::fill_bugs, (k_cnf(n)), (L, pp, n));
    KL(dev::fill_bugs, (k_cnf(n)), (ff, n));
}

void check(float dt0, const Coords *c, Dbg *dbg) {
    UC(dbg_check_pos    (c, "flu", dbg, n, pp));
    UC(dbg_check_vel    (dt0, c, "flu", dbg, n, pp));
    UC(dbg_check_forces (dt0, c, "flu.ff", dbg, n, pp, ff));
}

int main(int argc, char **argv) {
    Dbg *dbg;
    Config *cfg;
    Coords *coords;
    int3 L;
    float dt0;
    m::ini(&argc, &argv);
    
    UC(conf_ini(&cfg));
    UC(dbg_ini(&dbg));
    UC(conf_read(argc, argv, cfg));
    UC(conf_lookup_float(cfg, "glb.dt", &dt0));    
    UC(dbg_set_conf(cfg, dbg));
    UC(coords_ini_conf(m::cart, cfg, &coords));

    L = subdomain(coords);
    
    alloc();
    fill_bugs(L);
    check(dt0, coords, dbg);
    free();
    UC(dbg_fin(dbg));
    UC(conf_fin(cfg));
    UC(coords_fin(coords));
    m::fin();
}
