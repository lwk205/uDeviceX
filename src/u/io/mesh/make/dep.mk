$B/conf/imp.o: $S/conf/imp.h $S/conf/imp/get.h $S/conf/imp/main.h $S/conf/imp/set.h $S/conf/imp/type.h $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/coords/conf.o: $S/conf/imp.h $S/coords/ini.h $S/utils/error.h $S/utils/imp.h
$B/coords/imp.o: $B/conf.h $S/coords/imp.h $S/coords/imp/main.h $S/coords/imp/type.h $S/coords/ini.h $S/coords/type.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h
$B/d/api.o: $B/conf.h $S/d/api.h $S/d/common.h $S/d/cpu/imp.h $S/d/cuda/imp.h $S/inc/conf.h $S/utils/error.h $S/utils/imp.h
$B/io/mesh/imp.o: $B/conf.h $S/coords/imp.h $S/inc/conf.h $S/inc/type.h $S/io/mesh/imp.h $S/io/mesh/imp/main.h $S/io/mesh/imp/new.h $S/io/mesh/imp/shift/center.h $S/io/mesh/imp/shift/edge.h $S/io/mesh/imp/type.h $S/io/mesh/imp/util.h $S/io/mesh/write/imp.h $S/io/mesh_read/imp.h $S/mesh/vectors/imp.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h $S/utils/msg.h $S/utils/os.h
$B/io/mesh/write/imp.o: $B/conf.h $S/inc/conf.h $S/io/mesh/write/imp.h $S/io/mesh/write/imp/main.h $S/io/mesh/write/imp/type.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h
$B/io/mesh_read/imp.o: $S/io/mesh_read/imp.h $S/io/mesh_read/imp/main.h $S/io/mesh_read/imp/off.h $S/io/mesh_read/imp/ply.h $S/io/mesh_read/imp/type.h $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/math/tform/imp.o: $B/conf.h $S/inc/conf.h $S/math/tform/imp.h $S/math/tform/imp/main.h $S/math/tform/imp/type.h $S/math/tform/type.h $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/mesh/vectors/imp.o: $S/coords/imp.h $S/inc/type.h $S/math/tform/imp.h $S/mesh/vectors/imp.h $S/mesh/vectors/imp/main.h $S/mesh/vectors/imp/type.h $S/utils/error.h $S/utils/imp.h
$B/mpi/glb.o: $B/conf.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/mc.h
$B/mpi/type.o: $S/inc/conf.h $S/inc/type.h $S/mpi/type.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/u/io/mesh/main.o: $B/conf.h $S/conf/imp.h $S/coords/ini.h $S/inc/conf.h $S/io/mesh/imp.h $S/io/mesh_read/imp.h $S/mpi/glb.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/mc.h $S/utils/msg.h
$B/utils/cc.o: $B/conf.h $S/d/api.h $S/inc/conf.h $S/utils/cc/common.h $S/utils/error.h
$B/utils/error.o: $S/utils/error.h $S/utils/msg.h
$B/utils/imp.o: $S/utils/error.h $S/utils/imp.h
$B/utils/mc.o: $B/conf.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/mc.h
$B/utils/msg.o: $S/utils/msg.h
$B/utils/os.o: $S/utils/error.h $S/utils/msg.h $S/utils/os.h
