#!/usr/bin/sh

# Sergey's current run

. $HOME/.udx/u.sh
. ./gx.generic.sh

inc ./gx.HOST.sh

pre() {
    nv=498
    NX=2  NY=2  NZ=1
    XS=40 YS=52 ZS=20
    NN=$((${NX}*${NY}*${NZ}))
    LX=$((${NX}*${XS}))
    LY=$((${NY}*${YS}))
    LZ=$((${NZ}*${ZS}))

    df=1.0
    D="-XS=$XS -YS=$YS -ZS=$ZS"
    
    fraction=0.2 radius=2.3 sc=0.2 ang=0
    plcmt.ro $LX $LY $LZ $radius $fraction $sc $ang ic_solid.txt rbcs-ic.txt

    rm -rf diag.txt h5 bop ply solid-ply solid_diag*txt
    cp sdf/gx/small.rot.dat sdf.dat
    #cp ~/geoms/128.dat sdf.dat
    cp data/cylinder.ply mesh_solid.ply
    cp cells/sph.$nv.off  rbc.off

    #ply.sxyz xs ys zs in.ply > out.ply

    argp .conf.gx.base.h $D                   \
	 RBCnv=$nv                            \
	 rbcs solids contactforces            \
	 tend=3000.0 part_freq=100            \
	 walls wall_creation=1                \
	 pushflow     driving_force=$df       \
	 field_dumps part_dumps field_freq=100 > .conf.h
}

setup
pre
ini
compile
run
