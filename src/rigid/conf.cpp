#include <stdio.h>
#include <vector_types.h>

#include "utils/error.h"
#include "conf/imp.h"

#include "imp.h"

void rig_pininfo_set_conf(const Config *cfg, RigPinInfo *pi) {
    int3 com, axis;
    int pdir;

    UC(conf_lookup_int3(cfg, "rig.pin_com", &com));
    UC(conf_lookup_int3(cfg, "rig.pin_axis", &axis));
    UC(conf_lookup_int(cfg, "rig.pdir", &pdir));

    UC(rig_pininfo_set(com, axis, pi));
    UC(rig_pininfo_set_pdir(pdir, pi));
}
