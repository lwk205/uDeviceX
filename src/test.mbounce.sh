#### Double poiseuille non pinned sphere
# nTEST: mbounce.t1
# export PATH=../tools:$PATH
# rm -rf h5 bop diag.txt solid_diag.txt
# echo -e "8 16 8" > ic_solid.txt
# cp data/sphere.ply mesh_solid.ply
# :
# argp .conf.double.poiseuille.h      \
#   -solids -sbounce_back             \
#   -tend=0.5 -steps_per_dump=100     \
#   -pushtheflow -doublepoiseuille    \
#   -hdf5field_dumps -part_dumps      \
#   -wall_creation_stepid=0           \
#   -steps_per_hdf5dump=100 > .conf.h
# :
# (make clean && make -j && make -C ../tools) > /dev/null
# ./udx
# bop2txt bop/solid-00004.bop | awk '{print $1, $2}' | uscale 10 > bop.out.txt
#
