ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start
    nop

 times 33 db 0 ; 33 bytes of zeros

start:
    jmp 0:step2

step2:
    cli             ; Clear interrupts
    mov ax, 0x00    ; Set the data segment to 0x7c0
    mov ds, ax      ; Set the data segment to 0x7c0
    mov es, ax      ; Set the extra segment to 0x7c0
    mov ss, ax      ; Set the stack segment to 0x00
    mov sp, 0x7c00  ; Set the stack pointer to 0x7c00
    sti             ; Enables interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32

; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:           ; Code Segement should point to this
    dw 0xffff       ; Segment Limit first 0-15 bits
    dw 0            ; Base Address first 0-15 bits
    db 0            ; Base Address 16-23 bits
    db 0x9a         ; Access Byte
    db 11001111b    ; Granularity Byte
    db 0            ; Base Address 24-31 bits

; offset 0x10
gdt_data:           ; Data Segement should point to this
    dw 0xffff       ; Segment Limit first 0-15 bits
    dw 0            ; Base Address first 0-15 bits
    db 0            ; Base Address 16-23 bits
    db 0x92         ; Access Byte
    db 11001111b    ; Granularity Byte
    db 0            ; Base Address 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
load32:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    jmp $


times 510-($-$$) db 0   ; Fill the rest of the sector with zeros
dw 0xAA55               ; Boot sector signature

