D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/algo/scan && \
    d $B/d         && \
    d $B/glb       && \
    d $B/glb/wvel  && \
    d $B/mpi       && \
    d $B/u/scan    && \
    d $B/utils    
