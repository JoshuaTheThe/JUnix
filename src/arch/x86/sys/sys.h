
#ifndef SYSCALL_H
#define SYSCALL_H

#include <sched/core.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PATH (128)

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

// Syscall handler prototypes

void sys_exit(int);
void sys_fork(void);
int  sys_open(char *, int, int);
void sys_close(int);
int  sys_read(int, void *, size_t);
int  sys_write(int, void *, size_t);

int copy_from_user(void *, void *, size_t);
int copy_to_user(void *, void *, size_t);
size_t user_strnlen(char *, size_t);
bool address_is_user(void *);

#endif

