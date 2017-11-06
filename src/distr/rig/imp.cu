#include <mpi.h>
#include <assert.h>

#include <conf.h>
#include "inc/conf.h"

#include "msg.h"
#include "d/api.h"
#include "utils/cc.h"
#include "utils/kl.h"
#include "utils/error.h"
#include "inc/dev.h"
#include "inc/dev/common.h"
#include "inc/type.h"
#include "inc/def.h"

#include "frag/imp.h"
#include "mpi/basetags.h"
#include "comm/imp.h"
#include "comm/utils.h"

#include "rig/imp.h"

#include "distr/map/type.h"
#include "type.h"
#include "imp.h"

namespace distr {
namespace rig {
using namespace comm;

#include "distr/map/dev.h"
#include "distr/common/dev.h"
#include "distr/map/imp.h"
#include "dev.h"
#include "imp/ini.h"
#include "imp/fin.h"
#include "imp/map.h"
#include "imp/pack.h"
#include "imp/com.h"
#include "imp/unpack.h"

} // rig
} // distr
