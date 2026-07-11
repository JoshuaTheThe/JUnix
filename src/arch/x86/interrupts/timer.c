#include <interrupts/timer.h>
#include <cpu/io.h>
#include <cpu/cpu.h>
#include <dbg.h>
#include <panic.h>

extern volatile unsigned long ticks_since_boot;

void kdelay(unsigned long ticks)
{
        if (ticks == 0)
                return;
        uint32_t flags = save_flags();
        cpu_di();
        unsigned long start = ticks_since_boot;
        cpu_ei();
        while ((ticks_since_boot - start) < ticks)
        {
                __asm volatile("hlt");
        }
    
        restore_flags(flags);
}

void timer_init(unsigned int targetFreq)
{
        if (targetFreq == 0)
        {
                LOG(" [pit] targetFreq cannot be 0\r\n");
                panic(PANIC_DIVIDE_BY_ZERO);
                return;
        }
    
        if (targetFreq > 1193180)
        {
                LOG(" [pit] targetFreq %d too high (max 1193180)\r\n", targetFreq);
                targetFreq = 1193180;
        }
        if (targetFreq < 18)
        {
                LOG(" [pit] targetFreq %d too low (min ~18)\r\n", targetFreq);
                targetFreq = 18;
        }
    
        unsigned int divisor = 1193180 / targetFreq;
        if (divisor > 0xFFFF)
        {
                divisor = 0xFFFF;
                LOG(" [pit] Adjusted divisor to 0xFFFF\r\n");
        }
        if (divisor < 1)
        {
                divisor = 1;
                LOG(" [pit] Adjusted divisor to 1\r\n");
        }
        LOG(" [pit] Frequency %d Hz, divisor %d\r\n", targetFreq, divisor);
        outb(PIT_COMMAND, 0x36);
        outb(PIT_CHANNEL0, divisor & 0xFF);
        outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void disable_timer(void)
{
        uint8_t mask = inb(PIC1_DATA);
        mask |= (1 << IRQ_TIMER);
        outb(PIC1_DATA, mask);
}

void enable_timer(void)
{
        uint8_t mask = inb(PIC1_DATA);
        mask &= ~(1 << IRQ_TIMER);
        outb(PIC1_DATA, mask);
}
