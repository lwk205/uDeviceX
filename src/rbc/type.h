namespace rbc {
struct Edg {
    float a; /* equilibrium edge lengths and triangle area */
    float A;
};
struct Shape {
    float* anti; /* every edge is visited twice, what is an id of
                    another visit? */
    Edg *edg;
    float totArea;
};

struct Quants {
    int n, nc;             /* number of particles, cells            */
    Particle *pp, *pp_hst; /* vertices particles on host and device */
    float *av;             /* area and volume on device             */

    int *ii;               /* global ids (on host) */

    int nt, nv;            /* number of triangles and vertices per mesh */
    int *adj0, *adj1;      /* adjacency lists on device                 */
    int4 *tri, *tri_hst;   /* triangles: vertex indices                 */

    Shape shape;
};

} /* namespace */
