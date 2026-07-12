
#ifndef SYSCALL_H
#define SYSCALL_H

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

void sys_handler(void);
void sys_wrapper(void);
void sys_yield(void);

#endif

