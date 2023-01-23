#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define HARDIKHYPERIONOS_MAX_PATH 108

void kernel_main();
void print(const char *str);
void panic(const char *msg);

#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define IS_ERR(value) ((int)value < 0)

#endif