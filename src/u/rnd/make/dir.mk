D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/comm         && \
    d $B/d            && \
    d $B/frag         && \
    d $B/math/rnd     && \
    d $B/mpi          && \
    d $B/u/rnd        && \
    d $B/utils        && \
    d $B/utils/string
