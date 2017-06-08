#define XS   40
#define YS   52
#define ZS   20
#define XWM      6
#define YMARGIN_WALL      6
#define ZMARGIN_WALL      6

#define XMARGIN_BB 1.f
#define YMARGIN_BB 1.f
#define ZMARGIN_BB 1.f

#define numberdensity    10
#define kBT              4.44302e-8
#define dt               5e-4       /* timestep */
#define rbc_mass         0.5        /* ratio of RBC particle mass to solvent particle mass */
#define solid_mass       0.5        /* ratio of solid particle mass to solvent particle mass */
#define gamma_dot        0.0        /* shear rate */
#define driving_force    0.1        /* flow acceleration for Poiseuille setup */

#define aij_solv         2.6666667  /* aij for the solvent */
#define aij_rbc          2.6666667  /* aij for the RBC membrane */
#define aij_solid        2.6666667  /* aij for the solid */
#define aij_wall         2.6666667  /* aij for the wall */
#define gammadpd_solv    8.0        /* gamma for the solvent */
#define gammadpd_rbc     8.0        /* gamma for the RBC membrane */
#define gammadpd_solid   8.0        /* gamma for the solid */
#define gammadpd_wall    8.0        /* gamma for the wall */

#define ljsigma          0.3        /* RBC-RBC contact LJ interaction parameters */
#define ljepsilon        0.44

#define contactforces    (false)
