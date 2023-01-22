#include "string.h"

// Returns the length of a string
int strlen(const char *str)
{
    int i = 0;
    while (*str != '\0')
    {
        i++;
        str++;
    }

    return i;
}

// Returns the length of a string only up to maxlen
int strnlen(const char *ptr, int maxlen)
{
    int i = 0;
    while (*ptr != '\0' && i < maxlen)
    {
        i++;
        ptr++;
    }

    return i;
}

// Returns true if c is a digit
bool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

// Returns the numeric value of a digit
int tonumericdigit(char c)
{
    return c - '0';
}

// Copies a string from src to dest
char *strcpy(char *dest, const char *src)
{
    char *res = dest;
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';

    return res;
}
