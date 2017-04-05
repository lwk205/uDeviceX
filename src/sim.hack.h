int szp = sizeof(Particle);
Particle *sol_dev = s_pp, *sol_hst = sr_pp;

/* copy from device */
cudaMemcpy(sol_hst, sol_dev, szp*s_n, H2H);

/* process RBCs */
if (rbcs) {
    Particle *rbc_dev = r_pp, *rbc_hst = sol_hst + s_n;

    /* copy from device */
    cudaMemcpy(rbc_hst, rbc_dev, szp*r_n, H2H);

#define SXX sol_hst[i].r[0]
#define SYY sol_hst[i].r[1]
#define SZZ sol_hst[i].r[2]

#define RXX rbc_hst[i].r[0]
#define RYY rbc_hst[i].r[1]
#define RZZ rbc_hst[i].r[2]

#define SUU sol_hst[i].v[0]
    int i;
    for (i = 0; i < s_n; i++) {sol_xx[i] = SXX; sol_yy[i] = SYY; sol_zz[i] = SZZ;}
    for (i = 0; i < r_n; i++) {rbc_xx[i] = RXX; rbc_yy[i] = RYY; rbc_zz[i] = RZZ;}

    iotags_all(r_n, rbc_xx, rbc_yy, rbc_zz,
               s_n, sol_xx, sol_yy, sol_zz,
               iotags);

    /* collect statistics */
    int in2out = 0, out2in = 0, cnt_in = 0;
    for (i = 0; i < s_n; i++) {
      bool now_in = iotags[i] != -1         , now_out = !now_in;

      if (was_in  && now_out) in2out ++;
      if (was_out && now_in ) out2in ++;
      if (now_in            ) cnt_in ++;
    }
    /* set the last bit to 1 for tagged particles */
    fprintf(stderr, "(simulation.hack.h) in2out, out2in, cnt_in: %d  %d  %d\n",
	    in2out, out2in, cnt_in);

    /* copy to device */
    cudaMemcpy(rbc_dev, rbc_hst, szp*r_n, H2D);
 }

/* copy to device */
cudaMemcpy(sol_dev, sol_hst, szp*s_n, H2D);
