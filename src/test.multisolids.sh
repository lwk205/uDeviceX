#### spheres in double poiseuille, no contact
# nTEST: multisolid.t1
# set -x
# export PATH=../tools:$PATH
# rm -rf bop h5 diag.txt solid_diag.txt
# echo -e "4.1 21.5 8\n4.1 10.5 8\n11.9 16 8" > ic_solid.txt
# :
# argp .conf.double.poiseuille.h     \
#   -rbcs -rsph=4 -sbounce_back      \
#   -tend=0.5 -steps_per_dump=100    \
#   -pushtheflow -doublepoiseuille   \
#   -hdf5field_dumps -part_dumps     \
#   -wall_creation_stepid=0          \
#   -contactforces=false             \
#   -steps_per_hdf5dump=100 > .conf.h
# :
# (make clean && make -j && make -C ../tools) > /dev/null
# ./udx
# bop2test.py bop/solid-00004.bop | awk '{print $1, $2}' | uscale 10 > bop.out.txt
#

#### spheres in double poiseuille, contact
# nTEST: multisolid.t2
# set -x
# export PATH=../tools:$PATH
# rm -rf bop h5 diag.txt solid_diag.txt
# echo -e "4.1 21.5 8\n4.1 10.5 8\n11.9 16 8" > ic_solid.txt
# :
# argp .conf.double.poiseuille.h     \
#   -rbcs -rsph=4 -sbounce_back      \
#   -tend=0.5 -steps_per_dump=100    \
#   -pushtheflow -doublepoiseuille   \
#   -hdf5field_dumps -part_dumps     \
#   -wall_creation_stepid=0          \
#   -contactforces=true              \
#   -steps_per_hdf5dump=100 > .conf.h
# :
# (make clean && make -j && make -C ../tools) > /dev/null
# ./udx
# bop2test.py bop/solid-00004.bop | awk '{print $1, $2}' | uscale 10 > bop.out.txt
#

#### ellipsoids in double poiseuille, contact
# nTEST: multisolid.t3
# set -x
# export PATH=../tools:$PATH
# rm -rf h5 diag.txt solid_diag.txt
# echo -e "4.1 21.5 8\n4.1 10.5 8\n11.9 16 8" > ic_solid.txt
# :
# argp .conf.double.poiseuille.h     \
#   -rbcs -sbounce_back              \
#   -a2_ellipsoid=16 -b2_ellipsoid=4 \
#   -c2_ellipsoid=10                 \
#   -tend=0.5 -steps_per_dump=100    \
#   -pushtheflow -doublepoiseuille   \
#   -hdf5field_dumps -part_dumps     \
#   -wall_creation_stepid=0          \
#   -contactforces=true              \
#   -steps_per_hdf5dump=100 > .conf.h
# :
# (make clean && make -j && make -C ../tools) > /dev/null
# ./udx
# bop2test.py bop/solid-00004.bop | awk '{print $1, $2}' | uscale 10 > bop.out.txt
#
