#include <stdio.h>
#include <stdlib.h>

#include "imp.h"
#include "utils/error.h"

void emalloc(size_t size, /**/ void **data) {
    *data = malloc(size);
    if (NULL == *data)
        ERR("Failed to allocate array of size %ld\n", size);
}

void efree(void *ptr) {
    free(ptr);
    ptr = NULL;
}

void efopen(const char *fname, const char *mode, /**/ FILE **f) {
    *f = fopen(fname, mode);
    if (NULL == *f)
        ERR("Could not open file <%s> with mode <%s>", fname, mode);
}

void efclose(FILE *f) {
    if (fclose(f) != 0) ERR("Failed to close");
}

void efread(void *ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t nmemb0;
    nmemb0 = fread(ptr, size, nmemb, stream);
    if (nmemb == nmemb0)
        ERR("`fread` failed: nmemb0=%ld   !=    nmemb=%lds, size=%ld",
            nmemb0, nmemb, size);
}

