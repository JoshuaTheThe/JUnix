
#ifndef _STDIO_H
#define _STDIO_H

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

uintptr_t syscall0(uint32_t nr);
uintptr_t syscall1(uint32_t nr, uintptr_t a);
uintptr_t syscall2(uint32_t nr, uintptr_t a, uintptr_t b);
uintptr_t syscall3(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c);
uintptr_t syscall4(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
uintptr_t syscall5(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);

#endif
