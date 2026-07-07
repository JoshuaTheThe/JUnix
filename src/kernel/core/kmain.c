
#include <libk.h>
#include <cpu/cpu.h>
#include <drivers/kprint.h>
#include <sched/core.h>

void kmain(void)
{
        cpu_ei();
        int fd = open("/dev/serial", 0);
        if (fd < 0)
                panic(PANIC_TODO);
        write(fd, "Hello, World!\r\n", 15);
        close(fd);
        while(1)
                cpu_pause();
}
