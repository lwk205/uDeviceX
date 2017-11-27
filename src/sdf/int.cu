#include <mpi.h>
#include <stdio.h>
#include <conf.h>
#include "inc/conf.h"

#include "d/ker.h"
#include "d/api.h"

#include "utils/error.h"
#include "inc/def.h"
#include "msg.h"
#include "mpi/glb.h"
#include "utils/cc.h"

#include "inc/type.h"
#include "inc/dev.h"
#include "inc/macro.h"

#include "sdf/type.h"
#include "sdf/int.h"
#include "sdf/imp.h"

namespace sdf {
void alloc_quants(Quants *q) {
    cudaChannelFormatDesc fmt = cudaCreateChannelDesc<float>();
    CC(cudaMalloc3DArray(&q->arrsdf, &fmt, make_cudaExtent(XTE, YTE, ZTE)));
}

void  free_quants(Quants *q) {
    CC(cudaFreeArray(q->arrsdf));
    q->texsdf.destroy();
}

void ini(MPI_Comm cart, Quants *q) {
    UC(sub::ini(cart, q->arrsdf, &q->texsdf));
}

void bulk_wall(const tex3Dca<float> texsdf, /*io*/ Particle *s_pp, int *s_n, /*o*/ Particle *w_pp, int *w_n) {
    UC(sub::bulk_wall(texsdf, /*io*/ s_pp, s_n, /*o*/ w_pp, w_n));
}

int who_stays(const Quants q, Particle *pp, int n, int nc, int nv, int *stay) {
    return sub::who_stays(q.texsdf, pp, n, nc, nv, /**/ stay);
}

void bounce(const Quants *q, int n, /**/ Particle *pp) {
    UC(sub::bounce(q->texsdf, n, /**/ pp));
}
}
