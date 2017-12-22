#include <stdio.h>
#include <mpi.h>
#include <conf.h>
#include "inc/conf.h"

#include "d/ker.h"
#include "d/api.h"
#include "msg.h"

#include "mpi/glb.h"
#include "mpi/wrapper.h"

#include "inc/type.h"
#include "inc/dev.h"
#include "utils/cc.h"
#include "sdf/def.h"
#include "utils/kl.h"
#include "utils/error.h"

#include "glob/type.h"
#include "wvel/type.h"

#include "sdf/imp.h"
#include "sdf/type.h"
#include "sdf/dev.h"
#include "sdf/imp/type.h"


namespace dev {
#include "dev.h"
}

struct Part { float x, y, z; };
static int    argc;
static char **argv;
/* left shift */
void lshift() {
    argc--;
    if (argc < 1) ERR("u/sdf: not enough args");
}

void main0(Sdf *sdf, Part *p) {
    float x, y, z;
    x = p->x; y = p->y; z = p->z;
    KL(dev::main, (1, 1), (sdf->tex, x, y, z));
}

void main1(Part *p) {
    Sdf *sdf;
    UC(ini(&sdf));
    UC(gen(m::cart, sdf));
    UC(main0(sdf, p));
    UC(fin(sdf));
    dSync();
}

void ini_part(/**/ Part *p) {
    float x, y, z;
    x = atof(argv[argc - 1]); lshift();
    y = atof(argv[argc - 1]); lshift();
    z = atof(argv[argc - 1]); lshift();
    p->x = x; p->y = y; p->z = z;
}

void main2() {
    Part p;
    ini_part(/**/ &p);
    m::ini(&argc, &argv);
    UC(main1(&p));
    m::fin();
}

int main(int argc0, char **argv0) {
    argc = argc0; argv = argv0;
    UC(main2());
}
