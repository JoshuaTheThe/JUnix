
#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>
#include <stddef.h>

#define SEEK_SET (0)
#define SEEK_CUR (1)
#define SEEK_END (2)

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
        SYS_WAITPID,
        SYS_CREAT,
        SYS_LINK,
        SYS_UNLINK,
        SYS_EXECVE,
        SYS_CHDIR,
        SYS_TIME,
        SYS_MKNOD,
        SYS_CHMOD,
        SYS_LCHOWN16,
        SYS_STAT,
        SYS_LSEEK,

        SYS_DBGWRITE=256, // debug write
} syscmd_t;

// SYSCALL FORWARD DECL
void exit(int code);
int write(int fd, const void *const buf, size_t len);
int read(int fd, void *const buf, size_t len);
int open(const char *const path, int flags);
int open3(const char *const path, int flags, int mode);
int lseek(int fd, int off, int whence);
void close(int fd);
int fork(void);

uintptr_t syscall0(uint32_t nr);
uintptr_t syscall1(uint32_t nr, uintptr_t a);
uintptr_t syscall2(uint32_t nr, uintptr_t a, uintptr_t b);
uintptr_t syscall3(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c);
uintptr_t syscall4(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
uintptr_t syscall5(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);

#endif
