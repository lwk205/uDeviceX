$B/d/api.o: $S/d/cpu/imp.h $S/inc/conf.h $S/d/common.h $S/d/api.h $S/msg.h $B/conf.h $S/d/cuda/imp.h
$B/glb/imp.o: $S/inc/conf.h $S/glb/imp/util.h $S/glb/set.h $S/glb/wvel/imp.h $S/d/api.h $B/conf.h $S/glb/imp/main.h $S/glb/imp/dec.h $S/glb/get.h $S/mpi/glb.h
$B/glb/wvel/imp.o: $S/inc/conf.h $S/glb/wvel/imp/flat.h $S/glb/wvel/imp.h $S/msg.h $B/conf.h $S/glb/wvel/imp/dupire/common.h $S/glb/wvel/imp/dupire/down.h $S/glb/wvel/imp/dupire/up.h $S/glb/wvel/imp/sin.h
$B/io/bop/imp.o: $S/utils/os.h $S/inc/conf.h $S/inc/type.h $S/mpi/wrapper.h $S/io/bop/imp.h $S/inc/def.h $S/mpi/type.h $S/msg.h $S/d/api.h $S/utils/mc.h $B/conf.h $S/mpi/glb.h
$B/io/com.o: $S/utils/os.h $S/inc/conf.h $S/utils/error.h $S/mpi/wrapper.h $S/msg.h $S/utils/mc.h $B/conf.h $S/utils/halloc.h $S/mpi/glb.h
$B/io/diag.o: $S/inc/conf.h $S/inc/type.h $S/mpi/wrapper.h $S/msg.h $B/conf.h $S/io/diag.h $S/mpi/glb.h
$B/io/field/h5/imp.o: $S/mpi/wrapper.h $S/io/field/h5/imp.h $S/msg.h $S/mpi/glb.h
$B/io/field/imp.o: $S/io/field/imp/scalar.h $S/utils/os.h $S/inc/conf.h $S/io/field/xmf/imp.h $S/utils/error.h $S/inc/type.h $S/mpi/wrapper.h $S/io/field/imp.h $S/io/field/h5/imp.h $S/io/field/imp/dump.h $S/utils/mc.h $B/conf.h $S/utils/halloc.h $S/mpi/glb.h
$B/io/fields_grid.o: $S/inc/conf.h $S/inc/type.h $S/utils/cc.h $S/io/fields_grid.h $S/inc/dev.h $S/d/api.h $S/msg.h $B/conf.h $S/io/field/imp.h $S/io/fields_grid/solvent.h $S/io/fields_grid/all.h
$B/io/field/xmf/imp.o: $S/io/field/xmf/imp.h $S/mpi/glb.h
$B/io/mesh/imp.o: $S/utils/os.h $S/inc/conf.h $S/io/mesh/imp/shift/center.h $S/utils/error.h $S/io/mesh/imp/shift/edge.h $S/inc/type.h $S/io/mesh/imp.h $B/conf.h $S/io/mesh/imp/main.h $S/utils/halloc.h $S/io/mesh/write/imp.h $S/mpi/glb.h
$B/io/mesh/write/imp.o: $S/inc/conf.h $S/mpi/wrapper.h $S/io/mesh/write/imp.h $S/utils/mc.h $B/conf.h $S/io/mesh/write/imp/main.h $S/mpi/glb.h
$B/io/off.o: $S/io/off/imp.h $S/io/off.h
$B/io/ply.o: $S/io/ply/ascii.h $S/inc/type.h $S/inc/def.h $S/io/ply.h $S/msg.h $S/io/ply/bin.h $S/io/ply/common.h
$B/io/restart.o: $S/inc/conf.h $S/inc/type.h $S/inc/def.h $S/msg.h $B/conf.h $S/io/restart.h $S/mpi/glb.h
$B/io/rig.o: $S/inc/conf.h $S/inc/type.h $B/conf.h
$B/math/linal.o: $S/math/linal.h
$B/mpi/glb.o: $S/inc/conf.h $S/mpi/wrapper.h $S/utils/mc.h $B/conf.h $S/mpi/glb.h
$B/mpi/type.o: $S/inc/conf.h $S/inc/type.h $S/mpi/type.h $S/utils/mc.h
$B/mpi/wrapper.o: $S/mpi/wrapper.h
$B/msg.o: $S/msg.h $S/mpi/glb.h
$B/rbc/adj/imp.o: $S/rbc/adj/imp/fin.h $S/utils/error.h $S/rbc/adj/imp.h $S/rbc/adj/imp/ini.h $S/msg.h $S/rbc/adj/type/common.h $S/utils/halloc.h $S/rbc/adj/imp/map.h $S/rbc/adj/type/hst.h $S/rbc/edg/imp.h
$B/rbc/com/imp.o: $S/rbc/com/imp/fin.h $S/inc/conf.h $S/rbc/com/imp/com.h $S/inc/type.h $S/rbc/com/imp.h $S/utils/cc.h $S/inc/def.h $S/rbc/com/imp/ini.h $S/d/api.h $S/msg.h $S/inc/dev.h $B/conf.h $S/utils/kl.h $S/math/dev.h $S/rbc/com/dev/main.h $S/d/ker.h
$B/rbc/edg/imp.o: $S/rbc/edg/imp.h $S/msg.h
$B/rbc/force/area_volume/imp.o: $S/inc/conf.h $S/rbc/force/area_volume/imp.h $S/utils/cc.h $S/inc/def.h $S/d/api.h $S/msg.h $S/inc/dev.h $B/conf.h $S/rbc/force/area_volume/imp/main.h $S/utils/texo.h $S/utils/texo.dev.h $S/utils/kl.h $S/math/dev.h $S/rbc/force/area_volume/dev/main.h $S/d/ker.h
$B/rbc/force/imp.o: $S/rbc/force/imp/fin.h $S/rbc/force/params/lina.h $S/rbc/force/params/test.h $S/rbc/force/area_volume/imp.h $S/inc/conf.h $S/rbc/force/dev/common.h $S/rbc/type.h $S/rbc/force/dev/rnd0/main.h $S/inc/type.h $S/rbc/force/dev/stress_free1/shape.h $S/rbc/rnd/type.h $S/rbc/force/dev/stress_free0/shape.h $S/rbc/force/imp.h $S/rbc/adj/type/common.h $S/utils/cc.h $S/inc/def.h $S/rbc/adj/type/dev.h $S/rbc/force/imp/ini.h $S/rbc/force/dev/stress_free1/force.h $S/rbc/adj/dev.h $S/d/api.h $S/msg.h $S/inc/dev.h $B/conf.h $S/rbc/force/dev/stress_free0/force.h $S/utils/texo.h $S/rbc/force/imp/forces.h $S/rbc/force/params/area_volume.h $S/rbc/rnd/api/imp.h $S/utils/texo.dev.h $S/utils/te.h $S/utils/kl.h $S/math/dev.h $S/rbc/force/dev/rnd1/main.h $S/rbc/force/dev/main.h $S/rbc/rnd/imp.h $S/d/ker.h
$B/rbc/gen/imp.o: $S/inc/conf.h $S/utils/error.h $S/inc/type.h $S/mpi/wrapper.h $S/rbc/gen/imp.h $S/io/off.h $S/inc/def.h $S/utils/mc.h $S/msg.h $B/conf.h $S/rbc/gen/imp/main.h $S/utils/halloc.h $S/mpi/glb.h
$B/rbc/main/anti/imp.o: $S/utils/error.h $S/rbc/main/anti/imp.h $S/rbc/adj/type/common.h $S/msg.h $S/rbc/adj/type/hst.h $S/utils/halloc.h $S/rbc/adj/imp.h $S/rbc/edg/imp.h
$B/rbc/main/imp.o: $S/rbc/main/imp/fin.h $S/inc/conf.h $S/rbc/type.h $S/utils/error.h $S/inc/type.h $S/rbc/main/imp/util.h $S/rbc/gen/imp.h $S/io/restart.h $S/mpi/wrapper.h $S/rbc/main/imp.h $S/rbc/main/anti/imp.h $S/rbc/adj/type/common.h $S/io/off.h $S/rbc/main/imp/setup.h $S/utils/cc.h $S/inc/def.h $S/rbc/main/imp/generate.h $S/rbc/main/imp/ini.h $S/utils/mc.h $S/d/api.h $S/msg.h $S/inc/dev.h $B/conf.h $S/rbc/adj/type/hst.h $S/utils/halloc.h $S/rbc/adj/imp.h $S/rbc/main/imp/start.h $S/mpi/glb.h
$B/rbc/rnd/api/imp.o: $S/inc/conf.h $S/utils/error.h $S/rbc/rnd/api/imp.h $S/rbc/rnd/api/imp/cpu.h $S/rbc/rnd/api/imp/gaussrand.h $B/conf.h $S/utils/halloc.h $S/rbc/rnd/api/type.h $S/rbc/rnd/api/imp/cuda.h
$B/rbc/rnd/imp.o: $S/utils/os.h $S/inc/conf.h $S/rbc/rnd/imp/cu.h $S/utils/error.h $S/rbc/rnd/api/imp.h $S/rbc/rnd/imp.h $S/rbc/rnd/imp/seed.h $S/utils/cc.h $S/inc/dev.h $S/d/api.h $S/msg.h $B/conf.h $S/rbc/rnd/imp/main.h $S/rbc/rnd/api/type.h $S/utils/halloc.h $S/rbc/rnd/type.h $S/mpi/glb.h
$B/rbc/stretch/imp.o: $S/inc/conf.h $S/utils/error.h $S/inc/type.h $S/rbc/stretch/imp/util.h $S/rbc/stretch/imp.h $S/utils/cc.h $S/inc/def.h $S/d/api.h $S/msg.h $S/inc/dev.h $B/conf.h $S/rbc/stretch/imp/main.h $S/utils/halloc.h $S/rbc/stretch/imp/type.h $S/utils/kl.h $S/rbc/stretch/dev/main.h $S/d/ker.h
$B/scheme/force/imp.o: $S/inc/conf.h $S/inc/type.h $S/d/q.h $S/scheme/force/imp/shear.h $S/scheme/force/dev/constant.h $S/scheme/force/imp.h $S/utils/cc.h $S/inc/def.h $S/scheme/force/imp/double_poiseuille.h $S/scheme/force/dev/shear.h $S/inc/dev.h $S/utils/mc.h $S/msg.h $S/d/api.h $B/conf.h $S/utils/kl.h $S/scheme/force/dev/4roller.h $S/scheme/force/dev/double_poiseuille.h $S/scheme/force/imp/none.h $S/glb/get.h $S/scheme/force/imp/constant.h $S/scheme/force/imp/4roller.h $S/d/ker.h
$B/scheme/move/imp.o: $S/inc/conf.h $S/scheme/move/dev/euler.h $S/inc/type.h $S/d/q.h $S/scheme/move/imp.h $S/scheme/move/dev/vv.h $S/inc/def.h $S/inc/dev.h $S/msg.h $B/conf.h $S/scheme/move/imp/main.h $S/utils/kl.h $S/glb/get.h $S/scheme/move/dev/main.h $S/d/ker.h
$B/scheme/restrain/imp.o: $S/inc/conf.h $S/scheme/restrain/imp/red_vel.h $S/inc/type.h $S/scheme/restrain/imp/rbc_vel.h $S/scheme/restrain/imp.h $S/inc/def.h $S/scheme/restrain/sub/imp.h $S/msg.h $B/conf.h $S/scheme/restrain/imp/none.h
$B/scheme/restrain/sub/imp.o: $S/inc/conf.h $S/scheme/restrain/sub/dev/grey/map.h $S/scheme/restrain/sub/imp/color/main.h $S/scheme/restrain/sub/imp/main0.h $S/scheme/restrain/sub/dev/main0.h $S/inc/type.h $S/d/q.h $S/scheme/restrain/sub/imp.h $S/utils/cc.h $S/scheme/restrain/sub/imp/grey/main.h $S/scheme/restrain/sub/imp/common.h $S/scheme/restrain/sub/dev/util.h $S/inc/dev.h $S/msg.h $S/d/api.h $B/conf.h $S/scheme/restrain/sub/sum/imp.h $S/scheme/restrain/sub/dev/color/map.h $S/utils/kl.h $S/scheme/restrain/sub/dev/dec.h $S/scheme/restrain/sub/stat/imp.h $S/scheme/restrain/sub/dev/main.h $S/d/ker.h
$B/scheme/restrain/sub/stat/imp.o: $S/scheme/restrain/sub/stat/imp.h $S/scheme/restrain/sub/stat/imp/main.h $S/scheme/restrain/sub/stat/imp/dec.h
$B/scheme/restrain/sub/sum/imp.o: $S/inc/conf.h $S/mpi/wrapper.h $S/scheme/restrain/sub/sum/imp.h $S/utils/mc.h $B/conf.h $S/scheme/restrain/sub/sum/imp/main.h
$B/u/rbc/main/lib/imp.o: $S/inc/conf.h $S/rbc/type.h $S/utils/error.h $S/rbc/stretch/imp.h $S/inc/type.h $S/io/mesh/imp.h $S/u/rbc/main/lib/imp.h $S/io/diag.h $S/scheme/move/imp.h $S/utils/cc.h $S/inc/def.h $S/rbc/main/imp.h $S/rbc/force/area_volume/imp.h $S/inc/dev.h $S/msg.h $S/d/api.h $B/conf.h $S/u/rbc/main/lib/imp/main.h $S/utils/texo.h $S/utils/halloc.h $S/utils/te.h $S/rbc/force/imp.h $S/mpi/glb.h $S/rbc/rnd/imp.h
$B/u/rbc/main/main.o: $S/u/rbc/main/lib/imp.h $S/mpi/glb.h
$B/utils/cc.o: $S/utils/cc/common.h $S/inc/conf.h $S/d/api.h $S/msg.h $B/conf.h
$B/utils/error.o: $S/msg.h $S/utils/error.h
$B/utils/halloc.o: $S/utils/error.h $S/utils/halloc.h
$B/utils/mc.o: $S/inc/conf.h $S/utils/mc.h $B/conf.h
$B/utils/os.o: $S/utils/os.h
