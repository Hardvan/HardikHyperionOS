#include "stdio.h"
#include "hardikhyperionos.h"

int putchar(int c)
{
    hardikhyperionos_putchar((char)c);

    return 0;
}
