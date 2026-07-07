
#include <libk.h>

void exit(int code)
{
        __asm volatile (
                "movl $1, %%eax\n"
                "movl %0, %%ebx\n"
                "int $0x80\n"
                "1: jmp 1b"
                :
                : "r" (code)
                : "eax", "ebx"
        );
}

int write(int fd, const void *buf, size_t len)
{
        return syscall3(SYS_WRITE, fd, (uintptr_t)buf, len);
}

int read(int fd, void *buf, size_t len)
{
        return syscall3(SYS_READ, fd, (uintptr_t)buf, len);
}

int open(char *path, int flags)
{
        return syscall2(SYS_OPEN, (uintptr_t)path, flags);
}

void close(int fd)
{
        syscall1(SYS_CLOSE, fd);
}
