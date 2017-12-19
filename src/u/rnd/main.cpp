#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "msg.h"
#include "rnd/imp.h"
#include "mpi/glb.h"
#include "utils/error.h"

static void shift(int *argc, char ***argv) {
    (*argc)--;
    (*argv)++;
}

static void assert_n(int c) {
    if (c > 0) return;
    ERR("not enough args");
}

void main0(int n) {
    int i;
    rnd::KISS r(42, 42, 42, 42);
    for (i = 0; i < n; i++) {
        printf("%10.6e\n", r.get_float());
    }
}

void main1(int c, char **v) {
    int n;
    UC(assert_n(c)); n = atoi(v[0]);
    if (n == 0) ERR("expecting an a positive number");
    main0(n);
}

int main(int argc, char **argv) {
    m::ini(&argc, &argv);
    UC(main1(argc, argv));
    m::fin();
}
