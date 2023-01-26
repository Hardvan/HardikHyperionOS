[BITS 32]

section .asm

global print:function
global hardikhyperionos_getkey:function
global hardikhyperionos_malloc:function
global hardikhyperionos_free:function
global hardikhyperionos_putchar:function
global hardikhyperionos_process_load_start:function
global hardikhyperionos_process_get_arguments:function
global hardikhyperionos_system:function

; void print(const char* filename)
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1 ; Command print
    int 0x80
    add esp, 4
    pop ebp
    ret

; int hardikhyperionos_getkey()
hardikhyperionos_getkey:
    push ebp
    mov ebp, esp
    mov eax, 2 ; Command getkey
    int 0x80
    pop ebp
    ret

; void hardikhyperionos_putchar(char c)
hardikhyperionos_putchar:
    push ebp
    mov ebp, esp
    mov eax, 3 ; Command putchar
    push dword [ebp+8] ; Variable "c"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void* hardikhyperionos_malloc(size_t size)
hardikhyperionos_malloc:
    push ebp
    mov ebp, esp
    mov eax, 4 ; Command malloc (Allocates memory for the process)
    push dword[ebp+8] ; Variable "size"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void hardikhyperionos_free(void* ptr)
hardikhyperionos_free:
    push ebp
    mov ebp, esp
    mov eax, 5 ; Command 5 free (Frees the allocated memory for this process)
    push dword[ebp+8] ; Variable "ptr"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void hardikhyperionos_process_load_start(const char* filename)
hardikhyperionos_process_load_start:
    push ebp
    mov ebp, esp
    mov eax, 6 ; Command 6 process load start ( stars a process )
    push dword[ebp+8] ; Variable "filename"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void hardikhyperionos_process_get_arguments(struct process_arguments* arguments)
hardikhyperionos_process_get_arguments:
    push ebp
    mov ebp, esp
    mov eax, 8 ; Command 8 Gets the process arguments
    push dword[ebp+8] ; Variable arguments
    int 0x80
    add esp, 4
    pop ebp
    ret

hardikhyperionos_system:
    push ebp
    mov ebp, esp
    mov eax, 7 ; Command 7 process_system (runs a system command based on the arguments)
    push dword[ebp+8] ; Variable "arguments"
    int 0x80
    add esp, 4
    pop ebp
    ret