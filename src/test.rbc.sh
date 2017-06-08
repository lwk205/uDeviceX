
#### RBC in a periodic box
# nTEST: diag.t2
# export PATH=../tools:$PATH
# rm -rf diag.txt h5 bop ply
# x=0.75 y=8 z=12; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 > rbcs-ic.txt
# :
# argp .conf.couette.h -rbcs -tend=0.5 -part_freq=300 > .conf.h
# :
# { make clean && make -j && make -C ../tools; } > /dev/null
# ./udx
# ply2punto ply/rbcs-00003.ply | uscale 100 > ply.out.txt

#### RBC initialy rotated
# nTEST: rotated.t1
# export PATH=../tools:$PATH
# rm -rf diag.txt h5 bop ply
# x=0.75 y=8 z=12   c=0.5 s=0.866 # sin() and cos()
# echo     1  0   0 $x              \
#          0 $c -$s $y              \
#          0 $s  $c $z              \
#          0  0   0  1 > rbcs-ic.txt
# :
# argp .conf.couette.h -rbcs -tend=0.5 -part_freq=300 > .conf.h
# :
# { make clean && make -j && make -C ../tools; } > /dev/null
# ./udx
# ply2punto ply/rbcs-00003.ply | uscale 100 > ply.out.txt

#### RBC with wall
# sTEST: diag.t3
# export PATH=../tools:$PATH
# cp sdf/wall1/wall.dat                               sdf.dat
# x=0.75 y=8 z=12; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 > rbcs-ic.txt
# rm -rf diag.txt h5 bop ply
# :
# argp .conf.couette.h                                         \
#   -rbcs -tend=0.5 -part_freq=300  -walls  -wall_creation=100 \
#   -field_dumps -part_dumps -field_freq=300 -shear_z > .conf.h
# :
# { make clean && make -j && make -C ../tools; } > /dev/null
# ./udx
# ply2punto ply/rbcs-00003.ply | uscale 100 > ply.out.txt

#### one RBC around cylinder
# sTEST: flow.around.t1
# export PATH=../tools:$PATH
# rm -rf diag.txt h5 o ply
# cp sdf/cyl1/cyl.dat sdf.dat
# x=0.75 y=8 z=9; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 > rbcs-ic.txt
# :
# argp .conf.around.h -kBT=1e-7 \
#    -rbcs -tend=3.0 -part_freq=5000 -walls -wall_creation=1000 \
#    -field_dumps -part_dumps -field_freq=5000 -pushflow > .conf.h
# :
# { make clean && make -j && make -C ../tools; } > /dev/null
# ./udx
# ply2punto ply/rbcs-00001.ply | uscale 100 > ply.out.txt

#### two RBCs around cylinder
# sTEST: flow.around.t2
# export PATH=../tools:$PATH
# rm -rf diag.txt h5 bop ply
# x=0.75  y=3 z=9; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 >  rbcs-ic.txt
# x=0.75 y=13 z=9; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 >> rbcs-ic.txt
# cp sdf/cyl1/cyl.dat sdf.dat
# :
# argp .conf.around.h  -rbcs -tend=3.0 -part_freq=5000 \
#        -walls -wall_creation=1000 -kBT=1e-7 \
#        -field_dumps -part_dumps  \
#        -field_freq=5000 -pushflow > .conf.h
# :
# { make clean && make -j && make -C ../tools; } > /dev/null
# ./udx
# ply2punto ply/rbcs-00001.ply | uscale 10 > ply.out.txt

#### two RBCs around cylinder with one RBC removed by the wall
# sTEST: flow.around.t3
# export PATH=../tools:$PATH
# rm -rf diag.txt h5 bop ply
# cp sdf/cyl1/cyl.dat sdf.dat
# x=0.75 y=3 z=9; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 >  rbcs-ic.txt
# x=8    y=8 z=8; echo 1 0 0 $x  0 1 0 $y  0 0 1 $z  0 0 0 1 >> rbcs-ic.txt
# :
# argp .conf.around.h -rbcs -tend=3.0 -part_freq=5000 \
#     -walls -wall_creation=1000 -kBT=1e-7 \
#     -field_dumps -part_dumps  \
#     -field_freq=5000 -pushflow > .conf.h
# :
# { make clean && make -j && make -C ../tools; } > /dev/null
# ./udx
# ply2punto ply/rbcs-00001.ply | uscale 100 > ply.out.txt
