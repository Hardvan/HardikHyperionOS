ORG 0x7c00
BITS 16

start:
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
