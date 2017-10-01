static __device__ float sqdist(float x, float y, float z,   float x0, float y0, float z0) {
    x -= x0; y -= y0; z -= z0;
    return x*x + y*y + z*z;
}

static __device__ void merged0(uint mystart, uint mycount, uint myscan, uint tid, uint pshare) {
    float xs, ys, zs;
    float xd, yd, zd;
    float d2;
    uint x13, y13, y14; // TODO: LDS.128
    asm volatile( "ld.volatile.shared.v2.u32 {%0,%1}, [%3+104];" // 104 = 13 x 8-byte uint2
                  "ld.volatile.shared.u32     %2,     [%3+116];" // 116 = 14 x 8-bute uint2 + .y
                  : "=r"( x13 ), "=r"( y13 ), "=r"( y14 ) : "r"( pshare ) : "memory" );
    const uint dststart = x13;
    const uint lastdst  = xsub( xadd( dststart, y14 ), y13 );
    const uint nsrc     = y14;
    const uint spidext  = x13;

    uint nb = 0;

    for( uint p = 0; p < nsrc; p = xadd( p, 32u ) ) {

        const uint pid = p + tid;
        uint spid;
        asm volatile( "{ .reg .pred p, q, r;" // TODO: HOW TO USE LDS.128
                      "  .reg .f32  key;"
                      "  .reg .f32  scan3, scan6, scan9;"
                      "  .reg .f32  mystart, myscan;"
                      "  .reg .s32  array;"
                      "  .reg .f32  array_f;"
                      "   mov.b32           array, %4;"
                      "   ld.shared.f32     scan9,  [array +  9*8 + 4];"
                      "   setp.ge.f32       p, %1, scan9;"
                      "   selp.f32          key, %2, 0.0, p;"
                      "   mov.b32           array_f, array;"
                      "   fma.f32.rm        array_f, key, 8.0, array_f;"
                      "   mov.b32 array,    array_f;"
                      "   ld.shared.f32     scan3, [array + 3*8 + 4];"
                      "   setp.ge.f32       p, %1, scan3;"
                      "@p add.f32           key, key, %3;"
                      "   setp.lt.f32       p, key, %2;"
                      "   setp.lt.and.f32   p, %5, %6, p;"
                      "   ld.shared.f32     scan6, [array + 6*8 + 4];"
                      "   setp.ge.and.f32   q, %1, scan6, p;"
                      "@q add.f32           key, key, %3;"
                      "   fma.f32.rm        array_f, key, 8.0, %4;"
                      "   mov.b32           array, array_f;"
                      "   ld.shared.v2.f32 {mystart, myscan}, [array];"
                      "   add.f32           mystart, mystart, %1;"
                      "   sub.f32           mystart, mystart, myscan;"
                      "   mov.b32           %0, mystart;"
                      "}" : "=r"( spid ) : "f"( u2f( pid ) ), "f"( u2f( 9u ) ), "f"( u2f( 3u ) ), "f"( u2f( pshare ) ), "f"( u2f( pid ) ), "f"( u2f( nsrc ) ) );
        cloud_pos(xmin(spid, lastdst), &xs, &ys, &zs);
        for(uint dpid = dststart; dpid < lastdst; dpid = xadd( dpid, 1u ) ) {
            cloud_pos(dpid, /**/ &xd, &yd, &zd);
            d2 = sqdist(xd, yd, zd,   xs, ys, zs);

            asm volatile( ".reg .pred interacting;" );
            uint overview;
            asm( "   setp.lt.ftz.f32  interacting, %3, 1.0;"
                 "   setp.ne.and.f32  interacting, %1, %2, interacting;"
                 "   setp.lt.and.f32  interacting, %2, %5, interacting;"
                 "   vote.ballot.b32  %0, interacting;" :
                 "=r"( overview ) : "f"( u2f( dpid ) ), "f"( u2f( spid ) ), "f"( d2 ), "f"( u2f( 1u ) ), "f"( u2f( lastdst ) ) );

            const uint insert = xadd( nb, i2u( __popc( overview & __lanemask_lt() ) ) );

            asm volatile( "@interacting st.volatile.shared.u32 [%0+1024], %1;" : :
                          "r"( xmad( insert, 4.f, pshare ) ),
                          "r"( __pack_8_24( xsub( dpid, dststart ), spid ) ) :
                          "memory" );

            nb = xadd( nb, i2u( __popc( overview ) ) );
            if( nb >= 32u ) {
                core( dststart, pshare, tid, spidext );
                nb = xsub( nb, 32u );
                asm volatile( "{ .reg .u32 tmp;"
                              "   ld.volatile.shared.u32 tmp, [%0+1024+128];"
                              "   st.volatile.shared.u32 [%0+1024], tmp;"
                              "}" :: "r"( xmad( tid, 4.f, pshare ) ) : "memory" );
            }
        }
    }

    if( tid < nb ) {
        core( dststart, pshare, tid, spidext );
    }
    nb = 0;
}

static __device__ void merged1(uint mystart, uint mycount, uint tid, uint pshare) {
    uint myscan;
    asm volatile("st.volatile.shared.u32 [%0], %1;" ::
                  "r"(xmad(tid, 8.f, pshare)),
                  "r"(mystart) :
                  "memory");
    myscan  = mycount;
    asmb::scan(&myscan);
    
    asm volatile("{    .reg .pred lt15;"
                 "      setp.lt.f32 lt15, %0, %1;"
                 "@lt15 st.volatile.shared.u32 [%2+4], %3;"
                 "}":: "f"(u2f(tid)), "f"(u2f(15u)), "r"(xmad(tid, 8.f, pshare)), "r"(xsub(myscan, mycount)) : "memory");
    merged0(mystart, mycount, myscan, tid, pshare);
}

static __device__ void merged2(int cid, uint tid, uint pshare) {
    uint mystart, mycount;
    asmb::c2loc(cid, tid, /**/ &mystart, &mycount);
    merged1(mystart, mycount, tid, pshare);
}

static __device__ void merged3(uint it, int cbase, uint tid, uint pshare) {
    int cid;
    cid = asmb::get_cid(it, cbase);
    merged2(cid, tid, pshare);
}

static __global__ void merged() {
    uint tid, wid, pshare, it;
    int cbase;
    char4 offs;
    asm volatile( ".shared .u32 smem[512];" ::: "memory" );
    tid = threadIdx.x;
    wid = threadIdx.y;
    pshare = xscale( threadIdx.y, 256.f );
    offs = __ldg( tid2ind + tid );
    cbase = blockIdx.z * MYCPBZ * info.ncells.x * info.ncells.y +
        blockIdx.y * MYCPBY * info.ncells.x +
        blockIdx.x * MYCPBX + wid +
        offs.z * info.ncells.x * info.ncells.y +
        offs.y * info.ncells.x +
        offs.x;
    for (it = 0; it < 4 ; it = xadd( it, 1u ))
        merged3(it, cbase, tid, pshare);
}
