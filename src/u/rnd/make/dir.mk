D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/comm  && \
    d $B/d     && \
    d $B/frag  && \
    d $B/mpi   && \
    d $B/rnd   && \
    d $B/u/rnd && \
    d $B/utils
