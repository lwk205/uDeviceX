$B/algo/scan/imp.o: $S/algo/scan/cpu/imp.h $S/algo/scan/cpu/type.h $S/algo/scan/cuda/imp.h $S/algo/scan/cuda/type.h $S/algo/scan/dev.h $S/algo/scan/imp.h $B/conf.h $S/d/api.h $S/d/ker.h $S/d/q.h $S/inc/conf.h $S/inc/dev.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/kl.h
$B/clist/imp.o: $S/algo/scan/imp.h $S/clist/dev.h $S/clist/dev/main.h $S/clist/imp.h $S/clist/imp/fin.h $S/clist/imp/ini.h $S/clist/imp/main.h $S/clist/imp/type.h $B/conf.h $S/d/api.h $S/inc/conf.h $S/inc/def.h $S/inc/dev.h $S/inc/type.h $S/partlist/dev.h $S/partlist/type.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/kl.h $S/utils/msg.h
$B/conf/imp.o: $S/conf/imp.h $S/conf/imp/get.h $S/conf/imp/main.h $S/conf/imp/set.h $S/conf/imp/type.h $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/coords/conf.o: $S/conf/imp.h $S/coords/ini.h $S/utils/error.h $S/utils/imp.h
$B/coords/imp.o: $B/conf.h $S/coords/imp.h $S/coords/imp/main.h $S/coords/imp/type.h $S/coords/ini.h $S/coords/type.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h
$B/d/api.o: $B/conf.h $S/d/api.h $S/d/common.h $S/d/cpu/imp.h $S/d/cuda/imp.h $S/inc/conf.h $S/utils/error.h $S/utils/imp.h
$B/farray/imp.o: $S/farray/imp.h $S/farray/imp/main.h $S/farray/type.h
$B/fluforces/bulk/imp.o: $B/conf.h $S/d/api.h $S/d/ker.h $S/farray/dev.h $S/farray/imp.h $S/farray/type.h $S/fluforces/bulk/dev/fetch.h $S/fluforces/bulk/dev/main.h $S/fluforces/bulk/imp.h $S/fluforces/bulk/imp/main.h $S/fluforces/bulk/imp/type.h $S/inc/conf.h $S/inc/def.h $S/inc/dev.h $S/inc/type.h $S/math/dev.h $S/math/rnd/dev.h $S/math/rnd/imp.h $S/pair/dev.h $S/pair/imp.h $S/pair/type.h $S/utils/cc.h $S/utils/error.h $S/utils/kl.h $S/utils/texo.dev.h $S/utils/texo.h
$B/fluforces/halo/imp.o: $B/conf.h $S/d/api.h $S/d/ker.h $S/d/q.h $S/farray/dev.h $S/farray/imp.h $S/farray/type.h $S/flu/type.h $S/fluforces/halo/dev/dbg.h $S/fluforces/halo/dev/main.h $S/fluforces/halo/dev/map.h $S/fluforces/halo/imp.h $S/fluforces/halo/imp/main.h $S/fluforces/halo/imp/type.h $S/frag/dev.h $S/frag/imp.h $S/inc/conf.h $S/inc/def.h $S/inc/dev.h $S/inc/type.h $S/math/dev.h $S/math/rnd/dev.h $S/pair/dev.h $S/pair/imp.h $S/pair/type.h $S/parray/dev.h $S/parray/imp.h $S/parray/type.h $S/utils/cc.h $S/utils/error.h $S/utils/kl.h
$B/fluforces/imp.o: $B/conf.h $S/d/api.h $S/flu/type.h $S/fluforces/bulk/imp.h $S/fluforces/dev/main.h $S/fluforces/halo/imp.h $S/fluforces/imp.h $S/fluforces/imp/fin.h $S/fluforces/imp/ini.h $S/fluforces/imp/main.h $S/fluforces/imp/type.h $S/frag/imp.h $S/inc/conf.h $S/inc/dev.h $S/inc/type.h $S/math/rnd/dev.h $S/math/rnd/imp.h $S/mpi/wrapper.h $S/parray/imp.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/kl.h $S/utils/mc.h $S/utils/msg.h
$B/frag/imp.o: $S/frag/dev.h $S/frag/imp.h
$B/io/txt/imp.o: $S/inc/type.h $S/io/txt/imp.h $S/io/txt/imp/dump.h $S/io/txt/imp/read.h $S/io/txt/imp/type.h $S/utils/error.h $S/utils/imp.h $S/utils/msg.h
$B/math/rnd/imp.o: $S/math/rnd/imp.h $S/utils/error.h $S/utils/imp.h
$B/mpi/glb.o: $B/conf.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/mc.h
$B/mpi/type.o: $S/inc/conf.h $S/inc/type.h $S/mpi/type.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/pair/conf.o: $S/conf/imp.h $S/pair/imp.h $S/pair/type.h $S/utils/error.h
$B/pair/imp.o: $S/pair/imp.h $S/pair/imp/main.h $S/pair/imp/type.h $S/pair/type.h $S/utils/error.h $S/utils/imp.h
$B/parray/imp.o: $S/parray/imp.h $S/parray/imp/main.h $S/parray/type.h
$B/u/bulk/main.o: $S/clist/imp.h $B/conf.h $S/conf/imp.h $S/coords/imp.h $S/coords/ini.h $S/d/api.h $S/farray/imp.h $S/flu/type.h $S/fluforces/imp.h $S/inc/conf.h $S/inc/dev.h $S/inc/type.h $S/io/txt/imp.h $S/mpi/glb.h $S/mpi/wrapper.h $S/pair/imp.h $S/parray/imp.h $S/partlist/type.h $S/utils/cc.h $S/utils/error.h $S/utils/imp.h $S/utils/mc.h $S/utils/msg.h
$B/utils/cc.o: $B/conf.h $S/d/api.h $S/inc/conf.h $S/utils/cc/common.h $S/utils/error.h
$B/utils/error.o: $S/utils/error.h $S/utils/msg.h
$B/utils/imp.o: $S/utils/error.h $S/utils/imp.h
$B/utils/mc.o: $B/conf.h $S/inc/conf.h $S/mpi/wrapper.h $S/utils/error.h $S/utils/mc.h
$B/utils/msg.o: $S/utils/msg.h
$B/utils/os.o: $S/utils/error.h $S/utils/msg.h $S/utils/os.h
