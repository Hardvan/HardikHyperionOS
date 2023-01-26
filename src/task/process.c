#include "process.h"
#include "config.h"
#include "memory/memory.h"
#include "status.h"
#include "task/task.h"
#include "memory/heap/kheap.h"
#include "fs/file.h"
#include "string/string.h"
#include "kernel.h"
#include "memory/paging/paging.h"
#include "loader/formats/elfloader.h"

// Current Process that is running
struct process *current_process = 0;

static struct process *processes[HARDIKHYPERIONOS_MAX_PROCESSES] = {};

static void process_init(struct process *process)
{
    memset(process, 0, sizeof(struct process));
}

struct process *process_current()
{
    return current_process;
}

struct process *process_get(int process_id)
{
    if (process_id < 0 || process_id >= HARDIKHYPERIONOS_MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[process_id];
}

int process_switch(struct process *process)
{
    current_process = process;

    return 0;
}

static int process_load_binary(const char *filename, struct process *process)
{
    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd)
    {
        res = -EIO;
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (res != HARDIKHYPERIONOS_ALL_OK)
    {
        goto out;
    }

    void *program_data_ptr = kzalloc(stat.filesize);
    if (!program_data_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1)
    {
        res = -EIO;
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_BINARY;
    process->ptr = program_data_ptr;
    process->size = stat.filesize;

out:
    fclose(fd);
    return res;
}

int process_load_switch(const char *filename, struct process **process)
{
    int res = process_load(filename, process);
    if (res == 0)
    {
        process_switch(*process);
    }

    return res;
}

static int process_load_elf(const char *filename, struct process *process)
{
    int res = 0;
    struct elf_file *elf_file = 0;
    res = elf_load(filename, &elf_file);
    if (IS_ERR(res))
    {
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_ELF;
    process->elf_file = elf_file;
out:
    return res;
}

static int process_load_data(const char *filename, struct process *process)
{
    int res = 0;
    res = process_load_elf(filename, process);
    if (res == -EINFORMAT)
    {
        res = process_load_binary(filename, process);
    }

    return res;
}

int process_map_binary(struct process *process)
{
    int res = 0;
    paging_map_to(process->task->page_directory,
                  (void *)HARDIKHYPERIONOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr,
                  paging_align_address(process->ptr + process->size),
                  PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);

    return res;
}

static int process_map_elf(struct process *process)
{
    int res = 0;

    struct elf_file *elf_file = process->elf_file;
    res = paging_map_to(process->task->page_directory,
                        paging_align_to_lower_page(elf_virtual_base(elf_file)),
                        elf_phys_base(elf_file), paging_align_address(elf_phys_end(elf_file)),
                        PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

int process_map_memory(struct process *process)
{
    int res = 0;
    switch (process->filetype)
    {
    case PROCESS_FILETYPE_ELF:
        res = process_map_elf(process);
        break;

    case PROCESS_FILETYPE_BINARY:
        res = process_map_binary(process);
        break;

    default:
        panic("process_map_memory: Invalid filetype\n");
    }
    if (res < 0)
    {
        goto out;
    }

    // Finally, map the stack
    paging_map_to(process->task->page_directory,
                  (void *)HARDIKHYPERIONOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END,
                  process->stack,
                  paging_align_address(process->stack + HARDIKHYPERIONOS_USER_PROGRAM_STACK_SIZE),
                  PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);

out:
    return res;
}

int progess_get_free_slot()
{
    for (int i = 0; i < HARDIKHYPERIONOS_MAX_PROCESSES; i++)
    {
        if (processes[i] == 0)
        {
            return i;
        }
    }

    return -EISTKN;
}

int process_load(const char *filename, struct process **process)
{
    int res = 0;
    int process_slot = progess_get_free_slot();
    if (process_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);

out:
    return res;
}

int process_load_for_slot(const char *filename, struct process **process, int process_slot)
{
    int res = 0;
    struct task *task = 0;
    struct process *_process = 0;
    void *program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    program_stack_ptr = kzalloc(HARDIKHYPERIONOS_USER_PROGRAM_STACK_SIZE);
    if (!program_stack_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->pid = process_slot;

    // Create a task
    task = task_new(_process);
    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;

    res = process_map_memory(_process);
    if (res < 0)
    {
        goto out;
    }

    *process = _process;

    // Add the process to the array
    processes[process_slot] = _process;

out:
    if (IS_ERR(res))
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

        // TODO: Free the process data
    }
    return res;
}
