
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
} syscmd_t;

void sys_handler(void);
void sys_wrapper(void);
void sys_yield(void);

uintptr_t syscall0(uint32_t nr);
uintptr_t syscall1(uint32_t nr, uintptr_t a);
uintptr_t syscall2(uint32_t nr, uintptr_t a, uintptr_t b);
uintptr_t syscall3(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c);
uintptr_t syscall4(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d);
uintptr_t syscall5(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);

#endif

