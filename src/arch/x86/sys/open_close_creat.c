
#include <sys/sys.h>
#include <string.h>
#include <dbg.h>

int sys_open(char *path, int flags, int mode)
{
        char copy[MAX_PATH];
        memset(copy, 0, sizeof(copy));
        size_t len = user_strnlen(path, MAX_PATH);
        int err = copy_from_user(copy, path, len + 1);
        if (err < 0)
                return -EFAULT;
        return proc_open(current_proc, copy, flags, mode);
}

void sys_close(int fd)
{
        proc_close(current_proc, fd);
}

int sys_creat(char *path, int mode)
{
        char copy[MAX_PATH];
        size_t len = user_strnlen(path, MAX_PATH);
        if (len >= MAX_PATH)
                return -EFAULT;
        if (copy_from_user(copy, path, len + 1) < 0)
                return -EFAULT;
        vnode_t *node;
        if (vfs_lookup(copy, &node) == 0)
        {
                return proc_open(current_proc, copy, 0, mode);
        }

        char parent[MAX_PATH];
        char name[MAX_PATH];
        split_path(copy, parent, name);
        vnode_t *parent_node;
        if (vfs_lookup(parent, &parent_node) < 0)
                return -ENOENT;
        if (vfs_mkdir(parent_node, name, mode) == NULL)
                return -EIO;
        return proc_open(current_proc, copy, 0, mode);
}

int sys_lseek(int fd, int off, int whence)
{
        file_t *file;
        if (fd < 0 || fd >= (int)current_proc->fd.capacity)
                return -EBADF;

        file = current_proc->fd.items[fd];
        return vfs_lseek(file, off, whence);
}
