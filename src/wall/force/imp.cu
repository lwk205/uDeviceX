#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <conf.h>
#include "inc/conf.h"
#include "inc/def.h"
#include "msg.h"
#include "glb.h"

#include "d/api.h"
#include "d/q.h"
#include "d/ker.h"

#include "utils/cc.h"

#include "inc/type.h"
#include "inc/dev.h"
#include "utils/texo.h"
#include "utils/te.h"

#include "inc/macro.h"

#include "rnd/imp.h"
#include "rnd/dev.h"

#include "sdf/type.h"
#include "sdf/int.h"
#include "sdf/cheap.dev.h"

#include "inc/dev/wvel.h"
#include "forces/type.h"
#include "forces/use.h"
#include "forces/pack.h"
#include "forces/imp.h"

#include "cloud/hforces/type.h"
#include "cloud/hforces/get.h"
#include "algo/scan/int.h"
#include "clist/imp.h"

#include "utils/kl.h"
#include "imp.h"

namespace wall {
/*** generitc ***/
namespace dev {
  namespace map {
    #include "dev/map/type.h"
    #include "dev/map/ini.h"
    #include "dev/map/use.h"
  }
  #include "dev/pair0.h"
}

/*** polymorphic ***/
namespace grey {
  namespace dev {
    #include "dev/fetch/grey.h"
    #include "dev/pair.h"
  }
  #include "imp/pair.h"
}

namespace color {
  namespace dev {
    #include "dev/fetch/color.h"
    #include "dev/pair.h"
  }
  #include "imp/pair.h"
}
}
