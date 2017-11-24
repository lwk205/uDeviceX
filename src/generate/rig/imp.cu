#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <vector>

#include <conf.h>
#include "inc/conf.h"
#include "inc/def.h"

#include "mpi/wrapper.h"
#include "mpi/type.h"
#include "mpi/glb.h"
#include "utils/mc.h"

#include "msg.h"
#include "utils/error.h"
#include "utils/efopen.h"

#include "inc/type.h"

#include "math/linal.h"
#include "mesh/props.h"
#include "mesh/dist.h"
#include "mesh/bbox.h"
#include "d/ker.h"
#include "d/api.h"
#include "utils/cc.h"
#include "utils/texo.h"
#include "mesh/collision.h"

#include "imp.h"

namespace gen {
#include "imp/ids.h"
#include "imp/ic.h"
#include "imp/share.h"
#include "imp/ini_props.h"
#include "imp/main.h"
} // gen
