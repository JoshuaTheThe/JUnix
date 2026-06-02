#include <fs/fs.h>
#include <mm/alloc.h>
#include <string.h>

static int ramfs_read(file_t *f, void *buf, size_t count);
static int ramfs_write(file_t *f, const void *buf, size_t count);
static int ramfs_open(vnode_t *v, file_t **f);
static int ramfs_close(file_t *f);
static long ramfs_lseek(file_t *f, long offset, int whence);
static void ramfs_release(vnode_t *v);
static vnode_t *ramfs_mkdir(vnode_t *p, char *name, uint32_t flags);

static file_ops_t ramfs_file_ops =
{
        .read = ramfs_read,
        .write = ramfs_write,
        .open = ramfs_open,
        .close = ramfs_close,
        .lseek = ramfs_lseek,
        .release = ramfs_release,
        .capture = NULL,
        .mkdir = NULL,
};

static file_ops_t ramfs_dir_ops =
{
        .mkdir = ramfs_mkdir,
        .read = NULL,  // directories can't be read directly
        .write = NULL,
        .open = ramfs_open,
        .close = ramfs_close,
        .lseek = ramfs_lseek,
        .release = NULL,
        .capture = NULL,
};

typedef struct ramfs_file
{
        char *data;
        size_t size;
        size_t capacity;
} ramfs_file_t;

static int ramfs_read(file_t *f, void *buf, size_t count)
{
        vnode_t *v = f->vnode;
        ramfs_file_t *rf = v->private;
        if (!rf || (size_t)f->offset >= rf->size) return 0;
        size_t available = rf->size - f->offset;
        size_t to_read = count < available ? count : available;
        memcpy(buf, rf->data + f->offset, to_read);
        f->offset += to_read;
        return to_read;
}

static int ramfs_write(file_t *f, const void *buf, size_t count)
{
        vnode_t *v = f->vnode;
        ramfs_file_t *rf = v->private;
        if (!rf)
        {
                rf = kmalloc(sizeof(ramfs_file_t));
                memset(rf, 0, sizeof(ramfs_file_t));
                v->private = rf;
        }
    
        size_t new_end = f->offset + count;
        if (new_end > rf->capacity)
        {
                size_t new_cap = rf->capacity * 2;
                if (new_cap < 64) new_cap = 64;
                while (new_cap < new_end) new_cap *= 2;
                char *new_data = kmalloc(new_cap);
                if (rf->data)
                {
                        memcpy(new_data, rf->data, rf->size);
                        kfree(rf->data);
                }
                rf->data = new_data;
                rf->capacity = new_cap;
        }
    
        memcpy(rf->data + f->offset, buf, count);
        f->offset += count;
        if ((size_t)f->offset > rf->size) rf->size = f->offset;
        return count;
}

static int ramfs_open(vnode_t *v, file_t **f)
{
        (void)f;
        (void)v;
        return 0;
}

static int ramfs_close(file_t *f)
{
        (void)f;
        return 0;
}

static long ramfs_lseek(file_t *f, long offset, int whence)
{
        vnode_t *v = f->vnode;
        ramfs_file_t *rf = v->private;
        long new_offset;
        switch (whence)
        {
                case SEEK_SET: new_offset = offset; break;
                case SEEK_CUR: new_offset = f->offset + offset; break;
                case SEEK_END: 
                if (!rf) new_offset = offset;
                else new_offset = rf->size + offset;
                break;
                default: return -1;
        }
    
        if (new_offset < 0) return -1;
        f->offset = new_offset;
        return new_offset;
}

static void ramfs_release(vnode_t *v)
{
        ramfs_file_t *rf = v->private;
        if (rf)
        {
                if (rf->data) kfree(rf->data);
                kfree(rf);
        }
}

int ramfs_mount(vnode_t *mountpoint, void *data)
{
        (void)data;
        mountpoint->ops = &ramfs_dir_ops;
        mountpoint->children = NULL;
        mountpoint->flags |= VFS_DIRECTORY;
        return 0;  // Success
}

filesystem_t ramfs_create_fs(void)
{
        return (filesystem_t){
                .name = "ramfs",
                .mount = ramfs_mount
        };
}

vnode_t *ramfs_mkdir(vnode_t *p, char *name, uint32_t flags)
{
        vnode_t *dir = kmalloc(sizeof(vnode_t));
        memset(dir, 0, sizeof(vnode_t));
        dir->name = name;
        dir->ops = (flags & VFS_DIRECTORY) ? &ramfs_dir_ops : &ramfs_file_ops;
        vfs_append_child(p, dir);
        return dir;
}
