enum {
    XOFFSET = XS / 2,
    YOFFSET = YS / 2,
    ZOFFSET = ZS / 2
};

namespace c {
texture<int, cudaTextureType1D> start, id;
__constant__ const float2 *PP[MAX_OBJ_TYPES];
__constant__ float *FF[MAX_OBJ_TYPES];
}

namespace h {
__constant__ int starts[27];
__constant__ Particle *pp[26];
__constant__ Force *ff[26];
}
