
#include <interrupts/sys.h>
#include <cpu/cpu.h>
#include <sched/core.h>

void sys_handler(void)
{
        task_state_registers_t *regs = &scratch_proc;
        switch (regs->eax)
        {
                case SYS_EXIT:
                        ((task_t *)current_process_fil->private)->active = false;
                        cpu_ei();
                        while(1);
                        break;
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

void exit(int code)
{
        __asm volatile (
                "movl $1, %%eax\n"
                "movl %0, %%ebx\n"
                "int $0x80\n"
                "1: jmp 1b"
                :
                : "r" (code)
                : "eax", "ebx"
        );
}
