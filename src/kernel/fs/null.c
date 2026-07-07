
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
        vnode_t *dir = vfs_create("/dev", name, 0);
        dir->ops  = &ops;
        return dir;
}

