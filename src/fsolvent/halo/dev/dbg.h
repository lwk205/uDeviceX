__device__ void assert_frag(int i, const flu::RFrag frag) {
    int xs, ys, zs; /* sizes */
    int dx, dy, dz;
    xs = frag.xcells; ys = frag.ycells; zs = frag.zcells;
    dx = frag.dx; dy = frag.dy; dz = frag.dz;
    assert(xs * ys * zs == frag_ncell(i));
    assert(frag_d2i(dx, dy, dz) == i);
}

__device__ void assert_rc(const flu::RFrag frag, int i, int row, int col, int jump) {
    /* i: base cell id */
    int fid, nmax;
    int dx, dy, dz;
    int j;
    assert(row == 1 || row == 2 || row == 3);
    assert(col == 1 || col == 2 || col == 3);

    dx = frag.dx; dy = frag.dy; dz = frag.dz;
    assert(dx == -1 || dx == 0 || dx == 1);
    assert(dy == -1 || dy == 0 || dy == 1);
    assert(dz == -1 || dz == 0 || dz == 1);
    fid  = frag_d2i(dx, dy, dz);
    nmax = frag_ncell(fid) + 1;

    for (j = 0 ; j < row; j++) {
        assert(i       < nmax);
        assert(i + col < nmax);
        i += jump;
    }
}
