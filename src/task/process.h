#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "config.h"
#include "task.h"

struct process
{
    // Process ID
    uint16_t pid;

    char filename[HARDIKHYPERIONOS_MAX_PATH];

    // The main process task
    struct task *task;

    // The memory (malloc) allocatins
    void *allocations[HARDIKHYPERIONOS_MAX_PROGRAM_ALLOCATIONS];

    // Physical Pointer to the process memory
    void *ptr;

    // Physical Pointer to the stack memory
    void *stack;

    // Size of the data pointed to by ptr
    uint32_t size;
};

int process_load(const char *filename, struct process **process);
int process_load_for_slot(const char *filename, struct process **process, int process_slot);

#endif
