namespace forces {
struct Pa {
    float x, y, z, vx, vy, vz;
    int kind; /* flu, rig et ct. */
    int color;
};
struct Fo { /* force */
    float x, y, z;
};
}
