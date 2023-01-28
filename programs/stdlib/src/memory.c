/*
    ? memset:
    * This function sets a block of memory to a specified value.
    * This function takes three arguments:
        * a pointer to the memory to be set
        * the value to set it to
        * the size of the block of memory.

    ? memcmp:
    * This function compares two blocks of memory,
    * and returns 0 if they are equal,
    * 1 if the first is greater than the second,
    * and -1 if the second is greater than the first.

    ? memcpy:
    * This function copies a block of memory from one location to another.
    * This function takes three arguments:
        * a pointer to the destination memory,
        * a pointer to the source memory,
        * the size of the block of memory to be copied.
*/

#include "memory.h"

// Set memory
void *memset(void *ptr, int c, size_t size)
{
    char *c_ptr = (char *)ptr;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = (char)c;
    }

    return ptr;
}

// Compare memory
int memcmp(void *s1, void *s2, int count)
{
    char *c1 = s1;
    char *c2 = s2;
    while (count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}

// Copy memory
void *memcpy(void *dest, void *src, int len)
{
    char *d = dest;
    char *s = src;
    while (len--)
    {
        *d++ = *s++;
    }

    return dest;
}
