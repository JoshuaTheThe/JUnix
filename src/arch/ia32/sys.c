
#include <sys.h>
#include <sched/core.h>

void sys_handler(void)
{
        task_state_registers_t *regs = &scratch_proc;
        regs->eax = -1;
}

