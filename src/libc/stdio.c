
#include <stdio.h>

extern int main(int, char **argv, char **envp);
extern void init_memory(void);
extern void init_rt(void);
extern void shutdown(void);

void _start(void)
{
        init_memory();
        init_rt();
        int ret = main(0, NULL, NULL);
        shutdown();
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

int write(int fd, const void *const buf, size_t len)
{
        return syscall3(SYS_WRITE, fd, (uintptr_t)buf, len);
}

int read(int fd, void *const buf, size_t len)
{
        return syscall3(SYS_READ, fd, (uintptr_t)buf, len);
}

int open(const char *const path, int flags)
{
        return syscall2(SYS_OPEN, (uintptr_t)path, flags);
}

int open3(const char *const path, int flags, int mode)
{
        return syscall3(SYS_OPEN, (uintptr_t)path, flags, mode);
}

void close(int fd)
{
        syscall1(SYS_CLOSE, fd);
}

int lseek(int fd, int off, int whence)
{
        return syscall3(SYS_LSEEK, fd, off, whence);
}
