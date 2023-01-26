#include "string.h"

char tolower(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c + 32;

    return c;
}

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

int strnlen_terminator(const char *ptr, int maxlen, char terminator)
{
    int i = 0;
    for (i = 0; i < maxlen; i++)
    {
        if (ptr[i] == '\0' || ptr[i] == terminator)
        {
            break;
        }
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

char *strncpy(char *dest, const char *src, int count)
{
    int i;
    for (i = 0; i < count - 1; i++)
    {
        if (src[i] == 0x00)
            break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;

    return dest;
}

int strncmp(const char *str1, const char *str2, int n)
{
    unsigned char u1, u2;
    while (n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
        {
            return u1 - u2;
        }
        if (u1 == '\0')
        {
            return 0;
        }
    }

    return 0;
}

int istrncmp(const char *s1, const char *s2, int n)
{
    unsigned char u1, u2;
    while (n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2))
        {
            return u1 - u2;
        }
        if (u1 == '\0')
        {
            return 0;
        }
    }

    return 0;
}
