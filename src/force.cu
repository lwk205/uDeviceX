#include "force.h"
#include <conf.h>
#include "conf.common.h"

template<int s>
inline __device__ float viscosity_function(float x) { return sqrtf(viscosity_function<s - 1>(x)); }
template<> inline __device__ float viscosity_function<1>(float x) { return sqrtf(x); }
template<> inline __device__ float viscosity_function<0>(float x) { return x;        }

__device__ float3 force(int type1, int type2,
			float3 pos1, float3 pos2,
			float3 vel1, float3 vel2, float myrandnr) {
    /* return the DPD interaction force based on particle types */

    const float gammadpd[] = {gammadpd_solv, gammadpd_solid, gammadpd_wall, gammadpd_rbc};
    const float aij[] = {aij_solv, aij_solid, aij_wall, aij_rbc};

    const float _xr = pos1.x - pos2.x;
    const float _yr = pos1.y - pos2.y;
    const float _zr = pos1.z - pos2.z;

    const float rij2 = _xr * _xr + _yr * _yr + _zr * _zr;
    const float invrij = rsqrtf(rij2);
    const float rij = rij2 * invrij;
    if (rij2 >= 1)
    return make_float3(0, 0, 0);

    const float argwr = 1.f - rij;
    const float wr = viscosity_function<-VISCOSITY_S_LEVEL>(argwr);

    const float xr = _xr * invrij;
    const float yr = _yr * invrij;
    const float zr = _zr * invrij;

    const float rdotv =
        xr * (vel1.x - vel2.x) +
        yr * (vel1.y - vel2.y) +
        zr * (vel1.z - vel2.z);

    const float gammadpd_pair = 0.5 * (gammadpd[type1] + gammadpd[type2]);
    const float sigmaf_pair = sqrt(2*gammadpd_pair*kBT / dt);
    float strength = (-gammadpd_pair * wr * rdotv + sigmaf_pair * myrandnr) * wr;

    const bool ss = (type1 == SOLID_TYPE) && (type2 == SOLID_TYPE);
    const bool sw = (type1 == SOLID_TYPE) && (type2 ==  WALL_TYPE);

    if (ss || sw) {
        /*hack*/ const float ljsi = ss ? ljsigma : 2 * ljsigma;
        const float invr2 = invrij * invrij;
        const float t2 = ljsi * ljsi * invr2;
        const float t4 = t2 * t2;
        const float t6 = t4 * t2;
        const float lj = min(1e4f, max(0.f, ljepsilon * 24.f * invrij * t6 * (2.f * t6 - 1.f)));
        strength += lj;
    } 

    const float aij_pair = 0.5 * (aij[type1] + aij[type2]);
    strength += aij_pair * argwr;
    
    return make_float3(strength*xr, strength*yr, strength*zr);
}
