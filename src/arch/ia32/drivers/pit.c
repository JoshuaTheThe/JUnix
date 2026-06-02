
#include <drivers/pit.h>
#include <io.h>

void kdelay(unsigned long ticks)
{
        extern unsigned long ticks_since_boot;
        unsigned int start = ticks_since_boot;
        while ((ticks_since_boot - start) < ticks)
        {
                __asm volatile("pause");
        }
}

void pit_init(unsigned int targetFreq)
{
        unsigned int divisor = 1193180 / targetFreq;
        outb(PIT_COMMAND, 0x36);
        outb(PIT_CHANNEL0, divisor & 0xFF);
        outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}
