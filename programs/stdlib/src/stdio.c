/*
    ? putchar:
    * Used to display a character on the screen.

    ? printf:
    * Used to print a formatted string.
*/

#include "stdio.h"
#include "hardikhyperionos.h"
#include "stdlib.h"
#include <stdarg.h>

// Display a character on the screen
int putchar(int c)
{
    hardikhyperionos_putchar((char)c);

    return 0;
}

// Print a formatted string
int printf(const char *fmt, ...)
{
    // va: Variable argument
    va_list ap;    // Argument pointer
    const char *p; // Pointer to the format string
    char *sval;    // String value
    int ival;      // Integer value

    va_start(ap, fmt); // Initialize the argument pointer
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }

        switch (*++p)
        {
        case 'i':
            ival = va_arg(ap, int);
            print(itoa(ival));
            break;

        case 's':
            sval = va_arg(ap, char *);
            print(sval);
            break;

        default:
            putchar(*p);
            break;
        }
    }

    va_end(ap);

    return 0;
}
