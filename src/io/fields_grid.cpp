#include <stdio.h>
#include <math.h>
#include <mpi.h>

#include <conf.h>
#include "inc/conf.h"

#include "inc/type.h"
#include "msg.h"
#include "utils/error.h"

#include "d/api.h"

#include "field/imp.h"
#include "utils/cc.h"
#include "inc/dev.h"
#include "mpi/wrapper.h"

#include "fields_grid.h"

/* body */
#if dump_all_fields
  #include "fields_grid/all.h"
#else
  #include "fields_grid/solvent.h"
#endif
