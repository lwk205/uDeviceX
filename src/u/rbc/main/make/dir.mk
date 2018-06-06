D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/algo/edg              && \
    d $B/algo/vectors          && \
    d $B/conf                  && \
    d $B/coords                && \
    d $B/d                     && \
    d $B/io/diag/part          && \
    d $B/io/mesh               && \
    d $B/io/mesh_read          && \
    d $B/io/mesh_read/edg      && \
    d $B/io/restart            && \
    d $B/io/write              && \
    d $B/math/linal            && \
    d $B/math/rnd              && \
    d $B/math/tform            && \
    d $B/math/tri              && \
    d $B/mesh/force/kantor0    && \
    d $B/mesh/force/kantor1    && \
    d $B/mesh/gen              && \
    d $B/mesh/gen/matrices     && \
    d $B/mpi                   && \
    d $B/rbc                   && \
    d $B/rbc/adj               && \
    d $B/rbc/com               && \
    d $B/rbc/force             && \
    d $B/rbc/force/area_volume && \
    d $B/rbc/force/rnd         && \
    d $B/rbc/force/rnd/api     && \
    d $B/rbc/params            && \
    d $B/rbc/shape             && \
    d $B/rbc/stretch           && \
    d $B/scheme/force          && \
    d $B/scheme/move           && \
    d $B/scheme/restrain       && \
    d $B/scheme/time_line      && \
    d $B/u/rbc/main            && \
    d $B/u/rbc/main/lib        && \
    d $B/utils                 && \
    d $B/utils/convert         && \
    d $B/utils/nvtx            && \
    d $B/utils/string         
