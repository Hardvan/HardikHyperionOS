[BITS 32]

global _start
extern c_start
extern hardikhyperionos_exit

section .asm

_start:
    call c_start
    call hardikhyperionos_exit
    ret