#include "stdlib.h"
#include "hardikhyperionos.h"

void *malloc(size_t size)
{
    return peachos_malloc(size);
}

void free(void *ptr)
{
}
