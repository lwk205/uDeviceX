#include <mpi.h>
#include "common.h"
#include <conf.h>
#include "conf.common.h"
#include "m.h"

#include "restart.h"

namespace restart {
enum {X, Y, Z};

/* pattern : 
   sing processor  : base/strt/code/id.ext
   mult processors : base/strt/code/XXX.YYY.ZZZ/id.ext
   base depends on read/write
 */
#define PATTERN0     "%5d.%s"
#define PATTERN_SING "%s/strt/%s/"             PATTERN0
#define PATTERN_MULT "%s/strt/%s/%3d.%3d.%3d/" PATTERN0

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

#define CF(f) do {if (f==NULL) ERR("could not open the file\n");} while(0)

void gen_name(const bool read, const char *code, const int id, const char *ext, /**/ char *name) {
    if (m::d == 1) CSPR(sprintf(name, PATTERN_SING, read ? BASE_STRT_READ : BASE_STRT_DUMP, code, id, ext));
    else           CSPR(sprintf(name, PATTERN_MULT, read ? BASE_STRT_READ : BASE_STRT_DUMP, code, m::coords[X], m::coords[Y], m::coords[Z], id, ext));
}

namespace bopwrite {
void header(const char *bop, const char *rel, const long n) {
    FILE *f = fopen(bop, "w"); CF(f);
    
    fprintf(f, "%ld\n", n);
    fprintf(f, "DATA_FILE: %s\n", rel);
    fprintf(f, "DATA_FORMAT: float\n");
    fprintf(f, "VARIABLES: x y z vx vy vz\n");
    fclose(f);
}

void data(const char *val, const Particle *pp, const long n) {
    FILE *f = fopen(val, "w"); CF(f);
    fwrite((float *) pp, sizeof(float), sizeof(Particle)/sizeof(float) * n, f);
    fclose(f);
}
} // namespace bopwrite

namespace bopread {
void header(const char *name, long *n) {
    FILE *f = fopen(name, "r"); CF(f);
    if (fscanf(f, "%ld\n", n) != 1) ERR("wrong format\n");
    fclose(f);
}

void data(const char *name, const long n, Particle *pp) {
    FILE *f = fopen(name, "r"); CF(f);
    fread(pp, sizeof(Particle), n, f);
    fclose(f);
}
} // namespace bopread

void write_pp(const char *code, const int id, const Particle *pp, const long n) {
    char bop[BS] = {0}, rel[BS] = {0}, val[BS] = {0};
    gen_name(DUMP, code, id, "bop"   , /**/ bop);
    gen_name(DUMP, code, id, "values", /**/ val);

    CSPR(sprintf(rel, PATTERN0, id, "values"));
    
    bopwrite::header(bop, rel, n);
    bopwrite::data(val, pp, n);
}

void read_pp(const char *code, const int id, Particle *pp, int *n) {
    long np = 0;
    char bop[BS] = {0}, val[BS] = {0};
    gen_name(READ, code, id, "bop"   , /**/ bop);
    gen_name(READ, code, id, "values", /**/ val);
    
    bopread::header(bop, &np);
    bopread::data(val, np, pp);
    *n = np;
}

void write_ss(const char *code, const int id, const Solid *ss, const long n) {
    char fname[BS] = {0};
    gen_name(DUMP, code, id, "solid", /**/ fname);
        
    FILE *f = fopen(fname, "r"); CF(f);
    fprintf(f, "%ld\n", n);
    fwrite(ss, sizeof(Solid), n, f);
    fclose(f);
}

void read_ss(const char *code, const int id, Solid *ss, int *n) {
    long ns = 0;
    char fname[BS] = {0};
    gen_name(READ, code, id, "solid", /**/ fname);
    
    FILE *f = fopen(fname, "r"); CF(f);
    fscanf(f, "%ld\n", &ns);
    fread(ss, sizeof(Solid), ns, f);
    fclose(f);

    *n = ns;
}

#undef READ
#undef DUMP

} // namespace restart
