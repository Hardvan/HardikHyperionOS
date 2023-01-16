#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"

uint16_t *video_memory = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

// Takes a character and a color and returns a 16-bit value
uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

// Takes a character and a color and puts it on the screen
void terminal_putchar(int x, int y, char c, char color)
{
    video_memory[y * VGA_WIDTH + x] = terminal_make_char(c, color);
}

// Takes a character and a color and puts it on the screen
void terminal_writechar(char c, char color)
{
    if (c == '\n')
    {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row++;
    }
}

// Clears the screen
void terminal_initialize()
{
    video_memory = (uint16_t *)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

// Returns the length of a string
size_t strlen(const char *str)
{
    size_t len = 0;

    while (str[len])
    {
        len++;
    }

    return len;
}

// Prints a string
void print(const char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

static struct paging_4gb_chunk *kernel_chunk = 0;

// The kernel main function
void kernel_main()
{
    terminal_initialize();

    print("Hello, World!\nThis is written in C!");

    // Initialize the heap
    kheap_init();

    // Initialize the IDT
    idt_init();

    // Set up paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    char *ptr = kzalloc(4096);
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void *)0x1000,
               (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    // Enable paging
    enable_paging();

    char *ptr2 = (char *)0x1000;
    ptr2[0] = 'A';
    ptr2[1] = 'B';
    print(ptr2);

    print(ptr);

    // Enable interrupts
    enable_interrupts();
}
