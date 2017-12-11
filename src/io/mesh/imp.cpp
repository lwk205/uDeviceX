#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector_types.h>

#include <conf.h>
#include "inc/conf.h"

#include "glob/type.h"
#include "glob/imp.h"

#include "utils/os.h"
#include "utils/error.h"
#include "utils/halloc.h"
#include "mpi/glb.h"
#include "inc/type.h"
#include "write/imp.h"

#include "imp.h"

namespace io { namespace mesh {
#if   defined(MESH_SHIFT_EDGE)
   #include "imp/shift/edge.h"
#elif defined(MESH_SHIFT_CENTER)
   #include "imp/shift/center.h"
#else
   #error     MESH_SHIFT_* is undefined
#endif
#include "imp/main.h"

}} /* namespace */
