
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
                        //// copy process state, not memory (shallow copy)
                        //// it is assumed this will only really be used with exec(vxx) after
                        //task_state_registers_t copy = *regs;
                        //copy.eax = 0;
                        //vnode_t *x = scheduler_add_process(copy, "\0");
                        //x->name = kmalloc(32);
                        //itoa(x->name, ((task_t *)x->private)->pid, 16, 0);
                        //regs->eax = ((task_t *)x->private)->pid;
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
