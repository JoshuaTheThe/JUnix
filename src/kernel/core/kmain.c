
#include <libk.h>

void kmain(void)
{
        int fd = open("/dev/serial", 0);
        if (fd < 0)
                panic(PANIC_FD_NOT_FOUND);
        write(fd, "hi\r\n", 4);
        panic(PANIC_TODO);
}
