/*
    ? diskstreamer_new:
    * creates a new disk streamer,
    * takes in an integer 'disk_id' as input
    * and returns a pointer to a newly created 'struct disk_stream'.

    ? diskstreamer_seek:
    * seeks to a specific position,
    * takes in a pointer to a 'struct disk_stream'
    * and an integer 'pos' as input
    * and returns an integer indicating the status of the operation.

    ? diskstreamer_read:
    * reads a specified number of bytes from the stream,
    * takes in a pointer to a 'struct disk_stream',
    * a pointer 'out' to the buffer where the read data will be stored
    * and an integer 'total' indicating the number of bytes to be read
    * and returns an integer indicating the status of the operation.

    ?diskstreamer_close:
    * closes the stream by freeing the memory
    * allocated for the 'struct disk_stream'
    * and takes in a pointer to the 'struct disk_stream' as input.
*/

#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"
#include <stdbool.h>

// Create a new disk streamer
struct disk_stream *diskstreamer_new(int disk_id)
{
    struct disk *disk = disk_get(disk_id);
    if (!disk)
        return 0;

    struct disk_stream *streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;

    return streamer;
}

// Seek to a position
int diskstreamer_seek(struct disk_stream *stream, int pos)
{
    stream->pos = pos;
    return 0;
}

// Read from a stream
int diskstreamer_read(struct disk_stream *stream, void *out, int total)
{
    int sector = stream->pos / HARDIKHYPERIONOS_SECTOR_SIZE;
    int offset = stream->pos % HARDIKHYPERIONOS_SECTOR_SIZE;
    int total_to_read = total;
    bool overflow = (offset + total_to_read) >= HARDIKHYPERIONOS_SECTOR_SIZE;
    char buf[HARDIKHYPERIONOS_SECTOR_SIZE];

    if (overflow)
    {
        total_to_read -= (offset + total_to_read) - HARDIKHYPERIONOS_SECTOR_SIZE;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0)
        goto out;

    for (int i = 0; i < total_to_read; i++)
    {
        *(char *)out++ = buf[offset + i];
    }

    // Adjust the stream
    stream->pos += total_to_read;
    if (overflow)
    {
        res = diskstreamer_read(stream, out, total - total_to_read);
    }

out:
    return res;
}

// Close a stream
void diskstreamer_close(struct disk_stream *stream)
{
    kfree(stream);
}
