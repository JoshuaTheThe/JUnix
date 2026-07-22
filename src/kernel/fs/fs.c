#include <fs/fs.h>
#include <fs/fat/fat.h>
#include <fs/ufs/ufs.h>
#include <fs/ramfs/ramfs.h>
#include <fs/dir.h>
#include <sched/core.h>
#include <string.h>
#include <mm/alloc.h>
#include <panic.h>
#include <fs/ramfs/ramfs.h>
#include <db/db.h>
#include <dbg.h>

vnode_t *root_vnode = NULL;

filesystem_t ramfs = {0};

filesystem_t file_systems[MAX_FILE_SYSTEMS] = {0};
size_t       file_systems_count=0;

filesystem_t *get_file_system(char *name)
{
        for (size_t i = 0; i < file_systems_count; ++i)
        {
                if (!strcmp(name, file_systems[i].name))
                {
                        return &file_systems[i];
                }
        }

        return NULL;
}

int vfs_init(void)
{
        // Create root directory
        root_vnode = kmalloc(sizeof(vnode_t));
        memset(root_vnode, 0, sizeof(vnode_t));
        root_vnode->name = "/";
        root_vnode->ops = &dir_ops;
        root_vnode->flags = 0;
        ramfs = ramfs_create_fs();
        if (vfs_mount("/", &ramfs, NULL) != 0)
        {
                LOG(" [krnl] could not create ramfs\r\n");
                panic(PANIC_TODO);
        }

        file_systems[file_systems_count++] = fat_create_fs();
        file_systems[file_systems_count++] = ufs_create_fs();
        file_systems[file_systems_count++] = ramfs_create_fs();
        file_systems[file_systems_count++] = db_create_fs();
        return 0;
}

int vfs_init_dev_mnt(void)
{ 
        vfs_create("/", "dev", VFS_DIRECTORY)->ops = &dir_ops;
        vfs_create("/", "mnt", VFS_DIRECTORY)->ops = &dir_ops;
        return 0;
}

int vfs_readdir(file_t *f, void *buf, size_t count)
{
        not_optional(f);
        not_optional(buf);
        if (!f || !buf) return -1;
        vnode_t *v = f->vnode;
        if (!v->ops || !v->ops->readdir) return -1;
        return v->ops->readdir(f, buf, count);
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

        vnode_t *current = current_proc->cwd;
        if (path[0] == '/')
                current = root_vnode;
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
                        if (strlen(child->name) == strlen(segment) && !strncmp(child->name, segment, 1024))
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
        if (mountpoint->children) return -1;
        return fs->mount(mountpoint, data);
}

int vfs_open_direct(vnode_t *v, file_t **out)
{
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

int vfs_open(const char *path, file_t **out)
{
        not_optional(path);
        not_optional(out);
        vnode_t *v;
        if (vfs_lookup(path, &v) != 0) return -1;
        return vfs_open_direct(v, out);
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
                not_optional(v->ops->length);
                new_offset = v->ops->length(f) + offset;
                break;
        default:
                return -1;
        }

        if (new_offset < 0) return -1;
        f->offset = new_offset;
        return new_offset;
}

void vfs_append_child(vnode_t *f, vnode_t *x)
{
        x->next = NULL;
        x->parent = f;
        if (!f->children)
        {
                f->children = x;
                return;
        }
        vnode_t *last = f->children;
        while (last->next)
                last = last->next;
        last->next = x;
        x->prev = last;
}

vnode_t *vfs_mkdir(vnode_t *p, char *name, uint32_t flags)
{
        if (p->ops && p->ops->mkdir)
                return p->ops->mkdir(p, name, flags);
        panic(PANIC_UNSUPPORTED_FS_OP);
        return NULL;
}

vnode_t *vfs_create(char *parent_path, char *name, int flags)
{
        vnode_t *p;
        if (vfs_lookup(parent_path, &p) < 0)
        {
                LOG(" [krnl] could not find file: %s\r\n", parent_path);
                panic(PANIC_TODO);
        }

        return vfs_create_in(p, name, flags);
}

vnode_t *vfs_create_in(vnode_t *parent, char *name, int flags)
{
        not_optional(parent);
        not_optional(name);
        vnode_t *n = kmalloc(sizeof(vnode_t));
        n->name  = name;
        n->ops   = NULL;
        n->flags = flags;
        vfs_append_child(parent, n);
        return n;
}

int split_path(char *path, char *parent, char *name)
{
        char *last = strrchr(path, '/');

        if (!last)
        {
                strncpy(parent, ".", MAX_PATH);
                strncpy(name, path, MAX_PATH);
                return 0;
        }

        if (last == path)
        {
                strncpy(parent, "/", MAX_PATH);
        }
        else
        {
                *last = '\0';
                strncpy(parent, path, MAX_PATH);
        }

        strncpy(name, last + 1, MAX_PATH);
        return 0;
}
