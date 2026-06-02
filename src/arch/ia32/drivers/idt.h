#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <io.h>

#define IDT_ENTRIES 256

typedef struct __attribute__((__packed__))
{
        uint16_t base_low;
        uint16_t selector;
        uint8_t always_0;
        uint8_t flags;
        uint16_t base_high;
} idt_entry_t;

typedef struct __attribute__((__packed__))
{
        uint16_t limit;
        uint32_t base;
} idtp_t;

void idt_init(void);
void default_int(void);
void timer_int(void);

#endif
