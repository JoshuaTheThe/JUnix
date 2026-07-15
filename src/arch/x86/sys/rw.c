
#include <sys/sys.h>
#include <math.h>
#include <drivers/kprint.h>
#include <dbg.h>

int sys_read(int fd, void *b, size_t n)
{
        char buf[64];
        size_t cnt = 0;
        file_t *file;
        if (fd < 0 || fd >= (int)current_proc->fd.capacity)
                return -EBADF;
        file = current_proc->fd.items[fd];
        if (!file)
                return -EBADF;
        while (n > 0)
        {
                size_t chunk = min(n, sizeof(buf));
                int ret = vfs_read(file, buf, chunk);
                if (ret < 0)
                        return ret;
                if (ret == 0)
                        break;
                int err = copy_to_user((uint8_t *)b + cnt, buf, ret);
                if (err < 0)
                        return err;
                cnt += ret;
                n -= ret;
                if ((size_t)ret < chunk)
                        break;
        }

        return cnt;
}

int sys_write(int fd, void *b, size_t n)
{
        char buf[64];
        size_t cnt = 0;
        file_t *file;
        if (fd < 0 || fd >= (int)current_proc->fd.capacity)
                return -EBADF;

        file = current_proc->fd.items[fd];

        if (!file)
                return -EBADF;

        while (n > 0)
        {
                size_t chunk = min(n, sizeof(buf));

                int err = copy_from_user(buf, (uint8_t *)b + cnt, chunk);
                if (err < 0)
                        return err;

                int ret = vfs_write(file, buf, chunk);
                if (ret < 0)
                        return ret;
                if (ret == 0)
                        break;

                cnt += ret;
                n -= ret;

                if ((size_t)ret < chunk)
                        break;
        }

        return cnt;
}
