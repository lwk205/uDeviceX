#include "dpd-rng.h"
#include "hacks.h"
#include "../src/dpd-forces.h"

struct BipartiteInfoDPD {
    int3 ncells;
    float3 domainsize, invdomainsize, domainstart;
    float invrc;
};

__constant__ BipartiteInfoDPD bipart_info;

const static uint COLS = 8;
const static uint ROWS = (32 / COLS);
const static uint CPB = 4;

__global__
void _bipartite_dpd_directforces_floatized( float * const axayaz, const int np, const int np_src,
                                  const float seed, const int mask, const float * xyzuvw, const float * xyzuvw_src,
                                  const float invrc)
{
    const int tid = threadIdx.x % warpSize;
    const int pid = threadIdx.x + blockDim.x * blockIdx.x;
    const bool valid = pid < np;

    float xp, yp, zp, up, vp, wp;

    if( valid ) {
        xp = xyzuvw[0 + pid * 6];
        yp = xyzuvw[1 + pid * 6];
        zp = xyzuvw[2 + pid * 6];
        up = xyzuvw[3 + pid * 6];
        vp = xyzuvw[4 + pid * 6];
        wp = xyzuvw[5 + pid * 6];
    }

    float xforce = 0, yforce = 0, zforce = 0;

    for( int s = 0; s < np_src; s += warpSize ) {
        float my_xq, my_yq, my_zq, my_uq, my_vq, my_wq;

        const int batchsize = min( warpSize, np_src - s );

        if( tid < batchsize ) {
            my_xq = xyzuvw_src[0 + ( tid + s ) * 6];
            my_yq = xyzuvw_src[1 + ( tid + s ) * 6];
            my_zq = xyzuvw_src[2 + ( tid + s ) * 6];
            my_uq = xyzuvw_src[3 + ( tid + s ) * 6];
            my_vq = xyzuvw_src[4 + ( tid + s ) * 6];
            my_wq = xyzuvw_src[5 + ( tid + s ) * 6];
        }

        for( int l = 0; l < batchsize; ++l ) {
            const float xq = __shfl( my_xq, l );
            const float yq = __shfl( my_yq, l );
            const float zq = __shfl( my_zq, l );
            const float uq = __shfl( my_uq, l );
            const float vq = __shfl( my_vq, l );
            const float wq = __shfl( my_wq, l );

            {
                const int spid = s + l;
                const int dpid = pid;
                const int arg1 = mask * dpid + ( 1 - mask ) * spid;
                const int arg2 = mask * spid + ( 1 - mask ) * dpid;
                const float myrandnr = Logistic::mean0var1( seed, arg1, arg2 );

                // check for particle types and compute the DPD force
                float3 pos1 = make_float3(xp, yp, zp), pos2 = make_float3(xq, yq, zq);
                float3 vel1 = make_float3(up, vp, wp), vel2 = make_float3(uq, vq, wq);

                const float3 strength = compute_dpd_force_traced(SOLVENT_TYPE, SOLVENT_TYPE,
                        pos1, pos2, vel1, vel2, myrandnr);

                {
                    xforce += strength.x;
                    yforce += strength.y;
                    zforce += strength.z;
                }
            }
        }
    }

    if( valid ) {
        axayaz[0 + 3 * pid] = xforce;
        axayaz[1 + 3 * pid] = yforce;
        axayaz[2 + 3 * pid] = zforce;
    }
}

void directforces_dpd_cuda_bipartite_nohost(
    const float * const xyzuvw, float * const axayaz, const int np,
    const float * const xyzuvw_src, const int np_src,
    const float invsqrtdt,
    const float seed, const int mask)
{
    if( np == 0 || np_src == 0 ) {
        printf( "warning: directforces_dpd_cuda_bipartite_nohost called with ZERO!\n" );
        return;
    }

    _bipartite_dpd_directforces_floatized <<< ( np + 127 ) / 128, 128, 0 >>> ( axayaz, np, np_src, seed, mask,
            xyzuvw, xyzuvw_src, 1);

    CC( cudaPeekAtLastError() );
}

