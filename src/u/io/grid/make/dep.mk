$B/conf/imp.o: $S/utils/imp.h $S/utils/error.h $S/conf/imp/set.h $S/conf/imp.h $S/conf/imp/main.h $S/conf/imp/type.h $S/conf/imp/get.h $S/utils/msg.h
$B/coords/conf.o: $S/utils/imp.h $S/coords/ini.h $S/utils/error.h $S/conf/imp.h
$B/coords/imp.o: $S/utils/imp.h $S/inc/conf.h $S/coords/ini.h $S/utils/error.h $S/mpi/wrapper.h $S/coords/imp.h $S/utils/mc.h $B/conf.h $S/coords/imp/main.h $S/coords/imp/type.h $S/coords/type.h
$B/d/api.o: $S/d/cpu/imp.h $S/utils/imp.h $S/inc/conf.h $S/utils/error.h $S/d/common.h $S/d/api.h $B/conf.h $S/d/cuda/release/alloc.h $S/d/cuda/debug/alloc.h $S/d/cuda/imp.h
$B/io/grid/h5/imp.o: $S/utils/error.h $S/mpi/wrapper.h $S/io/grid/h5/imp.h $S/coords/ini.h $S/coords/imp.h
$B/io/grid/imp.o: $S/inc/conf.h $S/io/grid/xmf/imp.h $S/utils/error.h $S/mpi/wrapper.h $S/io/grid/h5/imp.h $S/utils/mc.h $B/conf.h
$B/io/grid/xmf/imp.o: $S/utils/imp.h $S/utils/error.h $S/io/grid/xmf/imp.h
$B/mpi/glb.o: $S/inc/conf.h $S/mpi/wrapper.h $S/utils/mc.h $B/conf.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/u/io/grid/main.o: $S/utils/imp.h $S/inc/conf.h $S/utils/error.h $S/mpi/wrapper.h $S/conf/imp.h $S/io/grid/imp.h $S/coords/ini.h $S/utils/mc.h $B/conf.h $S/mpi/glb.h $S/utils/msg.h $S/coords/imp.h
$B/utils/cc.o: $S/utils/cc/common.h $S/inc/conf.h $S/utils/error.h $S/d/api.h $B/conf.h
$B/utils/error.o: $S/utils/msg.h $S/utils/error.h
$B/utils/imp.o: $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/utils/mc.o: $S/inc/conf.h $S/utils/error.h $S/mpi/wrapper.h $S/utils/mc.h $B/conf.h
$B/utils/msg.o: $S/utils/msg.h
$B/utils/nvtx/imp.o: $S/utils/error.h
$B/utils/os.o: $S/utils/os.h $S/utils/error.h $S/utils/msg.h
$B/utils/string/imp.o: $S/utils/error.h $S/utils/string/imp.h
