/*
    ? encodeGdtEntry:
    * Encodes a GDT entry by taking in a pointer to the target memory location
    * and a source GDT entry as struct gdt_structured

    ? gdt_structured_to_gdt:
    * Converts a structured GDT to a GDT by taking in
    * a pointer to the GDT, a pointer to the structured GDT,
    * and the total number of entries
*/

#include "gdt.h"
#include "kernel.h"

// Encode a GDT entry
void encodeGdtEntry(uint8_t *target, struct gdt_structured source)
{
    if ((source.limit > 65536) && (source.limit & 0xFFF) != 0xFFF)
    {
        panic("encodeGdtEntry: Invalid argument\n");
    }

    target[6] = 0x40;
    if (source.limit > 65536)
    {
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    }

    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0x0F;

    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    // Set the type
    target[5] = source.type;
}

// Convert a structured GDT to a GDT
void gdt_structured_to_gdt(struct gdt *gdt, struct gdt_structured *structured_gdt, int total_entries)
{
    for (int i = 0; i < total_entries; i++)
    {
        encodeGdtEntry((uint8_t *)&gdt[i], structured_gdt[i]);
    }
}
