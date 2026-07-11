
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
        int fd = open("/mnt/hello.txt", 0);
        if (fd < 0)
                panic(PANIC_TODO);
        read(fd, buffer, 2048);
        kprint("%s\r\n", buffer);
        kfree(buffer);
        close(fd);
        panic(PANIC_TODO);
}
