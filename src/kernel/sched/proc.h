
#ifndef _PROC_H
#define _PROC_H

#include <sched/task.h>
#include <sched/trap.h>
#include <sched/user.h>
#include <mm/paging.h>

#define MAX_KERNEL_MAPPINGS (65536)

typedef uint64_t pid_t;
typedef uint64_t tid_t;

typedef struct
{
        page_directory_t *pd;
        mapping_t        *items;
        size_t            count;
        size_t            capacity;
} address_space_t;

typedef enum
{
        TASK_READY,
        TASK_RUNNING,
        TASK_BLOCKED,
        TASK_DEAD
} task_state_t;

typedef struct proc_t proc_t;

typedef struct task_t
{
        void *kernel_stack;
        struct task_t         *next;
        proc_t                *parent;
        tid_t                  tid;
        task_state_t           state;
        task_state_registers_t regs;
        uint64_t               last_ran; // in kernel ticks, used for schedule, we have 16 in queue, find oldest and use that when recalculating
} task_t;

typedef struct proc_t
{
        // its stored as a flat list, this is just to keep track
        struct proc_t   *parent;
        struct proc_t   *next;
        pid_t            pid;
        char           **argv;
        int              argc;
        vnode_t         *cwd;
        task_t          *tasks;
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
{} sig_t;

extern address_space_t kernel_address_space;
extern task_t         *schedule_queue[16]; // buffer of things to do, added based on priority, recalculate on empty
extern task_t         *current_task;
extern proc_t         *current_proc;

void sched_next(void);
void sched_load(void);
void sched_save(void);
void sched_init(void);

/**
 * Returns a new empty process, with a shallow copy of kernel mappings
 * also 4kb kernel stack
 */
proc_t *proc_create(void);
task_t *proc_create_task(proc_t *proc);
void    proc_kill(proc_t *); // deletes mappings (unless it's shared (kernel is only shared mapping for now)), and removes from ll
void    proc_suspend(proc_t *); // pause
void    proc_resume(proc_t *); // resume
void    proc_clear(proc_t *); // clear all mappings, and registers, also suspend
void    proc_assign_mappings(proc_t *, mapping_t mappings[], size_t count); // copy these mappings
void    task_set_regs(task_t *, task_state_registers_t *);
void    schedule(task_t *); // add it to the schedule list, automatic
void    proc_recalculate_schedule(void);
proc_t *proc_find(pid_t);

#endif
