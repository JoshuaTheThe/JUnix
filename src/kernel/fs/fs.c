#include <fs/fs.h>
#include <fs/dir.h>
#include <string.h>
#include <mm/alloc.h>
#include <panic.h>

vnode_t *root_vnode = NULL;

int vfs_init(void)
{
        // Create root directory
        root_vnode = kmalloc(sizeof(vnode_t));
        memset(root_vnode, 0, sizeof(vnode_t));
        root_vnode->name = "/";
        root_vnode->ops = &dir_ops;
        root_vnode->flags = 0;
        // Create /dev directory
        vnode_t *dev = kmalloc(sizeof(vnode_t));
        memset(dev, 0, sizeof(vnode_t));
        dev->name = "dev";
        dev->parent = root_vnode;
        dev->ops = &dir_ops;
        root_vnode->children = dev;
        return 0;
}

int vfs_lookup(const char *path, vnode_t **out)
{
        not_optional(path);
        not_optional(out);
        if (!*path) return -1;
        if (path[0] == '/' && path[1] == '\0')
        {
                *out = root_vnode;
                return 0;
        }

        vnode_t *current = root_vnode;
        if (*path == '/') path++;
        char segment[256];
        while (*path)
        {
                int i = 0;
                while (*path && *path != '/')
                {
                        segment[i++] = *path++;
                }
                segment[i] = '\0';
                if (*path == '/') path++;
                vnode_t *child = current->children;
                while (child)
                {
                        if (!strncmp(child->name, segment, 1024))
                        {
                                current = child;
                                break;
                        }
                        child = child->next;
                }
                if (!child) return -1;  // not found
        }

        *out = current;
        return 0;
}

int vfs_mount(const char *path, filesystem_t *fs, void *data)
{
        not_optional(path);
        not_optional(fs);
        vnode_t *mountpoint;
        if (vfs_lookup(path, &mountpoint) != 0) return -1;
        return fs->mount(mountpoint, data);
}

int vfs_open(const char *path, file_t **out)
{
        not_optional(path);
        not_optional(out);
        vnode_t *v;
        if (vfs_lookup(path, &v) != 0) return -1;
        file_t *f = kmalloc(sizeof(file_t));
        if (!f) return -1;
        memset(f, 0, sizeof(file_t));
        f->vnode = v;
        f->offset = 0;
        f->flags = 0;
        f->refcount = 1;
        v->refcount++;
        if (v->ops && v->ops->open)
        {
                int ret = v->ops->open(v, &f);
                if (ret != 0)
                {
                        kfree(f);
                        return ret;
                }
        }

        *out = f;
        return 0;
}

int vfs_close(file_t *f)
{
        not_optional(f);
        vnode_t *v = f->vnode;
        if (v->ops && v->ops->close)
        {
                v->ops->close(f);
        }
    
        v->refcount--;
        if (v->refcount == 0 && v->ops && v->ops->release)
        {
                v->ops->release(v);
        }
    
        kfree(f);
        return 0;
}

int vfs_read(file_t *f, void *buf, size_t count)
{
        not_optional(f);
        not_optional(buf);
        if (!f || !buf) return -1;
        vnode_t *v = f->vnode;
        if (!v->ops || !v->ops->read) return -1;
        return v->ops->read(f, buf, count);
}

int vfs_write(file_t *f, const void *buf, size_t count)
{
        not_optional(f);
        not_optional(buf);
        if (count == 0) return 0;
        vnode_t *v = f->vnode;
        if (!v->ops || !v->ops->write) return -1;
        return v->ops->write(f, buf, count);
}

long vfs_lseek(file_t *f, long offset, int whence)
{
        not_optional(f);
        vnode_t *v = f->vnode;
        if (v->ops && v->ops->lseek)
        {
                return v->ops->lseek(f, offset, whence);
        }
        
        long new_offset;
        switch (whence)
        {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = f->offset + offset;
            break;
        case SEEK_END:
            // For regular files, you'd need file size
            // For now, return error
            return -1;
        default:
            return -1;
        }

        if (new_offset < 0) return -1;
        f->offset = new_offset;
        return new_offset;
}
