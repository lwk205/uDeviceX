$B/algo/scan/imp.o: $S/algo/scan/cpu/imp.h $S/algo/scan/cpu/type.h $S/algo/scan/cuda/imp.h $S/algo/scan/cuda/type.h $S/algo/scan/dev.h $S/algo/scan/imp.h $B/conf.h $S/d/api.h $S/d/ker.h $S/d/q.h $S/inc/conf.h $S/inc/dev.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/kl.h
$B/clist/imp.o: $S/algo/scan/imp.h $S/clist/dev.h $S/clist/dev/main.h $S/clist/imp.h $S/clist/imp/fin.h $S/clist/imp/ini.h $S/clist/imp/main.h $S/clist/imp/type.h $B/conf.h $S/d/api.h $S/inc/conf.h $S/inc/def.h $S/inc/dev.h $S/inc/type.h $S/partlist/dev.h $S/partlist/type.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/kl.h $S/utils/msg.h
$B/conf/imp.o: $S/conf/imp.h $S/conf/imp/get.h $S/conf/imp/main.h $S/conf/imp/set.h $S/conf/imp/type.h $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/coords/conf.o: $S/conf/imp.h $S/coords/ini.h $S/utils/error.h $S/utils/imp.h
$B/coords/imp.o: $B/conf.h $S/coords/imp.h $S/coords/imp/main.h $S/coords/imp/type.h $S/coords/ini.h $S/coords/type.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h
$B/d/api.o: $B/conf.h $S/d/api.h $S/d/common.h $S/d/cpu/imp.h $S/d/cuda/imp.h $S/inc/conf.h $S/utils/error.h $S/utils/imp.h
$B/mpi/glb.o: $B/conf.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/mc.h
$B/mpi/type.o: $S/inc/conf.h $S/inc/type.h $S/mpi/type.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/u/clist/main.o: $S/algo/scan/imp.h $S/clist/imp.h $B/conf.h $S/conf/imp.h $S/coords/imp.h $S/coords/ini.h $S/d/api.h $S/inc/conf.h $S/inc/dev.h $S/inc/type.h $S/mpi/glb.h $S/mpi/wrapper.h $S/partlist/type.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h $S/utils/msg.h
$B/utils/cc.o: $B/conf.h $S/d/api.h $S/inc/conf.h $S/utils/cc/common.h $S/utils/error.h
$B/utils/error.o: $S/utils/error.h $S/utils/msg.h
$B/utils/imp.o: $S/utils/error.h $S/utils/imp.h
$B/utils/mc.o: $B/conf.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/mc.h
$B/utils/msg.o: $S/utils/msg.h
$B/utils/os.o: $S/utils/error.h $S/utils/msg.h $S/utils/os.h
