D = @d () { test -d "$$1" || mkdir -p -- "$$1"; } && \
    d $B/d                     && \
    d $B/io                    && \
    d $B/io/bop                && \
    d $B/io/field              && \
    d $B/io/field/h5           && \
    d $B/io/field/xmf          && \
    d $B/math                  && \
    d $B/mpi                   && \
    d $B/rbc/adj               && \
    d $B/rbc/com               && \
    d $B/rbc/edg               && \
    d $B/rbc/force             && \
    d $B/rbc/force/area_volume && \
    d $B/rbc/gen               && \
    d $B/rbc/main              && \
    d $B/rbc/rnd               && \
    d $B/rbc/rnd/api           && \
    d $B/rbc/stretch           && \
    d $B/u/rbc/force           && \
    d $B/u/rbc/force/lib       && \
    d $B/utils                
