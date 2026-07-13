
#include <libk.h>
#include <cpu/cpu.h>
#include <drivers/kprint.h>
#include <interrupts/timer.h>
#include <mm/alloc.h>
#include <sched/core.h>
#include <elf.h>

uint32_t entry;
void exec(void)
{
        file_t *file;
        if (vfs_open("/mnt/init", &file) < 0)
                panic(PANIC_TODO);
        elf_load(file, paging_get_address_space(), &entry);
        ((void (*)(void))entry)();
        while(true)
                ;
}

void kmain(void)
{
        cpu_ei();
        uint8_t *buffer = kmalloc(10000);
        int fd = open("/mnt/src/kernel/core/kmain.c", 0);
        if (fd < 0)
                panic(PANIC_TODO);
        read(fd, buffer, 10000);
        kprint("%s\r\n", buffer);
        kfree(buffer);
        close(fd);

        proc_t *proc   = proc_create();
        address_space_t space = paging_copy_space(&kernel_address_space);
        proc->space    = &space;
        task_t *task   = task_create(proc);
        task->regs.eip = (uintptr_t)exec;
        task->regs.esp = (uintptr_t)kmalloc(8192) + 8186; // whatever
        task->regs.cs  = 0x8;
        task->regs.ds  = 0x10;
        task->regs.es  = 0x10;
        task->regs.ss  = 0x10;
        task->regs.fs  = 0x10;
        task->regs.gs  = 0x10;
        task->state    = TASK_RUNNING;

        while (true)
        {
                sys_yield();
        }
}
