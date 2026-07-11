
#include <fs/part.h>
#include <panic.h>
#include <mm/alloc.h>
#include <drivers/kprint.h>
#include <string.h>

static int read(file_t *f, void *buf, size_t count)
{
        size_t off = ((partition_t *)(f->vnode)->private)->offset + f->offset;
        vfs_lseek(((partition_t *)(f->vnode)->private)->reference, off, SEEK_SET);
        int r = vfs_read(((partition_t *)(f->vnode)->private)->reference, buf, count);
        f->offset += count;
        return r;
}

static int write(file_t *f, const void *buf, size_t count)
{
        size_t off = ((partition_t *)(f->vnode)->private)->offset + f->offset;
        vfs_lseek(((partition_t *)(f->vnode)->private)->reference, off, SEEK_SET);
        int r = vfs_write(((partition_t *)(f->vnode)->private)->reference, buf, count);
        f->offset += count;
        return r;
}

static file_ops_t ops = {
        .read = read,
        .write = write,
};

int search_for_partitions(vnode_t *node)
{
        char mbr[512];
        not_optional(node);
        file_t *file;
        if (vfs_open_direct(node, &file) < 0)
                return -1;
        if (vfs_read(file, mbr, 512) < 0)
                goto error;
        mbr_partition_t *parts = (mbr_partition_t *)(mbr + 446);
        uint16_t sig = *(uint16_t *)&mbr[510];
        if (sig != 0xAA55)
                goto error;

        for (size_t i = 0; i < 4; ++i)
        {
                if (parts[i].type == 0x0C) // FAT32/LBA
                {
                        kprint(" [krnl] found fat32 / lba partition\r\n");
                        size_t size = strnlen(node->name, 256);
                        char *clone = kmalloc(size + 8);
                        memcpy(clone, node->name, size);
                        clone[size] = 'p';
                        itoa(&clone[size+1], i, 10, 0);
                        vnode_t *node = vfs_create("/dev", clone, 0);
                        partition_t *part = kmalloc(sizeof(*part));
                        part->reference = file;
                        part->offset    = parts[i].start_lba * 512;
                        node->ops       = &ops;
                        node->private   = part;
                }
        }
        
        return 0;
error:
        vfs_close(file);
        return -1;
}
