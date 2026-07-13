
#include <interrupts/sys.h>
#include <cpu/cpu.h>
#include <sched/core.h>
#include <string.h>
#include <mm/alloc.h>
#include <drivers/kprint.h>
#include <panic.h>

void sys_handler(void)
{
        task_state_registers_t *regs = &scratch;
        switch (regs->eax)
        {
                case SYS_EXIT:
                        proc_kill(current_proc);
                        cpu_ei();
                        while(1);
                        break;
                case SYS_FORK:
                {
                        panic(PANIC_TODO);
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
                        break;
                }

                case SYS_OPEN:
                {
                        // lol this would just work**
                        // because we have both kernel and user mapping
                        // so peak
                        // TODO! safety shit
                        regs->eax = proc_open(current_proc, (char *)regs->ebx,regs->ecx,regs->edx);
                        break;
                }
                
                case SYS_CLOSE:
                {
                        // TODO! safety shit
                        proc_close(current_proc, regs->ebx);
                        break;
                }

                case SYS_READ:
                {
                        // TODO! safety shit
                        file_t *file = current_proc->fd.items[regs->ebx];
                        void   *buf  = (void*)regs->ecx;
                        int     cnt  = regs->edx;
                        regs->eax = vfs_read(file, buf, cnt);
                        break;
                }

                case SYS_WRITE:
                {
                        // TODO! safety shit
                        file_t *file = current_proc->fd.items[regs->ebx];
                        void   *buf  = (void*)regs->ecx;
                        int     cnt  = regs->edx;
                        regs->eax = vfs_write(file, buf, cnt);
                        break;
                }

                case SYS_DBGWRITE:
                {
                        kprint("%s", regs->ebx, (char *)regs->ebx);
                        break;
                }

                default:
                        regs->eax = (uint32_t)-1;
                case SYS_NOOP:
                        break;
        }
}

void sys_yield(void)
{
        cpu_ei();
        cpu_pause();
}
