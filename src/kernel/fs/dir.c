
#include <fs/fs.h>
#include <fs/dir.h>
#include <string.h>

static int dir_read(file_t *f, void *buf, size_t count)
{
        // Reading a directory returns directory entries
        // just return filenames
        vnode_t *v = f->vnode;
        vnode_t *child = v->children;
        char *out = buf;
        size_t written = 0;
        while (child && written + strnlen(child->name, 1024) + 1 < count)
        {
                const char *name = child->name;
                while (*name) *out++ = *name++;
                *out++ = '\n';
                written += strnlen(child->name, 1024) + 1;
                child = child->next;
        }

        return written;
}

file_ops_t dir_ops =
{
        .readdir = dir_read,
        .read = NULL,
        .write = NULL,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
};
