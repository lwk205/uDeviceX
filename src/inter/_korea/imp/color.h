static int min (int x, int y)        { return x < y ? x : y; };
static int min3(int x, int y, int z) { return min(x, min(y, z)); }

static float rad(Coords c) { /* radius */
    int lx, ly, lz; /* domain */
    lx = xdomain(c);
    ly = ydomain(c);
    lz = zdomain(c);
    return 0.25*min3(lx, ly, lz);
}

/* red and blue spheres */
static bool blue(Coords c, float x, float y, float z) {
    int lx, ly, lz; /* domain */
    float x0, y0, z0, r;

    lx = xdomain(c);
    ly = ydomain(c);
    lz = zdomain(c);

    x0 = 0.25*lx; y0 = 0.6*ly; z0 = 0.5*lz;
    x -= x0; y -= y0; z -= z0;
    r = rad();
    return x*x + y*y + z*z < r*r;
}
static bool red(Coords c, float x, float y, float z) {
    int lx, ly, lz; /* domain */
    float x0, y0, z0, r;
    lx = xdomain(c);
    ly = ydomain(c);
    lz = zdomain(c);
    x0 = 0.75*lx; y0 = 0.4*ly; z0 = 0.5*lz;
    x -= x0; y -= y0; z -= z0;
    r = rad();
    return x*x + y*y + z*z < r*r;
}

static void color(Coords coords, Particle *pp, int n, /**/ int *cc) {
    enum {X, Y, Z};
    int i, w, b, r;
    float x, y, z;
    Particle p;
    enum {W = BLUE_COLOR, B = RED_COLOR, R = RED_COLOR};
    for (i = w = b = r = 0; i < n; i++) {
        p = pp[i];
        x = p.r[X]; y = p.r[Y]; z = p.r[Z];
        x = m::x2g(x); y = m::y2g(y); z = m::z2g(z);
        if      (blue(coords, x, y, z)) {cc[i] = B; b++;}
        else if (red (coords, x, y, z)) {cc[i] = R; r++;}
        else                            {cc[i] = W; w++;}
    }
    MSG("color scheme: Korea");
    MSG("white/blue/red : %d/%d/%d", w, b, r);
}
