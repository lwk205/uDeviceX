namespace Cont {
void transform(float* rr0, int nv, float *A, /* output */ Particle* pp) {
  /* rr0: vertices of RBC template
     A: affice transfromation matrix */
  for (int iv = 0; iv < nv; iv++) {
    float  *r = pp[iv].r, *v = pp[iv].v;
    float *r0 = &rr0[3*iv];

    for (int c = 0, i = 0; c < 3; c++) {
      r[c] += A[i++]*r0[0]; /* matrix transformation */
      r[c] += A[i++]*r0[1];
      r[c] += A[i++]*r0[2];
      r[c] += A[i++];

      v[c] = 0;
    }
  }
}

int setup_hst(int nv, Particle *pp_hst) {
  /* fills `pp_hst' with RBCs for this processor on host */
  const char *r_templ = "rbc.off", *r_state = "rbcs-ic.txt";;

  float rr0[3*MAX_VERT_NUM]; /* rbc template */
  off::f2vert(r_templ, rr0);

  int i, j, c, ic = 0;
  int mi[3], L[3] = {XS, YS, ZS};
  for (c = 0; c < 3; ++c) mi[c] = (m::coords[c] + 0.5) * L[c];

  float A[4*4]; /* 4x4 affice transformation matrix */
  FILE *f = fopen(r_state, "r");
  while (true) {
    for (i = 0; i < 4*4; i++) if (fscanf(f, "%f", &A[i]) != 1) goto done;
    for (c = 0; c < 3; c++) {
      j = 4 * c + 3;
      A[j] -= mi[c]; /* in local coordinates */
      if (2*A[j] < -L[c] || 2*A[j] > L[c]) goto next; /* not my RBC */
    }
    transform(rr0, nv, A, &pp_hst[nv*(ic++)]);
    next: ;
  }
 done:
  fclose(f);
  int nc = ic;
  return nc;
}

int setup(Particle* pp, int nv, /* storage */ Particle *pp_hst) {
  /* fills `pp' with RBCs for this processor */
  int nc = setup_hst(nv, pp_hst);
  CC(cudaMemcpy(pp, pp_hst, sizeof(Particle) * nv * nc, H2D));
  MPI_Barrier(m::cart);
  return nc;
}
}
