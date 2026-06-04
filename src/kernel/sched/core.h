
#ifndef SCHED_CORE_H
#define SCHED_CORE_H

#include <sched/task.h>
#include <sched/trap.h>
#include <fs/fs.h>

typedef struct task_t
{
        uint64_t               pid;
        task_state_registers_t regs;

        // in future add things like page tabels and perms
        // this is ring0 for now
} task_t;

void scheduler_init(void);
vnode_t *scheduler_add_process(task_state_registers_t initial_regs, char *name);

extern task_state_registers_t scratch_proc;

#endif

