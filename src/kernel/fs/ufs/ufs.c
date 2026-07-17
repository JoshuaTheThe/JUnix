
#include "ufs.h"
#include <fs/ufs/ufs.h>
#include <mm/alloc.h>
#include <string.h>
#include <panic.h>

static file_ops_t ufs_file_ops =
{
        .mkdir = NULL,
        .read = NULL,
        .write = NULL,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
};

static file_ops_t ufs_dir_ops =
{
        .mkdir = NULL,
        .read = NULL,
        .write = NULL,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
};

static void ufs_add_files(file_t *fil, uint16_t LBA);

static void ufs_add_file(vnode_t *root, ufs_file_t *fil)
{
        vnode_t *v = vfs_create_in(root, fil->name, 0);
        v->ops = &ufs_file_ops;
}

static void ufs_add_files(file_t *fil, uint16_t LBA)
{
        ufs_block_t block;
        while (LBA != 65535)
        {
                vfs_lseek(fil, LBA << 9, SEEK_SET);
                vfs_read(fil, &block, sizeof(block));
                if (block.next == UFS_FREE)
                {
                        panic(PANIC_CORRUPT_FS);
                }

                for (size_t i = 0; i < sizeof(block.files) / sizeof(block.files[0]); ++i)
                {
                        ufs_add_file(fil->vnode, &block.files[i]);
                }

                LBA = block.next;
        }
}

static void ufs_readfs(file_t *fil, ufs_boot_sector_t *bs)
{
        uint16_t LBA = bs->root_block;
        ufs_add_files(fil, LBA);
}

static int ufs_mount(vnode_t *n, vnode_t *param)
{
        (void)param;
        n->ops = &ufs_dir_ops;
        n->children = NULL;
        n->flags |= VFS_DIRECTORY;

        ufs_boot_sector_t bs = {0};
        file_t *fil;
        if (vfs_open_direct(n, &fil) < 0) return -1;
        vfs_read(fil, &bs, sizeof(bs));
        if (strncmp(bs.sign, UFS_SIGN, 4) || bs.ver > UFS_MAX_VER)
        {
                vfs_close(fil);
                return -1;
        }

        ufs_readfs(fil, &bs);
        vfs_close(fil);
        return 0;
}

filesystem_t ufs_create_fs(void)
{
        return (filesystem_t){.name = "ufs", .mount = ufs_mount};
}
