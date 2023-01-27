#include "string.h"

// Convert a character to lower case
char tolower(char s1)
{
    if (s1 >= 65 && s1 <= 90)
    {
        s1 += 32;
    }

    return s1;
}

// Return the length of a string
int strlen(const char *ptr)
{
    int i = 0;
    while (*ptr != 0)
    {
        i++;
        ptr += 1;
    }

    return i;
}

// Return the length of a string with a maximum length
int strnlen(const char *ptr, int max)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (ptr[i] == 0)
            break;
    }

    return i;
}

// Return the length of a string with a maximum length and a terminator
int strnlen_terminator(const char *str, int max, char terminator)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
            break;
    }

    return i;
}

// Compare two strings upto a maximum length (case insensitive)
int istrncmp(const char *s1, const char *s2, int n)
{
    unsigned char u1, u2;
    while (n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2))
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

// Compare two strings
int strncmp(const char *str1, const char *str2, int n)
{
    unsigned char u1, u2;

    while (n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

// Copy a string
char *strcpy(char *dest, const char *src)
{
    char *res = dest;
    while (*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}

// Copy a string with a maximum length
char *strncpy(char *dest, const char *src, int count)
{
    int i = 0;
    for (i = 0; i < count - 1; i++)
    {
        if (src[i] == 0x00)
            break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
}

// Returns true if c is a digit
bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

// Converts a character to a numeric digit
int tonumericdigit(char c)
{
    return c - 48;
}

// Like .split() in Python
char *sp = 0;
char *strtok(char *str, const char *delimiters)
{
    int i = 0;
    int len = strlen(delimiters);
    if (!str && !sp)
        return 0;

    if (str && !sp)
    {
        sp = str;
    }

    char *p_start = sp;
    while (1)
    {
        for (i = 0; i < len; i++)
        {
            if (*p_start == delimiters[i])
            {
                p_start++;
                break;
            }
        }

        if (i == len)
        {
            sp = p_start;
            break;
        }
    }

    if (*sp == '\0')
    {
        sp = 0;
        return sp;
    }

    // Find end of substring
    while (*sp != '\0')
    {
        for (i = 0; i < len; i++)
        {
            if (*sp == delimiters[i])
            {
                *sp = '\0';
                break;
            }
        }

        sp++;
        if (i < len)
            break;
    }

    return p_start;
}