#define SZ(a) (int)(sizeof(a)/sizeof(*(a)))
static enum cudaMemcpyKind k2k0[] = {  /* [k]ind to [k]ind */
    cudaMemcpyHostToHost, cudaMemcpyHostToDevice,
    cudaMemcpyDeviceToHost, cudaMemcpyDeviceToDevice,
    cudaMemcpyDefault };
static enum cudaMemcpyKind k2k(int k) {
    if (k < 0 || k >= SZ(k2k0)) ERR("unknown kind: %d", k);
    return k2k0[k];
}

static int R(cudaError_t e) {
    if (e == cudaSuccess) return 0;
    else {
        emsg0 = cudaGetErrorString(e);
        return 1;
    }
}

int ini() {
    /* panda specific for multi-gpu testing
       int device = m::rank % 2 ? 0 : 2; */
    int device = 0;
    return R(cudaSetDevice(device));
}

int alloc_pinned(void **pHost, size_t size) {
    return R(cudaHostAlloc(pHost, size, cudaHostAllocMapped));
}

int Malloc(void **devPtr, size_t size) {
    return R(cudaMalloc(devPtr, size));
}

int MemcpyToSymbol(const void *symbol, const void *src, size_t count, size_t offset, int kind0) {
    enum cudaMemcpyKind kind;
    kind = k2k(kind0);
    return R(cudaMemcpyToSymbol(symbol, src, count, offset, kind));
}

int HostGetDevicePointer (void **pDevice, void *pHost, unsigned int flags) {
    return R(cudaHostGetDevicePointer (pDevice, pHost, flags));
}

int Memcpy (void *dst, const void *src, size_t count, int kind0) {
    enum cudaMemcpyKind kind = k2k(kind0);
    return R(cudaMemcpy(dst, src, count, kind));
}

int MemsetAsync (void *devPtr, int value, size_t count, Stream_t stream) {
    if (stream != 0) ERR("streams are not supported");
    return R(cudaMemsetAsync(devPtr, value, count));
}

int Memset (void *devPtr, int value, size_t count) {
    return R(cudaMemset(devPtr, value, count));
}

int MemcpyAsync (void * dst, const void * src, size_t count, int kind0, Stream_t stream) {
    enum cudaMemcpyKind kind;
    if (stream != 0) ERR("streams are not supported");
    kind = k2k(kind0);
    return R(cudaMemcpyAsync (dst, src, count, kind));
}

int Free (void *devPtr) {
    return R(cudaFree (devPtr));
}

int FreeHost (void *hstPtr) {
    return R(cudaFreeHost(hstPtr));
}

int DeviceSynchronize (void) {
    return R(cudaDeviceSynchronize());
}
