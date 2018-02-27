#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "bop_common.h"
#include "bop_serial.h"

#include "macros.h"
#include "pp_id.h"

int X, Y, Z;

void updddL(const float *rrp, const float *rrc, const int n, /**/ float *ddL) {
    const int dL[3] = {X, Y, Z};
    
    for (int i = 0; i < 3*n; ++i) {
        const int d = i%3;
        const float dl = rrc[i] - rrp[i];
        const float sign = dl > 0 ? 1.f : -1.f;
        if (fabs(dl) > dL[d]/2) {
            ddL[i] -= sign * dL[d];
        }
    }
}

float MSD(const float *rr0, const float *rr, const float *ddL, const int buffsize, const int n) {
    float sumsq = 0.f;

    for (int i = 0; i < buffsize; ++i) {
        const float *r0 = rr0 + 3*i;
        const float  *r = rr  + 3*i;
        const float *dL = ddL + 3*i;
        for (int c = 0; c < 3; ++c) {
            const float dr = r[c] + dL[c] - r0[c];
            sumsq += dr*dr;
        }
    }
    
    return sumsq / n;
}

void MSD_seq(char **ffpp, char **ffii, const int nin, /**/ float *out) {

    BopData *dpp0, *dii0, *dpp, *dii;
    const int *ii0, *ii;
    const float *pp0, *pp;
    long np;

    BPC( bop_ini(&dpp0) );
    BPC( bop_ini(&dii0) );
    
    read_data(ffpp[0], dpp0, ffii[0], dii0);
    pp0 = (const float *) bop_get_data(dpp0);
    ii0 = (const   int *) bop_get_data(dii0);

    BPC( bop_get_n(dii0, &np) );
    const int buffsize = max_index(ii0, np) + 1;

    float *rr0 = new float[3*buffsize]; /* initial  positions     */
    float *rrc = new float[3*buffsize]; /* current  positions     */
    float *rrp = new float[3*buffsize]; /* previous positions     */
    float *ddL = new float[3*buffsize]; /* helper for periodic BC */

    memset(rr0, 0, 3*buffsize*sizeof(float));
    memset(ddL, 0, 3*buffsize*sizeof(float));
    pp2rr_sorted(ii0, pp0, np, 6, /**/ rr0);
    memcpy(rrp, rr0, 3*buffsize*sizeof(float));
    
    for (int i = 1; i < nin; ++i) {
        BPC( bop_ini(&dpp) );
        BPC( bop_ini(&dii) );
        
        read_data(ffpp[i], dpp, ffii[i], dii);
        pp = (const float *) bop_get_data(dpp);
        ii = (const   int *) bop_get_data(dii);

        memset(rrc, 0, 3*buffsize*sizeof(float));
        pp2rr_sorted(ii, pp, np, 6, /**/ rrc);
        updddL(rrp, rrc, buffsize, /**/ ddL);

        out[i-1] = MSD(rr0, rrc, ddL, buffsize, np);
        
        BPC( bop_fin(dpp) );
        BPC( bop_fin(dii) );
        memcpy(rrp, rrc, 3*buffsize*sizeof(float));
    }

    delete[] rr0; delete[] rrc;
    delete[] rrp; delete[] ddL;
    BPC( bop_fin(dpp0) );
    BPC( bop_fin(dii0) );
}

int main(int argc, char **argv) {

    if (argc < 7) {
        fprintf(stderr,
                "Usage: po.msd <optional args> <X> <Y> <Z> <inpp-*.bop> -- <inii-*.bop>\n"
                "Optional arguments:\n"
                "\t-t0step <t0step> : step (int) for averaging over t0 (default: Nfiles)\n");
        exit(1);
    }
    int iarg = 1;

    int t0step = -1;
    
    // check if optional arguments
    if (strcmp(argv[iarg], "-t0step") == 0) {
        iarg++;
        t0step = atoi(argv[iarg++]);
    }
    
    X = atoi(argv[iarg++]);
    Y = atoi(argv[iarg++]);
    Z = atoi(argv[iarg++]);

    const int sep = separator(argc, argv);
    const int nin = sep - iarg;

    if (nin < 2) ERR("Need more than one file\n");

    char **ffpp = argv + iarg;
    char **ffii = ffpp + nin + 1;

    float *msds    = new float[nin-1];
    float *submsds = new float[nin-1];
    int   *counts  = new   int[nin-1];
    
    memset(msds,   0, (nin-1)*sizeof(float));
    memset(counts, 0, (nin-1)*sizeof(int));

    t0step = t0step == -1 ? nin : t0step;
    
    for (int t0 = 0; t0 < nin-1; t0 += t0step) {
        memset(submsds, 0, (nin-1)*sizeof(float));

        MSD_seq(ffpp + t0, ffii + t0, nin - t0, /**/ submsds);

        for (int i = 0; i < nin-1-t0; ++i) {
            ++counts[i];
            msds[i] += submsds[i];
        }
    }

    // average
    for (int i = 0; i < nin - 1; ++i) {
        const int c = counts[i] ? counts[i] : 1;
        msds[i] /= c;
    }
    
    for (int i = 0; i < nin - 1; ++i) printf("%.6e\n", msds[i]);    
    delete[] msds; delete[] submsds;
    return 0;
}
