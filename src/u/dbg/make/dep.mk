$B/d/api.o: $B/conf.h $S/d/api.h $S/d/common.h $S/d/cpu/imp.h $S/d/cuda/imp.h $S/inc/conf.h $S/utils/error.h $S/utils/halloc.h
$B/dbg/imp.o: $B/conf.h $S/d/api.h $S/dbg/dev/clist.h $S/dbg/dev/color.h $S/dbg/dev/common.h $S/dbg/dev/force.h $S/dbg/dev/pos.h $S/dbg/dev/vel.h $S/dbg/error.h $S/dbg/imp.h $S/dbg/macro/switch.h $S/inc/conf.h $S/inc/def.h $S/inc/dev.h $S/inc/type.h $S/msg.h $S/utils/cc.h $S/utils/kl.h
$B/mpi/glb.o: $B/conf.h $S/inc/conf.h $S/mpi/glb.h $S/mpi/wrapper.h $S/utils/mc.h
$B/mpi/type.o: $S/inc/conf.h $S/inc/type.h $S/mpi/type.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/msg.o: $S/mpi/glb.h $S/msg.h
$B/u/dbg/main.o: $B/conf.h $S/d/api.h $S/dbg/imp.h $S/inc/conf.h $S/inc/dev.h $S/inc/type.h $S/mpi/glb.h $S/mpi/wrapper.h $S/msg.h $S/utils/cc.h $S/utils/kl.h
$B/utils/cc.o: $B/conf.h $S/d/api.h $S/inc/conf.h $S/utils/cc/common.h $S/utils/error.h
$B/utils/efopen.o: $S/utils/efopen.h $S/utils/error.h
$B/utils/error.o: $S/msg.h $S/utils/error.h
$B/utils/halloc.o: $S/utils/error.h $S/utils/halloc.h
$B/utils/mc.o: $B/conf.h $S/inc/conf.h $S/utils/error.h $S/utils/mc.h
$B/utils/os.o: $S/msg.h $S/utils/error.h $S/utils/os.h
