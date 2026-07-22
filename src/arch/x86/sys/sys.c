
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
                case SYS_EXIT:    sys_exit(regs->ebx); break;
                case SYS_FORK:    sys_fork(); break;
                case SYS_OPEN:    regs->eax = sys_open((char *)regs->ebx, regs->ecx, regs->edx); break;
                case SYS_CLOSE:   sys_close((int)regs->ebx); break;
                case SYS_READ:    regs->eax = sys_read((int)regs->ebx, (void*)regs->ecx, regs->edx);  break;
                case SYS_WRITE:   regs->eax = sys_write((int)regs->ebx, (void*)regs->ecx, regs->edx); break;
                case SYS_WAITPID: break;
                case SYS_CREAT:   regs->eax = sys_creat((char *)regs->ebx, regs->ecx); break;
                case SYS_LINK:    break;
                case SYS_UNLINK:  break;
                case SYS_EXECVE:  break;
                case SYS_CHDIR:   break;
                case SYS_TIME:    break;
                case SYS_MKNOD:   break;
                case SYS_CHMOD:   break;
                case SYS_LCHOWN16:break;
                case SYS_STAT:    break;
                case SYS_LSEEK:   sys_lseek(regs->ebx, regs->ecx, regs->edx); break;

                case SYS_MAP:     paging_map(current_proc->space, regs->ebx, virt_to_phys(&kernel_address_space, pmm_alloc()), regs->ecx); break;
                case SYS_UMAP:    paging_unmap(current_proc->space, regs->ebx); break;

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
