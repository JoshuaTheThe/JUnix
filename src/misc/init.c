
#include <stdio.h>

int main(void)
{
        int fd = open("/dev/serial", 0);
        if (fd < 0)
        {
                return 1;
        }

        write(fd, "Здравствуйте, Мир!\r\n", 35);
        close(fd);
        return 0;
}
