#include <cpu/idt.h>
#include <interrupts/sys.h>
#include <cpu/cpu.h>

static idt_entry_t idt[256];
static idtp_t idtp;

void idt_set(uint8_t n, void *handler, idt_entry_t idtEntries[static 256])
{
        idtEntries[n].base_low = (uint32_t)handler & 0xFFFF;
        idtEntries[n].base_high = ((uint32_t)handler >> 16) & 0xFFFF;
        idtEntries[n].selector = 8;
        idtEntries[n].always_0 = 0;
        idtEntries[n].flags = 0x8E;
}

void idt_init(void)
{
        for (uint64_t i = 0; i < IDT_ENTRIES; ++i)
        {
                idt_set((uint8_t)i, (void *)default_int, idt);
        }

        idt_set(0x20, (void *)timer_int, idt);
        idt_set(0x2e, (void *)ide_int, idt);
        idt_set(0x2f, (void *)ide_int, idt);
        
        idt_set(0x80, (void *)sys_wrapper, idt);
        idtp.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
        idtp.base = (uint32_t)idt;
        __asm volatile ("lidt (%0)" : : "r"(&idtp));

        /* PIC Remap */
        outb(0x20, 0x11);
        outb(0xA0, 0x11);
        outb(0x21, 0x20);
        outb(0xA1, 0x28);
        outb(0x21, 0x04);
        outb(0xA1, 0x02);
        outb(0x21, 0x01);
        outb(0xA1, 0x01);
        outb(0x21, 0xFA);
        outb(0xA1, 0x3F);
}

