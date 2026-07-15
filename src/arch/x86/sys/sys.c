
#include <sys/sys.h>
#include <cpu/cpu.h>
#include <sched/core.h>
#include <string.h>
#include <mm/alloc.h>
#include <drivers/kprint.h>
#include <panic.h>
#include <dbg.h>

void sys_handler(void)
{
        task_state_registers_t *regs = &scratch;
        switch (regs->eax)
        {
                case SYS_EXIT:  sys_exit(regs->ebx); break;
                case SYS_FORK:  sys_fork(); break;
                case SYS_OPEN:  regs->eax = sys_open((char *)regs->ebx, regs->ecx, regs->edx); break;
                case SYS_CLOSE: sys_close((int)regs->ebx); break;
                case SYS_READ:  regs->eax = sys_read((int)regs->ebx, (void*)regs->ecx, regs->edx);  break;
                case SYS_WRITE: regs->eax = sys_write((int)regs->ebx, (void*)regs->ecx, regs->edx); break;

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
