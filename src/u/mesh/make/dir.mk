D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/algo/minmax && \
    d $B/algo/scan   && \
    d $B/d           && \
    d $B/glb         && \
    d $B/glb/wvel    && \
    d $B/io          && \
    d $B/mesh        && \
    d $B/mpi         && \
    d $B/u/mesh      && \
    d $B/utils      
