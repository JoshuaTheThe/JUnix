
#ifndef SYSCALL_H
#define SYSCALL_H

#include <sched/core.h> // so we can have task state, currently unimpl

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
} SYSCMD;

void sys_handler(void);
void sys_wrapper(void);

#endif

