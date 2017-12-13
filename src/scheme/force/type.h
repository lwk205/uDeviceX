/* "view" structures: to be passed to device funtions */

/* constant force f */
struct FParam_cste_v {
    float3 a;
};

/* double poiseuille */
struct FParam_dp_v {
    float a;
};

/* shear force fx = a * (y - yc) */
struct FParam_shear_v {
    float a;
};

/* 4 rollers mill */
struct FParam_rol_v {
    float a;
};

/* radial force decaying as 1/r */
struct FParam_rad_v {
    float a;
};

enum {
    BODY_FORCE_V_NONE,
    BODY_FORCE_V_CSTE,
    BODY_FORCE_V_DP,
    BODY_FORCE_V_SHEAR,
    BODY_FORCE_V_ROL,
    BODY_FORCE_V_RAD
};

union FParam_v {
    FParam_cste_v cste;
    FParam_dp_v dp;
    FParam_shear_v shear;
    FParam_rol_v rol;
    FParam_rad_v rad;
};

/* structure containing parameters on host */

struct FParam {
    FParam_v dev;
    int type;
};
