
#include <libk.h>
#include <cpu/cpu.h>
#include <drivers/kprint.h>
#include <interrupts/timer.h>
#include <mm/alloc.h>
#include <sched/core.h>

void worker(void)
{
        kprint(" [worker] working..\r\n");
        while(1)
                ;
}

void kmain(void)
{
        cpu_ei();
        uint8_t *buffer = kmalloc(2048);
        int fd = open("/mnt/hello.txt", 0);
        if (fd < 0)
                panic(PANIC_TODO);
        read(fd, buffer, 2048);
        kprint("%s\r\n", buffer);
        kfree(buffer);
        close(fd);
        task_t *task   = task_create(current_proc);
        task->regs.eip = (uintptr_t)worker;
        task->regs.esp = (uintptr_t)kmalloc(1024) + 1020; // whatever
        task->regs.cs  = 0x8;
        task->regs.ds  = 0x10;
        task->regs.es  = 0x10;
        task->regs.ss  = 0x10;
        task->regs.fs  = 0x10;
        task->regs.gs  = 0x10;
        task->state    = TASK_RUNNING;
        sys_yield();
        panic(PANIC_TODO);
}
