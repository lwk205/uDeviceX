D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/algo/edg              && \
    d $B/algo/force_stat       && \
    d $B/algo/kahan_sum        && \
    d $B/algo/key_list         && \
    d $B/algo/minmax           && \
    d $B/algo/scan             && \
    d $B/clist                 && \
    d $B/cnt                   && \
    d $B/color                 && \
    d $B/comm                  && \
    d $B/conf                  && \
    d $B/control/den           && \
    d $B/control/inflow        && \
    d $B/control/outflow       && \
    d $B/control/vel           && \
    d $B/coords                && \
    d $B/d                     && \
    d $B/dbg                   && \
    d $B/distr/common          && \
    d $B/distr/flu             && \
    d $B/distr/flu/status      && \
    d $B/distr/map             && \
    d $B/distr/rbc             && \
    d $B/distr/rig             && \
    d $B/exch/common           && \
    d $B/exch/flu              && \
    d $B/exch/map              && \
    d $B/exch/mesh             && \
    d $B/exch/obj              && \
    d $B/farray                && \
    d $B/flu                   && \
    d $B/fluforces             && \
    d $B/fluforces/bulk        && \
    d $B/fluforces/halo        && \
    d $B/frag                  && \
    d $B/fsi                   && \
    d $B/generate/rig          && \
    d $B/inter                 && \
    d $B/inter/color           && \
    d $B/io/bop                && \
    d $B/io/com                && \
    d $B/io/diag/mesh          && \
    d $B/io/diag/part          && \
    d $B/io/field              && \
    d $B/io/field/h5           && \
    d $B/io/field/xmf          && \
    d $B/io/mesh               && \
    d $B/io/mesh/write         && \
    d $B/io/mesh_read          && \
    d $B/io/point              && \
    d $B/io/restart            && \
    d $B/io/rig                && \
    d $B/io/txt                && \
    d $B/io/vtk                && \
    d $B/math/linal            && \
    d $B/math/rnd              && \
    d $B/math/tform            && \
    d $B/math/tri              && \
    d $B/mesh/angle            && \
    d $B/mesh/angle/edg        && \
    d $B/mesh/area             && \
    d $B/mesh/bbox             && \
    d $B/mesh/collision        && \
    d $B/mesh/dist             && \
    d $B/mesh/props            && \
    d $B/mesh/tri_area         && \
    d $B/mesh/triangles        && \
    d $B/mesh/vectors          && \
    d $B/mesh/volume           && \
    d $B/meshbb                && \
    d $B/mpi                   && \
    d $B/pair                  && \
    d $B/parray                && \
    d $B/rbc                   && \
    d $B/rbc/adj               && \
    d $B/rbc/com               && \
    d $B/rbc/force             && \
    d $B/rbc/force/area_volume && \
    d $B/rbc/force/rnd         && \
    d $B/rbc/force/rnd/api     && \
    d $B/rbc/gen               && \
    d $B/rbc/matrices          && \
    d $B/rbc/params            && \
    d $B/rbc/shape             && \
    d $B/rbc/stretch           && \
    d $B/rig                   && \
    d $B/rigid                 && \
    d $B/scheme/force          && \
    d $B/scheme/move           && \
    d $B/scheme/restrain       && \
    d $B/scheme/time           && \
    d $B/scheme/time_step      && \
    d $B/sim                   && \
    d $B/utils                 && \
    d $B/utils/string          && \
    d $B/wall                  && \
    d $B/wall/exch             && \
    d $B/wall/force            && \
    d $B/wall/sdf              && \
    d $B/wall/sdf/array3d      && \
    d $B/wall/sdf/bounce       && \
    d $B/wall/sdf/field        && \
    d $B/wall/sdf/label        && \
    d $B/wall/sdf/tex3d        && \
    d $B/wall/sdf/tform        && \
    d $B/wall/wvel            
