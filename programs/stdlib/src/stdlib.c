/*
    ? itoa:
    * Converts the integer i into a character string.

    ? malloc:
    * Allocate memory.

    ? free:
    * Free memory.
*/

#include "stdlib.h"
#include "hardikhyperionos.h"

// Coverts the integer i into a character string
char *itoa(int i)
{
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;

    if (i >= 0)
    {
        neg = 0;
        i = -i;
    }

    while (i)
    {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11)
        text[--loc] = '0';

    if (neg)
        text[--loc] = '-';

    return &text[loc];
}

// Allocate memory
void *malloc(size_t size)
{
    return hardikhyperionos_malloc(size);
}

// Free memory
void free(void *ptr)
{
    hardikhyperionos_free(ptr);
}
