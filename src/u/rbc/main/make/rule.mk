$B/coords/imp.o: $S/coords/imp.cpp; $X -I$S/. -I$S/coords
$B/d/api.o: $S/d/api.cu; $N -I$S/. -I$S/d
$B/io/bop/imp.o: $S/io/bop/imp.cpp; $X -I$S/. -I$S/io/bop
$B/io/com/imp.o: $S/io/com/imp.cpp; $X -I$S/. -I$S/io/com
$B/io/diag/imp.o: $S/io/diag/imp.cpp; $X -I$S/. -I$S/io/diag
$B/io/field/h5/imp.o: $S/io/field/h5/imp.cpp; $X -I$S/. -I$S/io/field/h5
$B/io/field/imp.o: $S/io/field/imp.cpp; $X -I$S/. -I$S/io/field
$B/io/field/xmf/imp.o: $S/io/field/xmf/imp.cpp; $X -I$S/. -I$S/io/field/xmf
$B/io/fields_grid/imp.o: $S/io/fields_grid/imp.cpp; $X -I$S/. -I$S/io/fields_grid
$B/io/mesh/imp.o: $S/io/mesh/imp.cpp; $X -I$S/. -I$S/io/mesh
$B/io/mesh/write/imp.o: $S/io/mesh/write/imp.cpp; $X -I$S/. -I$S/io/mesh/write
$B/io/off/imp.o: $S/io/off/imp.cpp; $X -I$S/. -I$S/io/off
$B/io/ply/imp.o: $S/io/ply/imp.cpp; $X -I$S/. -I$S/io/ply
$B/io/restart/imp.o: $S/io/restart/imp.cpp; $X -I$S/. -I$S/io/restart
$B/io/rig/imp.o: $S/io/rig/imp.cpp; $X -I$S/. -I$S/io/rig
$B/io/txt/imp.o: $S/io/txt/imp.cpp; $X -I$S/. -I$S/io/txt
$B/math/linal/imp.o: $S/math/linal/imp.cpp; $X -I$S/. -I$S/math/linal
$B/math/rnd/imp.o: $S/math/rnd/imp.cpp; $X -I$S/. -I$S/math/rnd
$B/math/tform/imp.o: $S/math/tform/imp.cpp; $X -I$S/. -I$S/math/tform
$B/mpi/glb.o: $S/mpi/glb.cpp; $X -I$S/. -I$S/mpi
$B/mpi/type.o: $S/mpi/type.cpp; $X -I$S/. -I$S/mpi
$B/mpi/wrapper.o: $S/mpi/wrapper.cpp; $X -I$S/. -I$S/mpi
$B/parser/imp.o: $S/parser/imp.cpp; $X -I$S/. -I$S/parser
$B/rbc/adj/imp.o: $S/rbc/adj/imp.cpp; $X -I$S/. -I$S/rbc/adj
$B/rbc/com/imp.o: $S/rbc/com/imp.cu; $N -I$S/. -I$S/rbc/com
$B/rbc/edg/imp.o: $S/rbc/edg/imp.cpp; $X -I$S/. -I$S/rbc/edg
$B/rbc/force/area_volume/imp.o: $S/rbc/force/area_volume/imp.cu; $N -I$S/. -I$S/rbc/force/area_volume
$B/rbc/force/imp.o: $S/rbc/force/imp.cu; $N -I$S/. -I$S/rbc/force
$B/rbc/gen/imp.o: $S/rbc/gen/imp.cpp; $X -I$S/. -I$S/rbc/gen
$B/rbc/imp.o: $S/rbc/imp.cpp; $X -I$S/. -I$S/rbc
$B/rbc/params/conf.o: $S/rbc/params/conf.cpp; $X -I$S/. -I$S/rbc/params
$B/rbc/params/imp.o: $S/rbc/params/imp.cpp; $X -I$S/. -I$S/rbc/params
$B/rbc/rnd/api/imp.o: $S/rbc/rnd/api/imp.cpp; $X -I$S/. -I$S/rbc/rnd/api
$B/rbc/rnd/imp.o: $S/rbc/rnd/imp.cpp; $X -I$S/. -I$S/rbc/rnd
$B/rbc/stretch/imp.o: $S/rbc/stretch/imp.cu; $N -I$S/. -I$S/rbc/stretch
$B/scheme/force/conf.o: $S/scheme/force/conf.cpp; $X -I$S/. -I$S/scheme/force
$B/scheme/force/imp.o: $S/scheme/force/imp.cu; $N -I$S/. -I$S/scheme/force
$B/scheme/move/imp.o: $S/scheme/move/imp.cu; $N -I$S/. -I$S/scheme/move
$B/scheme/restrain/imp.o: $S/scheme/restrain/imp.cpp; $X -I$S/. -I$S/scheme/restrain
$B/scheme/restrain/sub/imp.o: $S/scheme/restrain/sub/imp.cu; $N -I$S/. -I$S/scheme/restrain/sub
$B/scheme/restrain/sub/stat/imp.o: $S/scheme/restrain/sub/stat/imp.cpp; $X -I$S/. -I$S/scheme/restrain/sub/stat
$B/scheme/restrain/sub/sum/imp.o: $S/scheme/restrain/sub/sum/imp.cpp; $X -I$S/. -I$S/scheme/restrain/sub/sum
$B/u/rbc/main/lib/imp.o: $S/u/rbc/main/lib/imp.cu; $N -I$S/. -I$S/u/rbc/main/lib
$B/u/rbc/main/main.o: $S/u/rbc/main/main.cpp; $X -I$S/. -I$S/u/rbc/main
$B/utils/cc.o: $S/utils/cc.cpp; $X -I$S/. -I$S/utils
$B/utils/error.o: $S/utils/error.cpp; $X -I$S/. -I$S/utils
$B/utils/imp.o: $S/utils/imp.cpp; $X -I$S/. -I$S/utils
$B/utils/mc.o: $S/utils/mc.cpp; $X -I$S/. -I$S/utils
$B/utils/msg.o: $S/utils/msg.cpp; $X -I$S/. -I$S/utils
$B/utils/os.o: $S/utils/os.cpp; $X -I$S/. -I$S/utils
