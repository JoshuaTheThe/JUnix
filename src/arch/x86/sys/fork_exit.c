
#include <sys/sys.h>
#include <cpu/cpu.h>
#include <mm/alloc.h>
#include <string.h>
#include <panic.h>
#include <drivers/kprint.h>

void sys_exit(int code)
{
        (void)code;
        kprint(" [sys] exiting with code %d\r\n", code);
        proc_kill(current_proc);
        cpu_ei();
        while(1)
                ;
}

// TODO - safety tighten
void sys_fork(void)
{
        task_state_registers_t *regs = &scratch;
        // copy proc && address space
        proc_t *new = proc_create();
        address_space_t *space = kmalloc(sizeof(*space));
        *space = paging_copy_space(paging_get_address_space());
        new->cwd  = current_proc->cwd;
        new->argv = current_proc->argv;
        new->argc = current_proc->argc;

        // copy tasks
        memcpy(new->tasks, current_proc->tasks, sizeof(new->tasks)); // for now, shallow copy
        new->taskcount = current_proc->taskcount;
        for (size_t i = 0; i < new->taskcount; ++i)
        {
                new->tasks[i].parent = new;
        }

        // copy file descriptors
        for (size_t i = 0; i < current_proc->fd.count; ++i)
        {
                proc_open_direct(new,
                                 current_proc->fd.items[i]->vnode,
                                 current_proc->fd.items[i]->flags,
                                 current_proc->fd.items[i]->mode);
        }

        int id = task_index(current_task, current_proc);
        if (id < 0) // idk how but, can happen i guess
        {
                panic(PANIC_TODO);
        }

        new->tasks[id].regs.eax = 0;
        current_task->regs.eax  = new->pid;
}
