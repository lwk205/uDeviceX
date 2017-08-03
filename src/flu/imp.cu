#include <cstdio>
#include <cassert>

#include <mpi.h>
#include "l/m.h"
#include "m.h"
#include "inc/type.h"
#include "common.h"
#include "common.cuda.h"
#include "common.mpi.h"
#include <conf.h>
#include "restart.h"

#include "flu/imp.h"
#include "flu/dev.h"

namespace flu {
namespace sub {

static int gen0(Particle *pp) { /* generate particle positions and velocities */
    enum {X, Y, Z};
    assert(XS * YS * ZS * numberdensity < MAX_PART_NUM);
  
    srand48(123456);
    int iz, iy, ix, l, nd = numberdensity;
    int n = 0; /* particle index */
    float x, y, z, dr = 0.99;
    for (iz = 0; iz < ZS; iz++)
    for (iy = 0; iy < YS; iy++)
    for (ix = 0; ix < XS; ix++) {
        /* edge of a cell */
        int xlo = -0.5*XS + ix, ylo = -0.5*YS + iy, zlo = -0.5*ZS + iz;
        for (l = 0; l < nd; l++) {
            Particle p = Particle();
            x = xlo + dr * drand48(), y = ylo + dr * drand48(), z = zlo + dr * drand48();
            p.r[X] = x; p.r[Y] = y; p.r[Z] = z;

            p.v[X] = 0; p.v[Y] = 0; p.v[Z] = 0;
          
            pp[n++] = p;
        }
    }

    MSG("ic::gen: created %06d solvent particles", n);
    return n;
}

int gen(Particle *dev, /*w*/ Particle *hst) {
    int n = gen0(hst);
    cH2D(dev, hst, n);
    return n;
}

static void ii_gen0(const long n, int *ii) {
    long i0 = 0;
    MC(l::m::Exscan(&n, &i0, 1, MPI_LONG, MPI_SUM, m::cart));
    for (long i = 0; i < n; ++i) ii[i] = i + i0;
}

void ii_gen(const int n, int *ii_dev, int *ii_hst) {
    ii_gen0(n, ii_hst);
    cH2D(ii_dev, ii_hst, n);
}

static void tags0_gen0(const long n, int *ii) {
    for (long i = 0; i < n; ++i) ii[i] = -1;
}

void tags0_gen(const int n, int *ii_dev, int *ii_hst) {
    tags0_gen0(n, ii_hst);
    cH2D(ii_dev, ii_hst, n);
}

int strt(const int id, Particle *dev, /*w*/ Particle *hst) {
    int n;
    restart::read_pp("flu", id, hst, &n);
    if (n) cH2D(dev, hst, n);
    return n;
}

int strt_ii(const char *subext, const int id, int *dev, /*w*/ int *hst) {
    int n;
    restart::read_ii("flu", subext, id, hst, &n);
    if (n) cH2D(dev, hst, n);
    return n;
}

void strt_dump(const int id, const int n, const Particle *dev, Particle *hst) {
    if (n) cD2H(hst, dev, n);
    restart::write_pp("flu", id, hst, n);
}

void strt_dump_ii(const char *subext, const int id, const int n, const int *dev, int *hst) {
    if (n) cD2H(hst, dev, n);
    restart::write_ii("flu", subext, id, hst, n);
}

void zip(const Particle *pp, const int n, /**/ float4 *zip0, ushort4 * zip1) {
    assert(sizeof(Particle) == 6 * sizeof(float)); /* :TODO: implicit dependency */
    dev::zip<<<(n + 1023) / 1024, 1024, 1024 * 6 * sizeof(float)>>>(zip0, zip1, (float*)pp, n);
}

} // sub
} // flu
