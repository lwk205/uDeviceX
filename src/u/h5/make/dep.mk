$B/d/api.o: $B/conf.h $S/d/api.h $S/d/common.h $S/d/cpu/imp.h $S/d/cuda/imp.h $S/inc/conf.h $S/utils/error.h $S/utils/halloc.h
$B/io/field/h5/imp.o: $S/io/field/h5/imp.h $S/mpi/glb.h $S/mpi/wrapper.h $S/utils/error.h
$B/io/field/xmf/imp.o: $S/io/field/xmf/imp.h $S/mpi/glb.h $S/utils/efopen.h $S/utils/error.h
$B/mpi/glb.o: $B/conf.h $S/inc/conf.h $S/mpi/glb.h $S/mpi/wrapper.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/msg.o: $S/mpi/glb.h $S/msg.h
$B/u/h5/main.o: $S/io/field/h5/imp.h $S/io/field/xmf/imp.h $S/mpi/glb.h $S/mpi/wrapper.h $S/msg.h $S/utils/error.h $S/utils/halloc.h
$B/utils/efopen.o: $S/utils/efopen.h $S/utils/error.h
$B/utils/error.o: $S/msg.h $S/utils/error.h
$B/utils/halloc.o: $S/utils/error.h $S/utils/halloc.h
$B/utils/mc.o: $B/conf.h $S/inc/conf.h $S/utils/error.h $S/utils/mc.h
