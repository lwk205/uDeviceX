$B/conf/imp.o: $S/conf/imp.cpp; $X -I$S/conf -I$S/.
$B/coords/conf.o: $S/coords/conf.cpp; $X -I$S/coords -I$S/.
$B/coords/imp.o: $S/coords/imp.cpp; $X -I$S/coords -I$S/.
$B/d/api.o: $S/d/api.cu; $N -I$S/d -I$S/.
$B/mpi/glb.o: $S/mpi/glb.cpp; $X -I$S/mpi -I$S/.
$B/mpi/wrapper.o: $S/mpi/wrapper.cpp; $X -I$S/mpi -I$S/.
$B/u/io/h5/main.o: $S/u/io/h5/main.cpp; $X -I$S/.
$B/utils/cc.o: $S/utils/cc.cpp; $X -I$S/utils -I$S/.
$B/utils/error.o: $S/utils/error.cpp; $X -I$S/utils -I$S/.
$B/utils/imp.o: $S/utils/imp.cpp; $X -I$S/utils -I$S/.
$B/utils/mc.o: $S/utils/mc.cpp; $X -I$S/utils -I$S/.
$B/utils/msg.o: $S/utils/msg.cpp; $X -I$S/utils -I$S/.
$B/utils/nvtx/imp.o: $S/utils/nvtx/imp.cpp; $X -I$S/utils/nvtx -I$S/.
$B/utils/os.o: $S/utils/os.cpp; $X -I$S/utils -I$S/.
$B/utils/string/imp.o: $S/utils/string/imp.cpp; $X -I$S/utils/string -I$S/.
