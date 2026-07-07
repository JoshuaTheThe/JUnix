
#ifndef VFS_H
#define VFS_H

#include <stddef.h>
#include <stdint.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define VFS_DIRECTORY 0x1

struct vnode;
struct file;
struct filesystem;

typedef struct file_ops
{
        size_t (*length)(struct file *f);
        struct vnode *(*mkdir)(struct vnode *p, char *name, uint32_t flags);
        int (*readdir)(struct file *f, void *buf, size_t count);
        int (*read)(struct file *f, void *buf, size_t count);
        int (*write)(struct file *f, const void *buf, size_t count);
        int (*open)(struct vnode *v, struct file **f);
        int (*close)(struct file *f);
        long (*lseek)(struct file *f, long offset, int whence);
        void (*release)(struct vnode *v); // called when refcount == 0
        void (*capture)(struct vnode *v); // called when refcount goes from == 0 to > 0
} file_ops_t;

typedef struct vnode
{
        char *name;
        struct vnode *parent;
        struct vnode *next;
        struct vnode *prev;
        struct vnode *children;
        void *private;           // driver-specific data (e.g., inode number)
        file_ops_t *ops;
        uint32_t flags;
        int refcount;
} vnode_t;

typedef struct file
{
        vnode_t *vnode;
        long offset;
        uint32_t flags;
        int refcount;
} file_t;

typedef struct filesystem
{
        char *name;
        int (*mount)(vnode_t *mountpoint, void *data);
} filesystem_t;

int vfs_init(void);
int vfs_init_dev_mnt(void);
int vfs_mount(const char *path, filesystem_t *fs, void *data);
int vfs_open(const char *path, file_t **f);
int vfs_open_direct(vnode_t *node, file_t **f);
int vfs_close(file_t *f);
int vfs_lookup(const char *path, vnode_t **out);
int vfs_read(file_t *f, void *buf, size_t count);
int vfs_write(file_t *f, const void *buf, size_t count);
long vfs_lseek(file_t *f, long offset, int whence);
void vfs_append_child(vnode_t *f, vnode_t *x);
vnode_t *vfs_mkdir(vnode_t *p, char *name, uint32_t flags);
int vfs_readdir(file_t *f, void *buf, size_t count);
vnode_t *vfs_create(char *parent_path, char *name, int flags);
vnode_t *vfs_create_in(vnode_t *parent, char *name, int flags);

extern vnode_t *root_vnode;

#endif
