#include <assert.h>
#include <mpi.h>
#include <stdint.h>
#include <conf.h>
#include "inc/conf.h"

#include "d/api.h"
#include "d/q.h"
#include "d/ker.h"

#include "algo/scan/int.h"

#include "rnd/imp.h"
#include "rnd/dev.h"

#include "inc/def.h"
#include "msg.h"
#include "mpi/glb.h"
#include "utils/cc.h"
#include "utils/mc.h"
#include "frag/imp.h"

#include "utils/kl.h"
#include "mpi/basetags.h"
#include "inc/type.h"
#include "mpi/type.h"
#include "inc/dev.h"

#include "utils/texo.h"
#include "utils/te.h"

#include "inc/tmp/pinned.h"
#include "sim/imp.h"
#include "dbg/imp.h"

#include "inc/dev/read.h"
#include "inc/dev/write.h"
#include "inc/dev/common.h"
#include "inc/dev/index.h"

#include "clist/imp.h"
#include "xrbc/imp.h"
#include "forces/type.h"
#include "forces/pack.h"
#include "forces/imp.h"
#include "cloud/hforces/type.h"
#include "cloud/hforces/get.h"

/* local */
#include "type.h"
#include "imp.h"

/* body */
namespace fsi {
namespace dev {
#include "dev/type.h"
#include "dev/decl.h"
#include "dev/fetch.h"
#include "dev/common.h"
#include "dev/map.common.h"
#include "dev/map/bulk.h"
#include "dev/map/halo.h"
#include "dev/pair.h"
#include "dev/bulk.h"
#include "dev/halo.h"
}

#include "imp/decl.h"
#include "imp/setup.h"
#include "imp/bind.h"
#include "imp/bulk.h"
#include "imp/fin.h"
#include "imp/halo.h"
#include "imp/ini.h"
}
