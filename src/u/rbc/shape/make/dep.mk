$B/d/api.o: $S/d/cpu/imp.h $S/utils/imp.h $S/inc/conf.h $S/utils/error.h $S/d/common.h $S/d/api.h $B/conf.h $S/d/cuda/imp.h
$B/mpi/glb.o: $S/inc/conf.h $S/mpi/wrapper.h $S/utils/mc.h $B/conf.h $S/mpi/glb.h
$B/mpi/type.o: $S/inc/conf.h $S/inc/type.h $S/mpi/type.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/rbc/adj/imp.o: $S/rbc/adj/imp/fin.h $S/utils/imp.h $S/inc/conf.h $S/utils/error.h $S/rbc/adj/imp.h $S/utils/cc.h $S/rbc/adj/imp/ini.h $S/inc/dev.h $S/d/api.h $B/conf.h $S/rbc/adj/type/common.h $S/rbc/adj/imp/type.h $S/rbc/adj/type/dev.h $S/rbc/adj/imp/anti.h $S/rbc/adj/imp/map.h $S/utils/msg.h $S/rbc/edg/imp.h
$B/rbc/edg/imp.o: $S/utils/error.h $S/rbc/edg/imp.h
$B/rbc/rnd/api/imp.o: $S/utils/imp.h $S/inc/conf.h $S/utils/error.h $S/rbc/rnd/api/imp.h $S/rbc/rnd/api/imp/cpu.h $S/rbc/rnd/api/imp/gaussrand.h $B/conf.h $S/rbc/rnd/api/type.h $S/rbc/rnd/api/imp/cuda.h
$B/rbc/rnd/imp.o: $S/utils/imp.h $S/utils/os.h $S/inc/conf.h $S/rbc/rnd/imp/cu.h $S/utils/error.h $S/rbc/rnd/api/imp.h $S/rbc/rnd/imp.h $S/rbc/rnd/imp/seed.h $S/utils/cc.h $S/inc/dev.h $S/d/api.h $B/conf.h $S/rbc/rnd/imp/main.h $S/rbc/rnd/api/type.h $S/rbc/rnd/type.h $S/mpi/glb.h $S/utils/msg.h
$B/u/rbc/rnd/main.o: $S/inc/conf.h $S/utils/cc.h $S/inc/dev.h $S/d/api.h $B/conf.h $S/rbc/rnd/imp.h $S/mpi/glb.h $S/utils/msg.h
$B/utils/cc.o: $S/utils/cc/common.h $S/inc/conf.h $S/utils/error.h $S/d/api.h $B/conf.h
$B/utils/error.o: $S/utils/msg.h $S/utils/error.h
$B/utils/imp.o: $S/utils/error.h $S/utils/imp.h
$B/utils/mc.o: $S/inc/conf.h $S/utils/error.h $S/mpi/wrapper.h $S/utils/mc.h $B/conf.h
$B/utils/msg.o: $S/utils/msg.h
$B/utils/os.o: $S/utils/os.h $S/utils/error.h $S/utils/msg.h
