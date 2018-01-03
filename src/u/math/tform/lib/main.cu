#include <conf.h>
#include "inc/conf.h"

#include "d/api.h"
#include "inc/dev.h"
#include "utils/cc.h"

#include "math/tform/type.h"
#include "math/tform/dev.h"

#include "main.h"

static __global__ void convert(Tform t, float *a, /**/ float *b) {
    tform_convert_dev(T, a, /**/ b);
}

void convert_dev(Tform *t, float a_hst[3], /**/ float b_hst[3]) {
    enum {dim = 3};
    float *a_dev, *b_dev;
    Dalloc(&a_dev, dim);
    Dalloc(&b_dev, dim);

    cH2D(a_dev, a_hst, dim);
    convert(&T, a_dev, b_dev);
    cD2H(a_hst, a_dev, dim);

    Dfree(a_dev);
    Dfree(b_dev);
}
