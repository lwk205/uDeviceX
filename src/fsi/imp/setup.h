static void setup_first() {
    dev::t::start.channelDesc = cudaCreateChannelDesc<int>();
    dev::t::start.filterMode = cudaFilterModePoint;
    dev::t::start.mipmapFilterMode = cudaFilterModePoint;
    dev::t::start.normalized = 0;
}

static void setup(const int *const cellsstart) {
    size_t offset;
    int nc;
    static bool firsttime = true;
    if (firsttime) {
        setup_first();
        firsttime = false;
    }

    offset = 0;
    nc = XS * YS * ZS;
    CC(cudaBindTexture(&offset, &dev::t::start, cellsstart, &dev::t::start.channelDesc, sizeof(int) * nc));
}
