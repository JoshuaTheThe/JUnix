
#ifndef _PROC_H
#define _PROC_H

#include <stdint.h>
#include <stddef.h>
#include <sched/task.h>
#include <sched/trap.h>
#include <sched/user.h>
#include <mm/paging.h>
#include <fs/fs.h>

#define MAX_TASKS (16)

#define KERNEL_STACK_VIRT (0xC0100000)

typedef uint64_t pid_t;

typedef enum
{
        TASK_READY,
        TASK_RUNNING,
        TASK_BLOCKED,
        TASK_DEAD,
        TASK_WAITING,
} task_state_t;

typedef struct proc_t proc_t;

typedef struct task_t
{
        void                  *kernel_stack;
        proc_t                *parent;
        task_state_t           state;
        task_state_registers_t regs;
} task_t;

typedef struct proc_t
{
        struct proc_t   *next,*prev;
        pid_t            pid;
        char           **argv;
        int              argc;
        vnode_t         *cwd;
        task_t           tasks[MAX_TASKS];
        size_t           taskcount;

        struct
        {
                file_t       **items; // if null, empty
                size_t         count;
                size_t         capacity;
        } fd;

        address_space_t *space;
} proc_t;

typedef enum
{
        SIG_TERM,
} sig_t;

extern task_t         *current_task;
extern proc_t         *current_proc;
extern task_state_registers_t scratch;
extern uint64_t ticks_since_boot;

void sched_next(void);
void sched_load(void);
void sched_save(void);
void sched_init(void);

proc_t *proc_create(void);
task_t *task_create(proc_t *proc);

void    proc_kill(proc_t *); // clear, remove from ll
void    proc_clear(proc_t *); // clear all mappings, and registers, .., also suspend

int proc_open_direct(proc_t *proc, vnode_t *node, int flags, int mode);
int proc_open(proc_t *proc, char *path, int flags, int mode);
void proc_close(proc_t *proc, int fd);
int task_index(task_t *task, proc_t *proc);

#endif
