$B/algo/scan/imp.o: $S/algo/scan/imp.cu; $N -I$S/. -I$S/algo/scan
$B/clist/imp.o: $S/clist/imp.cu; $N -I$S/. -I$S/clist
$B/d/api.o: $S/d/api.cu; $N -I$S/. -I$S/d
$B/glb.o: $S/glb.cu; $N -I$S/.
$B/mpi/glb.o: $S/mpi/glb.cpp; $X -I$S/. -I$S/mpi
$B/mpi/reduce.o: $S/mpi/reduce.cpp; $X -I$S/. -I$S/mpi
$B/mpi/type.o: $S/mpi/type.cpp; $X -I$S/. -I$S/mpi
$B/mpi/wrapper.o: $S/mpi/wrapper.cpp; $X -I$S/. -I$S/mpi
$B/msg.o: $S/msg.cpp; $X -I$S/.
$B/u/clist/main.o: $S/u/clist/main.cu; $N -I$S/. -I$S/u/clist
$B/utils/cc.o: $S/utils/cc.cpp; $X -I$S/. -I$S/utils
$B/utils/mc.o: $S/utils/mc.cpp; $X -I$S/. -I$S/utils
$B/utils/os.o: $S/utils/os.cpp; $X -I$S/. -I$S/utils
