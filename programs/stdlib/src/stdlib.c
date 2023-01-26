#include "stdlib.h"
#include "hardikhyperionos.h"

void *malloc(size_t size)
{
    return hardikhyperionos_malloc(size);
}

void free(void *ptr)
{
    hardikhyperionos_free(ptr);
}
