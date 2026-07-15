
#include <stdint.h>
#include <stddef.h>

typedef enum
{
        // no more for now
        SYS_NOOP=0,
        SYS_EXIT,
        SYS_FORK,
        SYS_READ,
        SYS_WRITE,
        SYS_OPEN,
        SYS_CLOSE,

        SYS_DBGWRITE=256, // debug write
} syscmd_t;

// SYSCALL FORWARD DECL
void exit(int code);
int write(int fd, const void *buf, size_t len);
int read(int fd, void *buf, size_t len);
int open(char *path, int flags);
void close(int fd);
int fork(void);
void debug(char *);

uintptr_t syscall0(uint32_t nr);
uintptr_t syscall1(uint32_t nr, uintptr_t a);
uintptr_t syscall2(uint32_t nr, uintptr_t a, uintptr_t b);
uintptr_t syscall3(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c);
uintptr_t syscall4(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
uintptr_t syscall5(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);

int main(void)
{
        int fd = open("/dev/serial", 0);
        if (fd < 0)
        {
                debug("error: could not open serial device\r\n");
                return 1;
        }

        write(fd, "Hello, World!\r\n", 15);
        close(fd);
        return 0;
}

void _start(void)
{
        int ret = main();
        exit(ret);
}

// SYSCALLS

uintptr_t syscall0(uint32_t nr)
{
        uintptr_t ret;

        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr)
                : "memory"
        );

        return ret;
}

uintptr_t syscall1(uint32_t nr, uintptr_t a)
{
        uintptr_t ret;

        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a)
                : "memory"
        );

        return ret;
}

uintptr_t syscall2(uint32_t nr, uintptr_t a, uintptr_t b)
{
        uintptr_t ret;

        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b)
                : "memory"
        );

        return ret;
}

uintptr_t syscall3(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c)
{
        uintptr_t ret;
        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b), "d"(c)
                : "memory"
        );

        return ret;
}

uintptr_t syscall4(uint32_t nr,
                   uintptr_t a,
                   uintptr_t b,
                   uintptr_t c,
                   uintptr_t d)
{
        uintptr_t ret;
        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b), "d"(c), "S"(d)
                : "memory"
        );

        return ret;
}

uintptr_t syscall5(uint32_t nr,
                   uintptr_t a,
                   uintptr_t b,
                   uintptr_t c,
                   uintptr_t d,
                   uintptr_t e)
{
        uintptr_t ret;
        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b), "d"(c), "S"(d), "D"(e)
                : "memory"
        );

        return ret;
}

int fork(void)
{
        return syscall0(SYS_FORK);
}

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

        while(1)
                ;
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

void debug(char *m)
{
        syscall1(SYS_DBGWRITE, (uintptr_t)m);
}
