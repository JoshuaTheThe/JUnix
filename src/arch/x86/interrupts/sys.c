
#include <interrupts/sys.h>
#include <cpu/cpu.h>
#include <sched/core.h>
#include <string.h>
#include <mm/alloc.h>

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
                case SYS_FORK:
                {
                        // copy process state, not memory (shallow copy)
                        // it is assumed this will only really be used with exec(vxx) after
                        task_state_registers_t copy = *regs;
                        copy.eax = 0;
                        vnode_t *x = scheduler_add_process(copy, "\0");
                        x->name = kmalloc(32);
                        itoa(x->name, ((task_t *)x->private)->pid, 16, 0);
                        regs->eax = ((task_t *)x->private)->pid;
                        break;
                }

                case SYS_OPEN:
                {
                        regs->eax = task_open(current_process_fil->private,
                                              (char *)regs->ebx,regs->ecx,regs->edx);
                        break;
                }
                
                case SYS_CLOSE:
                {
                        task_close(current_process_fil->private, regs->ebx);
                        break;
                }

                case SYS_READ:
                {
                        file_t *file = ((task_t *)current_process_fil->private)->fd.items[regs->ebx];
                        void   *buf  = (void*)regs->ecx;
                        int     cnt  = regs->edx;
                        vfs_read(file, buf, cnt);
                        break;
                }

                case SYS_WRITE:
                {
                        file_t *file = ((task_t *)current_process_fil->private)->fd.items[regs->ebx];
                        void   *buf  = (void*)regs->ecx;
                        int     cnt  = regs->edx;
                        vfs_write(file, buf, cnt);
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

uintptr_t syscall0(uint32_t nr)
{
        uintptr_t ret;

        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr)
                : "memory"
        );

        return ret;
}

uintptr_t syscall1(uint32_t nr, uintptr_t a)
{
        uintptr_t ret;

        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a)
                : "memory"
        );

        return ret;
}

uintptr_t syscall2(uint32_t nr, uintptr_t a, uintptr_t b)
{
        uintptr_t ret;

        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b)
                : "memory"
        );

        return ret;
}

uintptr_t syscall3(uint32_t nr, uintptr_t a, uintptr_t b, uintptr_t c)
{
        uintptr_t ret;
        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b), "d"(c)
                : "memory"
        );

        return ret;
}

uintptr_t syscall4(uint32_t nr,
                   uintptr_t a,
                   uintptr_t b,
                   uintptr_t c,
                   uintptr_t d)
{
        uintptr_t ret;
        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b), "d"(c), "S"(d)
                : "memory"
        );

        return ret;
}

uintptr_t syscall5(uint32_t nr,
                   uintptr_t a,
                   uintptr_t b,
                   uintptr_t c,
                   uintptr_t d,
                   uintptr_t e)
{
        uintptr_t ret;
        __asm__ volatile (
                "int $0x80"
                : "=a"(ret)
                : "a"(nr), "b"(a), "c"(b), "d"(c), "S"(d), "D"(e)
                : "memory"
        );

        return ret;
}
