#include "kernel.h"
#include <stdint.h>
#include <stddef.h>

uint16_t *video_memory = 0;

// Takes a character and a color and returns a 16-bit value
uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
    video_memory[y * VGA_WIDTH + x] = terminal_make_char(c, color);
}

void terminal_initialize()
{
    video_memory = (uint16_t *)(0xB8000);
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len])
        len++;

    return len;
}

void kernel_main()
{
    terminal_initialize();
    video_memory[0] = terminal_make_char('A', 15);
}
