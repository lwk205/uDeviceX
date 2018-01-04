struct Tform;
struct Coords;

/* T: texture size, N: sdf file size, M: wall margin */
void tex2sdf_ini(const Coords*, int T[3], int N[3], int M[3], /**/ Tform*);
void sub2sdf_ini(const Coords*, int N[3], /**/ Tform*);
void sub2tex_ini(/**/ Tform*);
