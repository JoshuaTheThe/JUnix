
#ifndef SCHED_CORE_H
#define SCHED_CORE_H

#include <sched/task.h>
#include <sched/trap.h>
#include <sched/user.h>
#include <mm/paging.h>
#include <fs/fs.h>

#define MAX_MAPPINGS (16384 + 1024)

typedef uint64_t pid_t;

typedef struct task_t
{
        task_state_registers_t regs;
        pid_t                  pid;
        bool                   active;
        bool                   waiting;
        userid_t               user;
        char                 **argv;
        int                    argc;
        vnode_t               *cwd;

        struct
        {
                file_t       **items; // if null, empty
                size_t         count;
                size_t         capacity;
        } fd;

        struct
        {
                struct {
                        pid_t pid;
                        int type;
                } *items; // yes
                size_t   capacity;
                size_t   count;
                int      result;
        } waiting_for_me;

        struct
        {
                mapping_t  items[MAX_MAPPINGS];
                size_t     count;
        } mappings;

        page_directory_t *pd;
        
        // in future add things like page tabels and perms
        // this is ring0 for now
} task_t;

int  task_open(task_t *task, char *path, int flags, int mode);
void task_close(task_t *task, int fd);

void scheduler_init(void);
vnode_t *scheduler_add_process(task_state_registers_t initial_regs, char *name);
vnode_t *scheduler_find_process(pid_t pid);

extern task_state_registers_t scratch_proc;
extern vnode_t *override_next, *current_process_fil;
extern uint32_t ticks_since_boot;

extern task_t  early_task;
extern task_t *active_task;

#endif

