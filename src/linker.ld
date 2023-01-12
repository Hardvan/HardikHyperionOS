ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M; /* 1MB */
    .text:
    {
        *(.text)
    }

    .rodata:
    {
        *(.rodata)
    }

    .data:
    {
        *(.data)
    }

    .bss:
    {
        *(COMMON)
        *(.bss)
    }
}