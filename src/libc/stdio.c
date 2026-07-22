
#include <stdio.h>

extern int main(int, char **argv, char **envp);
extern void init_memory(void);
extern void init_rt(void);
extern void shutdown(void);

uintptr_t __heap_next;

void _start(void)
{
        __heap_next = 0xA0000000;
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

void *map(void *virt, int flags)
{
        if (!virt)
        {
                virt = (void *)__heap_next;
                __heap_next += PAGE_SIZE;
        }

        syscall2(SYS_MAP, (uintptr_t)virt, flags);
        return virt;
}

void umap(void *virt)
{
        syscall1(SYS_UMAP, (uintptr_t)virt);
}

void *malloc(size_t bytes)
{
        if (bytes == 0)
                return NULL;
        size_t total = align_up(sizeof(header_t) + bytes, PAGE_SIZE);
        size_t pages = total / PAGE_SIZE;
        header_t *h = map(NULL, 0);
        if (!h)
                return NULL;
        for (size_t i = 1; i < pages; i++)
        {
                if (!map((char *)h + i * PAGE_SIZE, 0))
                {
                        while (i--)
                                umap((char *)h + i * PAGE_SIZE);
                        return NULL;
                }
        }

        h->pages = pages;
        return h + 1;
}

void free(void *ptr)
{
        if (!ptr)
                return;
        header_t *h = (header_t *)ptr - 1;
        for (size_t i = 0; i < h->pages; i++)
                umap((char *)h + i * PAGE_SIZE);
}
