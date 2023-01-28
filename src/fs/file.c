/*
    ? fopen:
    * a function that opens a file with a given filename and mode.
    * It first parses the filename to extract the root path and drive number,
    * and then checks if the corresponding disk exists.
    * It then resolves the filesystem
    * and calls the filesystem's open function to open the file,
    * creating a new file descriptor and returning its index.
    * If any errors occur, it returns a negative error code.

    ? fstat:
    * is a function that takes in a file descriptor (fd)
    * and a pointer to a struct file_stat as arguments.
    * It retrieves a file's status information, such as its size, permissions, and timestamps,
    * and stores it in the struct file_stat pointed to by the second argument.

    ? fseek:
    * is a function that takes in a file descriptor (fd), an offset, and a FILE_SEEK_MODE as arguments.
    * It changes the current position in the file referenced by the file descriptor.

    ? fread:
    * is a function that takes in a pointer to a buffer ptr, a size,
    * a nmemb and an integer file descriptor as arguments.
    * It reads nmemb number of blocks of size bytes from the file referenced by the file descriptor
    * and stores them in the buffer pointed to by ptr.

    ? fclose:
    * is a function that takes in a file descriptor as its argument.
    * It closes the file referenced by the file descriptor
    * and releases any resources associated with it.
    * It returns 0 if the file is closed successfully,
    * else it returns -EIO.
*/

#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "kernel.h"
#include "fat/fat16.h"
#include "disk/disk.h"
#include "string/string.h"

struct filesystem *filesystems[HARDIKHYPERIONOS_MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[HARDIKHYPERIONOS_MAX_FILE_DESCRIPTORS];

// Get a free file descriptor
static struct filesystem **fs_get_free_filesystem()
{
    int i = 0;
    for (i = 0; i < HARDIKHYPERIONOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

// Insert a filesystem
void fs_insert_filesystem(struct filesystem *filesystem)
{
    struct filesystem **fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
        print("Problem inserting filesystem");
        while (1)
        {
        }
    }

    *fs = filesystem;
}

static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());
}

void fs_load()
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static void file_free_descriptor(struct file_descriptor *desc)
{
    file_descriptors[desc->index - 1] = 0x00;
    kfree(desc);
}

// Get a free file descriptor
static int file_new_descriptor(struct file_descriptor **desc_out)
{
    int res = -ENOMEM;
    for (int i = 0; i < HARDIKHYPERIONOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
            desc->index = i + 1; // Descriptors start at 1
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor *file_get_descriptor(int fd)
{
    if (fd <= 0 || fd > HARDIKHYPERIONOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    int index = fd - 1;

    return file_descriptors[index];
}

struct filesystem *fs_resolve(struct disk *disk)
{
    struct filesystem *fs = 0;
    for (int i = 0; i < HARDIKHYPERIONOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

FILE_MODE file_get_mode_by_string(const char *str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if (strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}

// Open a file
int fopen(const char *filename, const char *mode_str)
{
    int res = 0;

    struct path_root *root_path = pathparser_parse(filename, NULL);
    if (!root_path)
    {
        res = -EINVARG;
        goto out;
    }

    if (!root_path->first)
    {
        res = -EINVARG;
        goto out;
    }

    // Ensure the disk exists
    struct disk *disk = disk_get(root_path->drive_no);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }

    // Ensure the filesystem exists
    if (!disk->filesystem)
    {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out;
    }

    void *descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if (IS_ERR(descriptor_private_data))
    {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor *desc = 0;
    res = file_new_descriptor(&desc);
    if (res < 0)
    {
        goto out;
    }
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

out:
    if (res < 0)
        res = 0;

    return res;
}

int fstat(int fd, struct file_stat *stat)
{
    int res = 0;
    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->stat(desc->disk, desc->private, stat);

out:
    return res;
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence)
{
    int res = 0;
    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->seek(desc->private, offset, whence);

out:
    return res;
}

// Read from a file
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd)
{
    int res = 0;

    if (size == 0 || nmemb == 0 || fd < 1)
    {
        res = -EINVARG;
        goto out;
    }

    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }

    res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char *)ptr);

out:
    return res;
}

// Close a file
int fclose(int fd)
{
    int res = 0;
    struct file_descriptor *desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->close(desc->private);
    if (res == HARDIKHYPERIONOS_ALL_OK)
    {
        file_free_descriptor(desc);
    }

out:
    return res;
}
