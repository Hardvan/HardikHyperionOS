;   This is a x86 assembly code that creates a bootable image of a FAT16 file system.
;   It starts by setting the code and data segments using the Global Descriptor Table (GDT)
;   and sets the stack pointer to 0x7c00.

ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; FAT16 Header
OEMIdentifier       db 'HARDIOS ' ; 8 bytes (named HARDI on purpose)
BytesPerSector      dw 0x200
SectorsPerCluster   db 0x80
ReservedSectors     dw 200
FATCopies           db 0x02
RootDirEntries      dw 0x40
NumSectors          dw 0x00
MediaType           db 0xF8
SectorsPerFat       dw 0x100
SectorsPerTrack     dw 0x20
NumberOfHeads       dw 0x40
HiddenSectors       dd 0x00
SectorsBig          dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber         db 0x80
WinNTBit            db 0x00
Signature           db 0x29
VolumeID            dd 0xD105
VolumeIDString      db 'HARDIOS BOO' ; 11 bytes (named HARDIOS BOO on purpose)
SystemIDString      db 'FAT16   ' ; 8 bytes (named FAT16 on purpose)




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
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ; Backup the LBA
    ; Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select the master drive
    mov dx, 0x1F6
    out dx, al
    ; Finished sending the highest 8 bits of the lba

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    ; Send more bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory
.next_sector:
    push ecx

; Checking if we need to read
.try_again:
    mov dx, 0x1F7
    in al, dx
    test al, 8
    jz .try_again

; We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret


times 510-($-$$) db 0   ; Fill the rest of the sector with zeros
dw 0xAA55               ; Boot sector signature

