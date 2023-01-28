/*
    ? terminal_make_char:
    * takes a character and a color and returns a 16-bit value.

    ? terminal_putchar:
    * takes a character, color, and x, y coordinates and puts it on the screen.

    ? terminal_backspace:
    * removes the last character that was printed on the screen.

    ? terminal_writechar:
    * takes a character and a color and writes it on the screen,
    * keeps track of the current row and column

    ?terminal_initialize:
    * clears the screen

    ? print:
    * takes a string as an input and prints it on the screen

    ? panic:
    * takes a message as an input and prints it on the screen, and the program enters an infinite loop

    ? kernel_page:
    * sets up the paging for kernel

    ? kernel_main:
    * the kernel main function,
    * it initializes the terminal,
    * prints a message on the screen,
    * waits for a bit,
    * initializes the GDT, loads the GDT,
    * initializes the heap, initializes the filesystems,
    * searches and initializes the disks,
    * initializes the IDT, sets up the task switching,
    * and enters the user mode.
*/

#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "status.h"
#include "task/tss.h"
#include "task/task.h"
#include "task/process.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"

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

void terminal_backspace()
{
    if (terminal_row == 0 && terminal_col == 0)
    {
        return;
    }

    if (terminal_col == 0)
    {
        terminal_row -= 1;
        terminal_col = VGA_WIDTH;
    }

    terminal_col -= 1;
    terminal_writechar(' ', 15);
    terminal_col -= 1;
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

    if (c == 0x08)
    {
        terminal_backspace();
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

void panic(const char *msg)
{
    print(msg);
    while (1)
    {
    }
}

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;
struct gdt gdt_real[HARDIKHYPERIONOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[HARDIKHYPERIONOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                 // Null Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},           // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},           // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},           // User data segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9} // TSS segment
};

// The kernel main function
void kernel_main()
{
    // Initialize the terminal
    terminal_initialize();

    // ? Testing the terminal
    print("HardikHyperionOS v0.0.1\n\n");
    print("Testing the terminal:\n");
    print("Hello, World!\nThis is written in C!\n\n");

    // Wait for a bit
    for (int i = 0; i < 100000; i++)
    {
    }

    // Initialize the GDT
    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, HARDIKHYPERIONOS_TOTAL_GDT_SEGMENTS);

    // Load the GDT
    gdt_load(gdt_real, sizeof(gdt_real));

    // Initialize the heap
    kheap_init();

    // Initialize the filesystems
    fs_init();

    // Search and initialize the disks
    disk_search_and_init();

    // Initialize the IDT
    idt_init();

    // Set up the TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28);

    // Set up paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(kernel_chunk);

    // Enable paging
    enable_paging();

    // Register the kernel commands
    isr80h_register_commands();

    // Initialize all the system keyboards
    keyboard_init();

    // ? Open hello.txt using fopen, fread
    print("Testing fopen and fread:\n");
    int fd = fopen("0:/hello.txt", "r");
    if (fd)
    {
        print("hello.txt opened successfully!\n");

        print("Displaying contents of hello.txt:\n");
        char buf[36];
        fread(buf, 35, 1, fd);
        buf[35] = 0x00;
        print(buf);
    }
    print("\n\n");

    // ? Injecting arguments and showcasing multithreading
    // ? The first argument is "Testing!" and the second is "Abc!"
    // ? blank.elf is common to both processes
    // Load the first process
    struct process *process = 0;
    int res = process_load_switch("0:/blank.elf", &process);
    if (res != HARDIKHYPERIONOS_ALL_OK)
    {
        panic("Failed to load blank.elf\n");
    }

    // First Argument ("Testing!") is injected into the first process
    struct command_argument argument;
    strcpy(argument.argument, "Testing!");
    argument.next = 0x00;
    process_inject_arguments(process, &argument);

    // Load the second process
    res = process_load_switch("0:/blank.elf", &process);
    if (res != HARDIKHYPERIONOS_ALL_OK)
    {
        panic("Failed to load blank.elf\n");
    }

    // Second Argument ("Abc!") is injected into the second process
    strcpy(argument.argument, "Abc!");
    argument.next = 0x00;
    process_inject_arguments(process, &argument);

    task_run_first_ever_task();

    while (1)
    {
    }
}
