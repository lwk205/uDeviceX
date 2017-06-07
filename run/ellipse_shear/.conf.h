/* Part I (.conf.double.poiseuille.h) */

#define rc                1.5  /* cutoff radius */

#define XS   16
#define YS   32
#define ZS   16
#define XMARGIN_WALL      6
#define YMARGIN_WALL      6
#define ZMARGIN_WALL      6

#define _numberdensity    3
#define _kBT              1e-7
#define  dt               5e-4  /* timestep */
#define  rbc_mass         0.5   /* ratio of RBC particle mass to solvent particle mass */
#define _gamma_dot        0.0   /* shear rate */
#define _hydrostatic_a    10.0 /* flow acceleration for Poiseuille setup */

#define _aij_out          4.0  /* aij for the solvent outside the RBC */
#define _aij_in           4.0  /* aij for the solvent inside the RBC */
#define _aij_rbc          4.0  /* aij for the RBC membrane */
#define _aij_wall         4.0  /* aij for the wall */
#define _gammadpd_out     8.0  /* gamma for the solvent outside the RBC */
#define _gammadpd_in      8.0  /* gamma for the solvent inside the RBC */
#define _gammadpd_rbc     8.0  /* gamma for the RBC membrane */
#define _gammadpd_wall    8.0  /* gamma for the wall */

#define _ljsigma          0.3   /* RBC-RBC contact LJ interaction parameters */
#define _ljepsilon        1.0

/* Part II (added by tools/argp) */

#undef            a2_ellipse
#undef               _aij_in
#undef              _aij_out
#undef              _aij_rbc
#undef            b2_ellipse
#undef      doublepoiseuille
#undef          _gammadpd_in
#undef         _gammadpd_out
#undef         _gammadpd_rbc
#undef       hdf5field_dumps
#undef        hdf5part_dumps
#undef        _hydrostatic_a
#undef               pin_com
#undef           pushflow
#undef                 RBCnt
#undef                 RBCnv
#undef                  rbcs
#undef        steps_per_dump
#undef    steps_per_hdf5dump
#undef                  tend
#undef  wall_creation_stepid
#undef                 walls

#define           a2_ellipse    (16)    /* */
#define              _aij_in   (0.0)    /* */
#define             _aij_out   (0.0)    /* */
#define             _aij_rbc   (0.0)    /* */
#define           b2_ellipse     (4)    /* */
#define     doublepoiseuille (false)
#define         _gammadpd_in   (0.0)    /* */
#define        _gammadpd_out   (0.0)    /* */
#define        _gammadpd_rbc   (0.0)    /* */
#define      hdf5field_dumps  (true)    /* */
#define       hdf5part_dumps  (true)    /* */
#define       _hydrostatic_a   (0.0)    /* */
#define              pin_com  (true)    /* */
#define          pushflow (false)
#define                RBCnt   (992)
#define                RBCnv   (498)
#define                 rbcs  (true)    /* */
#define       steps_per_dump   (100)    /* */
#define   steps_per_hdf5dump   (100)    /* */
#define                 tend   (5.0)    /* */
#define wall_creation_stepid  (5000)
#define                walls (false)
