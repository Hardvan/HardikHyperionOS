/*
    ? disk_read_sector:
    * reads a sector from the disk,
    * using the specified logical block address (lba)
    * and total number of sectors to read,
    * and stores the data in the buffer pointed to by buf.

    ? disk_search_and_init:
    * initializes the disk, by setting the
    * disk type, sector size, id, and filesystem.

    ? disk_get:
    * returns a pointer to the disk struct
    * based on the index provided,
    * if index is not zero, it will return NULL.

    ? disk_read_block:
    * reads a block from the disk, using the
    * disk struct pointer, logical block address (lba)
    * and total number of block to read,
    * and stores the data in the buffer pointed to by buf.
*/

#include "disk.h"
#include "io/io.h"
#include "memory/memory.h"
#include "status.h"
#include "config.h"

struct disk disk;

// Read a sector from the disk
int disk_read_sector(int lba, int total, void *buf)
{
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xFF));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short *ptr = (unsigned short *)buf;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while (!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }

    return 0;
}

// Initialize the disk
void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.type = HARDIKHYPERIONOS_DISK_TYPE_REAL;
    disk.sector_size = HARDIKHYPERIONOS_SECTOR_SIZE;
    disk.id = 0;
    disk.filesystem = fs_resolve(&disk);
}

// Get a disk by index
struct disk *disk_get(int index)
{
    if (index != 0)
        return 0;

    return &disk;
}

// Read a block from the disk
int disk_read_block(struct disk *idisk, unsigned int lba, int total, void *buf)
{
    if (idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}
