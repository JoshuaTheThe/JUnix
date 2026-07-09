
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
        kprint(" [krnl] %d bytes\r\n", l);
        bool suc = false;

        cpu_di();
        pid_t pid = elfLoadProgram(buffer, l, &suc, 0, 1, argv);
        if (!suc)
        {
                kprint(" [krnl] failed to open elf program\r\n");
                panic(PANIC_TODO);
        }

        kprint(" [krnl] executed /mnt/init on pid %d (waow) (%d bytes)\r\n", pid, l);
        kfree(buffer);
        close(fd);
        cpu_ei();
        while(1)
                cpu_halt();
}
