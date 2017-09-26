namespace meshbb {

#define make_enum(a) a ,
#define make_str(a) #a ,

#define bbstates(_)                                                 \
    _(BB_SUCCESS)   /* succesfully bounced                       */ \
    _(BB_NOCROSS)   /* did not cross the plane                   */ \
    _(BB_WTRIANGLE) /* [w]rong triangle                          */ \
    _(BB_HFAIL)     /* no time solution h                        */ \
    _(BB_HNEXT)     /* h triangle is not the first to be crossed */
    
enum BBState {
    bbstates(make_enum)
    NBBSTATES
};

static const char *bbstatenames[] = {bbstates(make_str)};

#undef bbstates
#undef make_enum
#undef make_str

} // meshbb
