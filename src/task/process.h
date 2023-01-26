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

    struct keyboard_buffer
    {
        char buffer[HARDIKHYPERIONOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_load(const char *filename, struct process **process);
int process_load_for_slot(const char *filename, struct process **process, int process_slot);
struct process *process_current();
struct process *process_get(int process_id);

#endif