__global__
void _dpd_bipforces_floatized( const float2 * const xyzuvw, const int np, cudaTextureObject_t texDstStart,
                     cudaTextureObject_t texSrcStart,  cudaTextureObject_t texSrcParticles, const int np_src, const int3 halo_ncells,
                     const float seed, const uint mask, float * const axayaz )
{
    const uint tid = threadIdx.x;
    const uint subtid = tid % COLS;
    const uint slot = tid / COLS;
    const uint wid = threadIdx.y;

    __shared__ uint volatile starts[CPB][32], scan[CPB][32];

    const int mycid = xmad( blockIdx.x, float(CPB), threadIdx.y );

    if( mycid >= halo_ncells.x * halo_ncells.y * halo_ncells.z ) return;

    uint mycount = 0, myscan = 0;
    if( tid < 27 ) {
        const int dx = ( tid ) % 3;
        const int dy = ( ( tid / 3 ) ) % 3;
        const int dz = ( ( tid / 9 ) ) % 3;

        int xcid = ( mycid % halo_ncells.x ) + dx - 1;
        int ycid = ( ( mycid / halo_ncells.x ) % halo_ncells.y ) + dy - 1;
        int zcid = ( ( mycid / halo_ncells.x / halo_ncells.y ) % halo_ncells.z ) + dz - 1;

        const bool valid_cid =
                ( xcid >= 0 ) && ( xcid < halo_ncells.x ) &&
                ( ycid >= 0 ) && ( ycid < halo_ncells.y ) &&
                ( zcid >= 0 ) && ( zcid < halo_ncells.z );

        xcid = xmin( xsub( halo_ncells.x, 1 ), max( 0, xcid ) );
        ycid = xmin( xsub( halo_ncells.y, 1 ), max( 0, ycid ) );
        zcid = xmin( xsub( halo_ncells.z, 1 ), max( 0, zcid ) );

        const int cid = max( 0, xcid + halo_ncells.x * ( ycid + halo_ncells.y * zcid ) );

        starts[wid][tid] = tex1Dfetch<uint>( texSrcStart, cid );

        myscan = mycount = valid_cid ? ( tex1Dfetch<uint>( texSrcStart, cid + 1 ) - tex1Dfetch<uint>( texSrcStart, cid ) ) : 0u;
    }

    for( int L = 1; L < 32; L <<= 1 )
	//int or float yuhang?
        myscan += ( tid >= L ) * __shfl_up((int) myscan, L ) ;

    if( tid < 28 )
        scan[wid][tid] = myscan - mycount;

    const uint dststart = tex1Dfetch<uint>( texDstStart, mycid );
    const uint nsrc = scan[wid][27];
    const uint ndst = xsub( tex1Dfetch<uint>( texDstStart, xadd( mycid, 1 ) ), tex1Dfetch<uint>( texDstStart, mycid ) );

    for( uint d = 0u; d < ndst; d += ROWS ) {
        const uint np1 = xmin( ndst - d, ROWS );
        const uint dpid = xadd( xadd( dststart, d ), slot );
        const int entry = xscale( dpid, 3.f );
        float2 dtmp0 = xyzuvw[entry];
        float2 dtmp1 = xyzuvw[xadd(entry, 1)];
        float2 dtmp2 = xyzuvw[xadd(entry, 2)];

        float3 f = make_float3(0,0,0);

        for( uint s = 0; s < nsrc; s += COLS ) {
            const uint np2 = xmin( xsub(nsrc, s), COLS );
            const uint pid = xadd( s, subtid );
            const uint key9 = xadd( xsel_ge( pid, scan[wid][9            ], 9u, 0u ), xsel_ge( pid, scan[wid][18           ], 9u, 0u ) );
            const uint key3 = xadd( xsel_ge( pid, scan[wid][xadd(key9,3u)], 3u, 0u ), xsel_ge( pid, scan[wid][xadd(key9,6u)], 3u, 0u ) );
            const uint key  = xadd( key9, key3 );
            const uint spid = xsub( xadd( starts[wid][key], pid ), scan[wid][key] );
            const int sentry = xscale( spid, 3.f );

            const float2 stmp0 = tex1Dfetch<float2>( texSrcParticles, sentry );
            const float2 stmp1 = tex1Dfetch<float2>( texSrcParticles, xadd( sentry, 1 ) );
            const float2 stmp2 = tex1Dfetch<float2>( texSrcParticles, xadd( sentry, 2 ) );

            {
                const uint arg1 = xsel_gt( mask, 0u, dpid, spid );
                const uint arg2 = xsel_gt( mask, 0u, spid, dpid );
                const float myrandnr = Logistic::mean0var1( seed, arg1, arg2 );

                // check for particle types and compute the DPD force
                float3 pos1 = make_float3(dtmp0.x, dtmp0.y, dtmp1.x), pos2 = make_float3(stmp0.x, stmp0.y, stmp1.x);
                float3 vel1 = make_float3(dtmp1.y, dtmp2.x, dtmp2.y), vel2 = make_float3(stmp1.y, stmp2.x, stmp2.y);

                const float3 strength = compute_dpd_force_traced(SOLVENT_TYPE, SOLVENT_TYPE,
                        pos1, pos2, vel1, vel2, myrandnr);

                const bool valid = xfcmp_lt( slot, np1 ) * xfcmp_lt( subtid, np2 );

                if( valid ) {
                    f.x += strength.x;
                    f.y += strength.y;
                    f.z += strength.z;
                }
            }
        }

        for( uint L = COLS / 2u; L > 0u; L >>= 1 ) {
			f.x += __shfl_xor( f.x, L );
			f.y += __shfl_xor( f.y, L );
			f.z += __shfl_xor( f.z, L );
        }

        const uint c = ( subtid % 3u );
        const float fcontrib = xsel_eq( c, 0u, f.x, xsel_eq( c, 1u, f.y, f.z ) );
        const uint dstpid = xadd( xadd( dststart, d ), slot );

        if( slot < np1 )
            axayaz[ xmad( dstpid, 3.f, c ) ] = fcontrib;
    }
}

void forces_dpd_cuda_bipartite_nohost( const float2 * const xyzuvw, const int np, cudaTextureObject_t texDstStart,
                                       cudaTextureObject_t texSrcStart, cudaTextureObject_t texSrcParticles, const int np_src,
                                       const int3 halo_ncells,
                                       const float seed, const int mask, float * const axayaz )
{
    const int ncells = halo_ncells.x * halo_ncells.y * halo_ncells.z;

    static bool fbip_init = false;

    if( !fbip_init ) {
        CC( cudaFuncSetCacheConfig( _dpd_bipforces_floatized, cudaFuncCachePreferL1 ) );

        fbip_init = true;
    }

    _dpd_bipforces_floatized <<< ( ncells + CPB - 1 ) / CPB, dim3( 32, CPB ), 0>>> (
        xyzuvw, np, texDstStart, texSrcStart, texSrcParticles, np_src,
        halo_ncells, seed, mask,
        axayaz );
}
