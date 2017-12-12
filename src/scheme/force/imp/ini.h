void ini_none(/**/ FParam *p) {
    p->type = TYPE_NONE;
}

void ini(FParam_cste_v par, /**/ FParam *p) {
    p->type = TYPE_CSTE;
    p->dev.cste = par;
}

void ini(FParam_dp_v par, /**/ FParam *p) {
    p->type = TYPE_DP;
    p->dev.dp = par;
}

void ini(FParam_shear_v par, /**/ FParam *p) {
    p->type = TYPE_SHEAR;
    p->dev.shear = par;
}

void ini(FParam_rol_v par, /**/ FParam *p) {
    p->type = TYPE_ROL;
    p->dev.rol = par;
}

void ini(FParam_rad_v par, /**/ FParam *p) {
    p->type = TYPE_RAD;
    p->dev.rad = par;
}
