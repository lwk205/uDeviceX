static __constant__ InfoDPD info;
static __device__ char4 tid2ind[32] = {
    { -1, -1, -1, 0}, {0, -1, -1, 0}, {1, -1, -1, 0},
    { -1,  0, -1, 0}, {0,  0, -1, 0}, {1,  0, -1, 0},
    { -1 , 1, -1, 0}, {0,  1, -1, 0}, {1,  1, -1, 0},
    { -1, -1,  0, 0}, {0, -1,  0, 0}, {1, -1,  0, 0},
    { -1,  0,  0, 0}, {0,  0,  0, 0}, {1,  0,  0, 0},
    { -1,  1,  0, 0}, {0,  1,  0, 0}, {1,  1,  0, 0},
    { -1, -1,  1, 0}, {0, -1,  1, 0}, {1, -1,  1, 0},
    { -1,  0,  1, 0}, {0,  0,  1, 0}, {1,  0,  1, 0},
    { -1,  1,  1, 0}, {0,  1,  1, 0}, {1,  1,  1, 0},
    { 0,  0,  0, 0}, {0,  0,  0, 0}, {0,  0,  0, 0},
    { 0,  0,  0, 0}, {0,  0,  0, 0}
};
static texture<float4, cudaTextureType1D> texParticlesF4;
static texture<ushort4, cudaTextureType1D, cudaReadModeNormalizedFloat> texParticlesH4;
texture<uint2, cudaTextureType1D> texStartAndCount;
static texture<int, cudaTextureType1D> texColor;
