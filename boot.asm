ORG 0
BITS 16
_start:
    jmp short start
    nop

 times 33 db 0 ; 33 bytes of zeros

start:
    jmp 0x7c0:step2

handle_zero:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0x00
    int 0x10
    iret            ; Return from interrupt

handle_one:
    mov ah, 0eh
    mov al, 'V'
    mov bx, 0x00
    int 0x10
    iret            ; Return from interrupt

step2:
    cli             ; Clear interrupts
    mov ax, 0x7c0   ; Set the data segment to 0x7c0
    mov ds, ax      ; Set the data segment to 0x7c0
    mov es, ax      ; Set the extra segment to 0x7c0
    mov ax, 0x00    ; Set the video mode to 0x00
    mov ss, ax      ; Set the stack segment to 0x00
    mov sp, 0x7c00  ; Set the stack pointer to 0x7c00
    sti             ; Enables interrupts

    mov word[ss:0x00], handle_zero ; Set the interrupt handler
    mov word[ss:0x02], 0x7c0

    mov word[ss:0x04], handle_one
    mov word[ss:0x06], 0x7c0

    int 1

    mov si, message ; Message to be displayed
    call print      ; Call the print function
    jmp $           ; Jump to the current address

print:
    mov bx, 0       ; Position of the character
.loop:
    lodsb           ; Load the next character into al
    cmp al, 0       ; Compare it with 0
    je .done        ; If it is 0, jump to .done
    call print_char ; Otherwise, print the character
    jmp .loop       ; Jump to .loop
.done:
    ret

print_char:
    mov ah, 0eh     ; Function 0eh is for displaying a character
    int 0x10        ; Interrupt 10h is for video services
    ret

message: db 'Hello, World!', 0 ; Message to be displayed

times 510-($-$$) db 0   ; Fill the rest of the sector with zeros
dw 0xAA55               ; Boot sector signature
