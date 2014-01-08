#ifndef QSORT_H
#define QSORT_H

//#include <cruntime.h>
#include <stdlib.h>
#include <search.h>
//#include <internal.h>

void __cdecl qsort (
    void *base,
    size_t num,
    size_t width,
    int (__cdecl *comp)(const void *, const void *)
    );

#endif // QSORT_H
