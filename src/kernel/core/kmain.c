
#include <libk.h>
#include <cpu/cpu.h>
#include <drivers/kprint.h>
#include <interrupts/timer.h>
#include <sched/core.h>
#include <mm/alloc.h>
#include <elf/elf.h>

void kmain(void)
{
        cpu_ei();
        uint8_t *buffer = kmalloc(2048);
        char *argv[] = { "/mnt/init", NULL };
        char *path = argv[0];
        int fd = open(path, 0);
        if (fd < 0)
                panic(PANIC_TODO);
        int l = read(fd, buffer, 2048);
        bool suc = false;

        cpu_di();
        pid_t pid = elfLoadProgram(buffer, l, &suc, 0, 1, argv);
        if (!suc)
        {
                kprint(" [krnl] failed to open elf program\r\n");
                panic(PANIC_TODO);
        }
        
        kprint(" [krnl] executed /mnt/init on pid %d (waow)\r\n", pid);
        kprint(" [krnl] %d bytes\r\n", l);
        kfree(buffer);
        close(fd);
        cpu_ei();
        task_t *task = scheduler_find_process(pid)->private;
        while(1)
        {
                kprint(" [krnl] EAX=%x\r\n", task->regs.eax);
                kprint("    --- EBX=%x\r\n", task->regs.ebx);
                kprint("    --- ECX=%x\r\n", task->regs.ecx);
                kprint("    --- EDX=%x\r\n", task->regs.edx);
                kprint("    --- ESI=%x\r\n", task->regs.esi);
                kprint("    --- EDI=%x\r\n", task->regs.edi);
                kprint("    --- ESP=%x\r\n", task->regs.esp);
                kprint("    --- EBP=%x\r\n", task->regs.ebp);
                kprint("    --- EIP=%x\r\n", task->regs.eip);
                kdelay(1000);
        }
}
