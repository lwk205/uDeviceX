D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/conf             && \
    d $B/coords           && \
    d $B/d                && \
    d $B/io/grid          && \
    d $B/io/grid/h5       && \
    d $B/io/grid/xmf      && \
    d $B/math/linal       && \
    d $B/math/rnd         && \
    d $B/math/tform       && \
    d $B/math/tri         && \
    d $B/mpi              && \
    d $B/u/sdf            && \
    d $B/utils            && \
    d $B/utils/nvtx       && \
    d $B/utils/string     && \
    d $B/wall/sdf         && \
    d $B/wall/sdf/array3d && \
    d $B/wall/sdf/bounce  && \
    d $B/wall/sdf/field   && \
    d $B/wall/sdf/label   && \
    d $B/wall/sdf/tex3d   && \
    d $B/wall/sdf/tform   && \
    d $B/wall/wvel       
