#ifndef HARDIKHYPERIONOS_H
#define HARDIKHYPERIONOS_H

#include <stddef.h>

void print(const char *filename);
int getkey();
void *hardikhyperionos_malloc(size_t size);
void hardikhyperionos_free(void *ptr);

#endif
