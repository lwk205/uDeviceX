/* DOMAIN */
#define XS          20
#define YS          20
#define ZS          20
#define XWM          6
#define YWM          6
#define ZWM          6
#define XBBM         1.f
#define YBBM         1.f
#define ZBBM         1.f

/* DPD */
#define numberdensity      10
#define dt               1e-5
#define flu_mass          1.0
#define rbc_mass         0.25
#define solid_mass        1.0

#define adpd_b     2.66667
#define adpd_br    2.66667
#define adpd_r     2.66667
#define gdpd_b       11.5
#define gdpd_br      8.25
#define gdpd_r          5
#define kBT    0.00444302

#define multi_solvent           true
#define color_freq              500
#define contactforces           false
#define ljsigma                 0.3
#define ljepsilon               0.44
#define walls                   false
#define wall_creation           1000
#define tend                    1000000

/* FLOW TYPE */
#define WVEL_FLAT               true

/* DUMPS */
#define dump_all_fields         true
#define part_freq               20000
#define field_dumps             true
#define strt_dumps              true
#define strt_freq               2000000

#define           RBC_RND       true
#define       DPD_GRAVITY       true
#define        field_freq      10000
#define         fsiforces       true
#define        part_dumps      true

#define              rbcs       true
#define             RBCnv       642
#define   RBC_PARAMS_LINA       true

#define     wall_creation       1000
#define        WVEL_PAR_A        (0)
