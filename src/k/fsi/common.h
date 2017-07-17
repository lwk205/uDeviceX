namespace k_fsi {
texture<float2, cudaTextureType1D> texSolventParticles;
texture<int, cudaTextureType1D> texCellsStart, texCellsCount;
static const int NCELLS = XS * YS * ZS;
__constant__ int packstarts_padded[27], packcount[26];
__constant__ Particle *packstates[26];
__constant__ Force *packresults[26];
}
