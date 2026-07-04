
#include <fs/null.h>
#include <panic.h>
#include <string.h>
#include <mm/alloc.h>

static int read(file_t *file, void *buf, size_t count)
{
        not_optional(buf);
        not_optional(file);
        memset(buf, 0, count);
        return count;
}

static int write(file_t *file, const void *buf, size_t count)
{
        not_optional(buf);
        not_optional(file);
        (void)count;
        return count;
}

static file_ops_t ops = {
        .read = read,
        .write = write,
        NULL,NULL,NULL,NULL,NULL,
};

vnode_t *CreateNullDevice(char *name)
{
        not_optional(name);
        vnode_t *dir = kmalloc(sizeof(vnode_t)), *dev;
        memset(dir, 0, sizeof(vnode_t));
        dir->name = name;
        dir->ops  = &ops;
        vfs_lookup("dev", &dev);
        vfs_append_child(dev, dir);
        return dir;
}

