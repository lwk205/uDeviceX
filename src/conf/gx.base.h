#define XWM      6
#define YWM      6
#define ZWM      6

#define XBBM 1.f
#define YBBM 1.f
#define ZBBM 1.f

#define numberdensity       4
#define kBT                 4.44302e-8
#define dt                  5e-4
#define dpd_mass         1.0        /* solvent particle mass */
#define rbc_mass         0.5        /* RBC particle mass     */
#define solid_mass       0.5        /* solid particle mass   */

#define WVEL_PAR_A        0.0        /* shear rate */

#define adpd_b            5
#define adpd_r             5
#define adpd_br            5
#define gdpd_b       8.0
#define gdpd_r        8.0
#define gdpd_br       8.0

#define ljsigma          0.3        /* RBC-RBC contact LJ interaction parameters */
#define ljepsilon        0.44

#define global_ids       (false)
#define multi_solvent    (false)

#define fsiforces        (true)
#define contactforces    (false)
#define strt_dumps       (false)
#define strt_freq        (5000)
#define field_dumps      (false)
#define field_freq       (2000)
#define part_dumps       (false)
#define part_freq        (1000)
#define pushsolid        (false)
#define pushrbc          (false)
#define tend             (50)
#define wall_creation    (5000)
#define walls            (false)
#define RBCnv            (498)
