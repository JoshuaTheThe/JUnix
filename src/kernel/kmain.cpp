
#include <junix.hpp>

extern "C"
{
        #include <libk.h>
        #include <cpu/cpu.h>
        #include <drivers/kprint.h>
        #include <interrupts/timer.h>
        #include <mm/alloc.h>
        #include <sched/core.h>
        #include <elf.h>
};

uint32_t entry;
void exec(void)
{
        file_t *file;
        if (vfs_open("/mnt/bin/jwm.o", &file) < 0)
                panic(PANIC_TODO);
        elf_load(file, paging_get_address_space(), &entry);
        ((void (*)(void))entry)();
        while(true)
                ;
}

PROCEDURAL void kmain(void)
{
        cpu_ei();
        proc_t *proc   = proc_create();

        address_space_t *space = (address_space_t *)kmalloc(sizeof(*space));
        *space = paging_copy_space(&kernel_address_space);
        proc->space    = space;
        task_t *task   = task_create(proc);
        task->regs.eip = (uintptr_t)exec;
        task->regs.esp = 0xbffff000; // whatever
        task->regs.cs  = 0x8;
        task->regs.ds  = 0x10;
        task->regs.es  = 0x10;
        task->regs.ss  = 0x10;
        task->regs.fs  = 0x10;
        task->regs.gs  = 0x10;
        for (uintptr_t addr = 0xbfff0000;
             addr < 0xbffff000;
             addr += PAGE_SIZE)
        {
                paging_map(space, addr, virt_to_phys(paging_get_address_space(), pmm_alloc()), PAGE_WRITE);
        }
        task->state    = TASK_RUNNING;
        
        proc_t *_proc = processes;
        while (true)
        {
                _proc = _proc->next;
                if (!_proc)
                        _proc = processes;
                if (_proc->awaiting_destruction)
                {
                        proc_destroy(_proc);
                        if (_proc == proc)
                                panic(PANIC_TODO);
                        _proc = processes;
                }
                sys_yield();
        }
}
