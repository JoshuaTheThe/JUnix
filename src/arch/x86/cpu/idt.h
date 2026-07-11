#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <cpu/io.h>

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
void ide_int(void);
void divide_error(void);
void divide_error_handler(void);
void debug_exception(void);
void debug_exception_handler(void);
void nmi_exception(void);
void nmi_exception_handler(void);
void breakpoint_exception(void);
void breakpoint_exception_handler(void);
void overflow_exception(void);
void overflow_exception_handler(void);
void bound_exception(void);
void bound_exception_handler(void);
void invalid_opcode(void);
void invalid_opcode_handler(void);
void device_not_available(void);
void device_not_available_handler(void);
void double_fault(void);
void double_fault_handler(void);
void invalid_tss(void);
void invalid_tss_handler(void);
void segment_not_present(void);
void segment_not_present_handler(void);
void stack_fault(void);
void stack_fault_handler(void);
void general_protection(void);
void general_protection_handler(void);
void page_fault(void);
void page_fault_handler(void);
void x87_exception(void);
void x87_exception_handler(void);
void alignment_check(void);
void alignment_check_handler(void);
void machine_check(void);
void machine_check_handler(void);
void simd_exception(void);
void simd_exception_handler(void);

#endif
