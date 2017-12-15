#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "utils/error.h"
#include "utils/imp.h"
#include "msg.h"

#include "imp.h"

enum {
    ARG, /* from arguments       */
    OPT, /* from additional file */
    DEF, /* from default file    */
    NCFG
};

// tag::struct[]
struct Config {
    config_t c[NCFG];
};
// end::struct[]

void conf_ini(/**/ Config **c) {
    UC(emalloc(sizeof(Config), (void**) c));

    Config *cfg = *c;
    for (int i = 0; i < NCFG; ++i)
        config_init(cfg->c + i);
}

void conf_destroy(/**/ Config *c) {
    for (int i = 0; i < NCFG; ++i)
        config_destroy(c->c + i);
    UC(efree(c));
}

static void concatenate(int n, char **ss, /**/ char *a) {
    char *s;
    a[0] = '\0';

    for(int i = 0; i < n; ++i) {
        s = ss[i];
        strcat(a, s);
        strcat(a, " ");
    }
}

static void read_file(const char *fname, /**/ config_t *c) {
    MSG("read config from <%s>", fname);
    if (!config_read_file(c, fname))
        ERR( "%s:%d - %s\n", config_error_file(c),
             config_error_line(c), config_error_text(c));
}

static void read_args(int argc, char **argv, /**/ config_t *c) {
   enum {MAX_CHAR = 100000};
    char *args;
    
    UC(emalloc(MAX_CHAR * sizeof(char), (void **) &args));

    concatenate(argc, argv, /**/ args);

    if (!config_read_string(c, args))
        ERR( "arguments: %d - %s\n",
             config_error_line(c), config_error_text(c));
    
    delete[] args;
}

static void shift(int *c, char ***v) {
    (*c) --;
    (*v) ++;
}

static int get_opt_file(int *argc, char ***argv, /**/ char fname[]) {
    char *lastpnt, *a;
    int differ;

    if (*argc) {
        a = (*argv)[0];
        lastpnt = strrchr(a, '.');
        
        if (lastpnt != NULL) {
            differ = strcmp(lastpnt, ".cfg");
            if (differ) return 0;
            strcpy(fname, a);
            shift(argc, argv);
            return 1;
        }
    }
    return 0;
}

void conf_read(int argc, char **argv, /**/ Config *cfg) {
    char *home, defname[1024] = {0}, optname[1024];
    home = getenv("HOME");

    strcpy(defname, home);
    strcat(defname, "/.udx/default.cfg");

    UC(read_file(defname, /**/ &cfg->c[DEF])); 
    
    if (get_opt_file(&argc, &argv, /**/ optname)) {
        UC(read_file(optname, /**/ &cfg->c[OPT]));
    }

    if (argc)
        UC(read_args(argc, argv, /**/ &cfg->c[ARG]));
}

static bool found(int s) {return s == CONFIG_TRUE;}

void conf_lookup_int(const Config *c, const char *desc, int *a) {
    int i, s;
    for (i = 0; i < NCFG; ++i) {
        s = config_lookup_int(c->c + i, desc, /**/ a);
        if ( found(s) ) return;
    }
    ERR("Could not find the field <%s>\n", desc);
}

void conf_lookup_float(const Config *c, const char *desc, float *a) {
    int i, s;
    double d;

    for (i = 0; i < NCFG; ++i) {
        s = config_lookup_float(c->c + i, desc, /**/ &d);
        *a = d;
        if ( found(s) ) return;
    }
    ERR("Could not find the field <%s>\n", desc);
}

void conf_lookup_bool(const Config *c, const char *desc, int *a) {
    int i, s;
    for (i = 0; i < NCFG; ++i) {
        s = config_lookup_bool(c->c + i, desc, a);
        if ( found(s) ) return;
    }
    ERR("Could not find the field <%s>\n", desc);
}

void conf_lookup_string(const Config *c, const char *desc, const char **a) {
    int i, s;
    for (i = 0; i < NCFG; ++i) {
        s = config_lookup_string(c->c + i, desc, a);
        if ( found(s) ) return;
    }
    ERR("Could not find the field <%s>\n", desc);
}
