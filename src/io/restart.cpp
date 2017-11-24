#include <stdio.h>

#include <conf.h>
#include "inc/conf.h"

#include "inc/type.h"
#include "inc/def.h"
#include "msg.h"
#include "mpi/glb.h"
#include "utils/error.h"

#include "restart.h"

//#define DBG(...) MSG(__VA_ARGS__)
#define DBG(...) 

namespace restart {
enum {X, Y, Z};

/* pattern : 
   sing processor  : base/code/id.ext
   mult processors : base/code/XXX.YYY.ZZZ/id.ext
   base depends on read/write
 */
#define PF    "%s.%s"
#define DIR_S "%s/%s/"                 PF
#define DIR_M "%s/%s/%03d.%03d.%03d/"  PF

#define READ (true)
#define DUMP (false)

// buff size
#define BS (256)

// check fprintf (BS-1 for \0 character)
#define CSPR(a) do {                                        \
        int check = a;                                      \
        if (check < 0 || check >= BS-1)                     \
        ERR("Buffer too small to handle this format\n");    \
    } while (0)

#define CF(f, fname) do {if (f==NULL) ERR("could not open the file <%s>\n", fname);} while(0)

void id2str(const int id, char *str) {
    switch (id) {
    case TEMPL:
        CSPR(sprintf(str, "templ"));
        break;
    case FINAL:
        CSPR(sprintf(str, "final"));
        break;
    default:
        CSPR(sprintf(str, "%05d", id));
        break;
    }
}

void gen_name(const bool read, const char *code, const int id, const char *ext, /**/ char *name) {
    char idcode[BS] = {0};
    id2str(id, /**/ idcode);
    
    if (m::size == 1)
        CSPR(sprintf(name, DIR_S, read ? BASE_STRT_READ : BASE_STRT_DUMP, code, idcode, ext));
    else
        CSPR(sprintf(name, DIR_M, read ? BASE_STRT_READ : BASE_STRT_DUMP, code, m::coords[X], m::coords[Y], m::coords[Z], idcode, ext));
}

namespace bopwrite {
void header_pp(const char *bop, const char *rel, const long n) {
    FILE *f = fopen(bop, "w"); CF(f, bop); /* TODO */
    
    fprintf(f, "%ld\n", n);
    fprintf(f, "DATA_FILE: %s\n", rel);
    fprintf(f, "DATA_FORMAT: float\n");
    fprintf(f, "VARIABLES: x y z vx vy vz\n");
    fclose(f);
}

void header_ii(const char *bop, const char *rel, const long n) {
    FILE *f = fopen(bop, "w"); CF(f, bop);
    
    fprintf(f, "%ld\n", n);
    fprintf(f, "DATA_FILE: %s\n", rel);
    fprintf(f, "DATA_FORMAT: int\n");
    fprintf(f, "VARIABLES: id\n");
    fclose(f);
}

template <typename T>
void data(const char *val, const T *dat, const long n) {
    FILE *f = fopen(val, "w"); CF(f, val);
    fwrite(dat, sizeof(T), n, f);
    fclose(f);
}
} // namespace bopwrite

namespace bopread {
void read_n(const char *name, long *n) {
    FILE *f = fopen(name, "r"); CF(f, name);
    if (fscanf(f, "%ld\n", n) != 1) ERR("wrong format\n");
    fclose(f);
}

template <typename T>
void data(const char *name, const long n, T *dat) {
    FILE *f = fopen(name, "r"); CF(f, name);
    fread(dat, sizeof(T), n, f);
    fclose(f);
}
} // namespace bopread

void write_pp(const char *code, const int id, const Particle *pp, const long n) {
    char bop[BS] = {0}, rel[BS] = {0}, val[BS] = {0}, idcode[BS] = {0};
    gen_name(DUMP, code, id, "bop"   , /**/ bop);
    gen_name(DUMP, code, id, "values", /**/ val);

    id2str(id, /**/ idcode);
    CSPR(sprintf(rel, PF, idcode, "values"));    

    bopwrite::header_pp(bop, rel, n);
    bopwrite::data(val, pp, n);
}

void read_pp(const char *code, const int id, Particle *pp, int *n) {
    long np = 0;
    char bop[BS] = {0}, val[BS] = {0};
    gen_name(READ, code, id, "bop"   , /**/ bop);
    gen_name(READ, code, id, "values", /**/ val);
    MSG("reading <%s> and <%s>", bop, val);
    bopread::read_n(bop, &np);
    bopread::data(val, np, pp);
    *n = np;
    DBG("I have read %ld pp", np);
}

void write_ii(const char *code, const char *subext, const int id, const int *ii, const long n) {
    char bop[BS] = {0}, rel[BS] = {0}, val[BS] = {0}, idcode[BS] = {0},
        extbop[BS] = {0}, extval[BS] = {0};
    CSPR(sprintf(extbop, "%s.bop",    subext));
    CSPR(sprintf(extval, "%s.values", subext));
    
    gen_name(DUMP, code, id, extbop, /**/ bop);
    gen_name(DUMP, code, id, extval, /**/ val);

    id2str(id, /**/ idcode);
    CSPR(sprintf(rel, PF, idcode, "id.values"));    

    bopwrite::header_ii(bop, rel, n);
    bopwrite::data(val, ii, n);
}

void read_ii(const char *code, const char *subext, const int id, int *ii, int *n) {
    long np = 0;
    char bop[BS] = {0}, val[BS] = {0}, extbop[BS] = {0}, extval[BS] = {0};
    CSPR(sprintf(extbop, "%s.bop",    subext));
    CSPR(sprintf(extval, "%s.values", subext));
    
    gen_name(READ, code, id, extbop, /**/ bop);
    gen_name(READ, code, id, extval, /**/ val);
    DBG("reading <%s> and <%s>", bop, val);
    bopread::read_n(bop, &np);
    bopread::data(val, np, ii);
    *n = np;
    DBG("I have read %ld pp", np);
}

void write_ss(const char *code, const int id, const Solid *ss, const long n) {
    char fname[BS] = {0};
    gen_name(DUMP, code, id, "solid", /**/ fname);
        
    FILE *f = fopen(fname, "w"); CF(f, fname);
    fprintf(f, "%ld\n", n);
    fwrite(ss, sizeof(Solid), n, f);
    fclose(f);
}

void read_ss(const char *code, const int id, Solid *ss, int *n) {
    long ns = 0;
    char fname[BS] = {0};
    gen_name(READ, code, id, "solid", /**/ fname);
    fprintf(stderr, "reading %s\n", fname);
    FILE *f = fopen(fname, "r"); CF(f, fname);
    fscanf(f, "%ld\n", &ns);
    fread(ss, sizeof(Solid), ns, f);
    fclose(f);
    *n = ns;
    DBG("I have read %ld ss.", ns);
}

#undef PF
#undef DIR_S
#undef DIR_M

#undef READ
#undef DUMP

#undef DBG
} // namespace restart
