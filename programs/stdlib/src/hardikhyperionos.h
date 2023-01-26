#ifndef HARDIKHYPERIONOS_H
#define HARDIKHYPERIONOS_H

#include <stddef.h>
#include <stdbool.h>

struct command_argument
{
    char argument[512];
    struct command_argument *next;
};

void print(const char *filename);
int hardikhyperionos_getkey();

void *hardikhyperionos_malloc(size_t size);
void hardikhyperionos_free(void *ptr);
void hardikhyperionos_putchar(char c);
int hardikhyperionos_getkeyblock();
void hardikhyperionos_terminal_readline(char *out, int max, bool output_while_typing);
void hardikhyperionos_process_load_start(const char *filename);
struct command_argument *hardikhyperionos_parse_command(const char *command, int max);

#endif
