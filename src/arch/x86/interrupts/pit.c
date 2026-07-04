#include <interrupts/pit.h>
#include <cpu/io.h>
#include <arch.h>
#include <drivers/kprint.h>
#include <panic.h>

extern volatile unsigned long ticks_since_boot;

static inline uint32_t save_flags(void)
{
        uint32_t flags;
        __asm volatile("pushfl; popl %0" : "=r"(flags) : : "memory");
        return flags;
}

static inline void restore_flags(uint32_t flags)
{
        __asm volatile("pushl %0; popfl" : : "r"(flags) : "memory", "cc");
}

void kdelay(unsigned long ticks)
{
        if (ticks == 0)
                return;
        uint32_t flags = save_flags();
        cli();
        unsigned long start = ticks_since_boot;
        sti();
        while ((ticks_since_boot - start) < ticks)
        {
                __asm volatile("hlt");
        }
    
        restore_flags(flags);
}

void pit_init(unsigned int targetFreq)
{
        if (targetFreq == 0)
        {
                kprint(" [krnl!] PIT: targetFreq cannot be 0\r\n");
                panic(PANIC_DIVIDE_BY_ZERO);
                return;
        }
    
        if (targetFreq > 1193180)
        {
                kprint(" [krnl!] PIT: targetFreq %d too high (max 1193180)\r\n", targetFreq);
                targetFreq = 1193180;
        }
        if (targetFreq < 18)
        {
                kprint(" [krnl!] PIT: targetFreq %d too low (min ~18)\r\n", targetFreq);
                targetFreq = 18;
        }
    
        unsigned int divisor = 1193180 / targetFreq;
        if (divisor > 0xFFFF)
        {
                divisor = 0xFFFF;
                kprint(" [krnl] PIT: Adjusted divisor to 0xFFFF\r\n");
        }
        if (divisor < 1)
        {
                divisor = 1;
                kprint(" [krnl] PIT: Adjusted divisor to 1\r\n");
        }
        kprint(" [krnl] PIT: Frequency %d Hz, divisor %d\r\n", targetFreq, divisor);
        outb(PIT_COMMAND, 0x36);
        outb(PIT_CHANNEL0, divisor & 0xFF);
        outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}
