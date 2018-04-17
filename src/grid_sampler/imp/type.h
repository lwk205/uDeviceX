enum {
    RHO, VX, VY, VZ,
    NFIELDS_P
};

enum {
    SXX, SXY, SXZ,
    SYY, SYZ, SZZ,
    NFIELDS_S
};

enum {
    TOT_NFIELDS = NFIELDS_P + NFIELDS_S
};

static const char *names_p[NFIELDS_P] =
    {"density", "u", "v", "w"};

static const char *names_s[NFIELDS_S] =
    {"sxx", "sxy", "sxz",
     "syy", "syz", "szz"};

struct Grid {
    int3 L, N; /* subdomain size, grid size */
    float *p[NFIELDS_P]; /* particle density and velocity */
    float *s[NFIELDS_S]; /* stress                        */
    bool stress;
};

struct GridSampler {
    Grid sdev, stdev, hst;
    long nsteps;
};

struct SampleDatum {
    long n;
    const Particle *pp;
    const float *ss;
};

enum {
    MAX_N_DATA = 10
};

struct GridSampleData {
    int n;
    SampleDatum d[MAX_N_DATA];
};
