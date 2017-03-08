namespace gsl {

#define XX 0
#define XY 1
#define XZ 2
#define YY 3
#define YZ 4
#define ZZ 5

#define YX XY
#define ZX XZ
#define ZY YZ

#define DIM 3

void inv3x3_s1(gsl_matrix *m, gsl_permutation *p, gsl_matrix *minv) {
	int s;
	gsl_linalg_LU_decomp(m, p, &s);
	gsl_linalg_LU_invert(m, p, minv);
}

void inv3x3_s0(double *m0, double *minv0) {
	gsl_permutation *p = gsl_permutation_alloc(DIM);
	gsl_matrix_view m = gsl_matrix_view_array(m0, DIM, DIM);
	gsl_matrix_view minv = gsl_matrix_view_array(minv0, DIM, DIM);

	inv3x3_s1(&(m.matrix), p, &(minv.matrix));

	gsl_permutation_free(p);
}

void inv3x3(float *m0, float *minv0) {
	double m[DIM*DIM], minv[DIM*DIM];
	int i;

	i = 0;
	m[i++] = m0[XX]; m[i++] = m0[XY]; m[i++] = m0[XZ];
	m[i++] = m0[YX]; m[i++] = m0[YY]; m[i++] = m0[YZ];
	m[i++] = m0[ZX]; m[i++] = m0[ZY]; m[i++] = m0[ZZ];

	inv3x3_s0(m, minv);

	i = 0;
	minv0[XX] = minv[i++]; minv0[XY] = minv[i++]; minv0[XZ] = minv[i++];
	minv0[YX] = minv[i++]; minv0[YY] = minv[i++]; minv0[YZ] = minv[i++];
	minv0[ZX] = minv[i++]; minv0[ZY] = minv[i++]; minv0[ZZ] = minv[i++];
}
}
