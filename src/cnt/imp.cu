#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <conf.h>
#include "inc/conf.h"

#include "d/api.h"
#include "d/q.h"
#include "d/ker.h"

#include "algo/scan/imp.h"

#include "rnd/imp.h"
#include "rnd/dev.h"

#include "inc/def.h"
#include "msg.h"
#include "mpi/glb.h"
#include "utils/cc.h"

#include "frag/imp.h"

#include "utils/kl.h"
#include "inc/type.h"
#include "inc/dev.h"

#include "dbg/imp.h"

#include "inc/dev/common.h"

#include "forces/type.h"
#include "forces/use.h"
#include "forces/pack.h"
#include "forces/imp.h"

#include "clist/imp.h"
#include "clist/code.h"

#include "imp.h"

namespace cnt {

typedef Sarray<const float2*, MAX_OBJ_TYPES> float2pWraps;
typedef Sarray<      float *, MAX_OBJ_TYPES>  ForcepWraps;

enum {
    XOFFSET = XS / 2,
    YOFFSET = YS / 2,
    ZOFFSET = ZS / 2
};

namespace dev {
#include "dev/pair.h"
#include "dev/map/common.h"
#include "dev/map/halo.h"
#include "dev/map/bulk.h"
#include "dev/bulk.h"
#include "dev/halo.h"
}

#include "imp/bulk.h"
#include "imp/halo.h"
#include "imp/main.h"

} /* namespace */
