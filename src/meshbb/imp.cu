#include <assert.h>
#include <stdio.h>

#include <conf.h>
#include "inc/conf.h"
#include "inc/dev.h"
#include "utils/kl.h"
#include "utils/cc.h"
#include "inc/type.h"
#include "d/api.h"

#include "math/dev.h"

#include "imp.h"

namespace meshbb {

/* conf */
enum {MAX_COL = 4};

#include "type.h"
#include "bbstates.h"
#include "dev/roots.h"
#include "dev/utils.h"
#include "dev/intersection.h"
#include "dev/collect.h"
#include "dev/main.h"

#include "imp/main.h"

#ifdef MESHBB_LOG_ROOTS
#include "imp/find_collisions/log_root1.h"
#else
#include "imp/find_collisions/log_root0.h"
#endif


} /* meshbb */
