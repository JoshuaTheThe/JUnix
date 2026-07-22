
#include <stdio.h>

int main(void)
{
        int fd;
        if ((fd = open("/dev/serial", 0)) < 0)
                return 1;
        write(fd, "HI\r\n", 4);
        close(fd);
        return 0;
}
