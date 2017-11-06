#include <stdio.h>
#include <assert.h>

#include <conf.h>
#include "inc/conf.h"
#include "inc/type.h"
#include "inc/dev.h"
#include "utils/cc.h"
#include "d/api.h"
#include "utils/kl.h"
#include "msg.h"

#include "imp.h"

namespace rig {

enum {X, Y, Z};
enum {XX, XY, XZ, YY, YZ, ZZ};
enum {YX = XY, ZX = XZ, ZY = YZ};

namespace dev {
#include "dev/utils.h"
#include "dev/main.h"
}

#include "imp/main.h"

} // rig
