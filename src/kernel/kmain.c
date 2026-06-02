
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <fs/fs.h>
#include <fs/ramfs/ramfs.h>
#include <mm/bitmap.h>
#include <arch.h>
#include <string.h>
#include <panic.h>

void list(vnode_t *node, size_t depth)
{
        if (!node)
                return;
        for (size_t i = 0; i < depth; ++i)
                kprint(" ");
        kprint("%s\r\n", node->name);
        list(node->children, depth+1);
        list(node->next, depth);
}

void kmain(void)
{
        arch_init();
        vfs_init();
        serial_init();

        filesystem_t ramfs = ramfs_create_fs();
        if (vfs_mount("/mnt", &ramfs, NULL) != 0)
        {
                kprint("could not create ramfs");
                panic(PANIC_TODO);
        }

        vnode_t *mnt;
        if (vfs_lookup("/mnt", &mnt) != 0)
        {
                kprint("could not create in ramfs");
                panic(PANIC_TODO);
        }

        vfs_mkdir(mnt, "test", 0);
        vnode_t *root = root_vnode;
        list(root, 0);

        file_t *f;
        if (vfs_open("/dev/serial", &f) != 0)
        {
                kprint("could not find");
                panic(PANIC_TODO);
        }

        vfs_write(f, "Hello, VFS World!\r\n", 19);
        vfs_close(f);
        kprint("created ramfs\r\n");
        sti();
        panic(PANIC_TODO);
}
