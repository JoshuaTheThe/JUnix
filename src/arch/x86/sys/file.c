
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
