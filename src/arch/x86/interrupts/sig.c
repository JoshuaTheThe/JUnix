
#include <stdint.h>
#include <drivers/kprint.h>

void divide_error_handler(void)
{
        kprint(" [error] cant divide by zero\r\n");
}

void debug_exception_handler(void)
{
        kprint(" [error] debug exception\r\n");
}

void nmi_exception_handler(void)
{
        kprint(" [error] nmi exception\r\n");
}

void breakpoint_exception_handler(void)
{
        kprint(" [error] breakpoint exception\r\n");
}

void overflow_exception_handler(void)
{
        kprint(" [error] overflow exception\r\n");
}

void bound_exception_handler(void)
{
        kprint(" [error] bounds exception\r\n");
}

void invalid_opcode_handler(void)
{
        kprint(" [error] invalid opcode\r\n");
}

void device_not_available_handler(void)
{
        kprint(" [error] device not available\r\n");
}

void double_fault_handler(void)
{
        kprint(" [error] double fault\r\n");
}

void invalid_tss_handler(void)
{
        kprint(" [error] invalid tss\r\n");
}

void segment_not_present_handler(void)
{
        kprint(" [error] segment not present\r\n");
}

void stack_fault_handler(void)
{
        kprint(" [error] stack fault\r\n");
}

void general_protection_handler(void)
{
        kprint(" [error] general protection fault\r\n");
}

void page_fault_handler(int cs, int eip)
{
        uintptr_t fault_addr;
        asm volatile("mov %%cr2, %0" : "=r"(fault_addr));
        kprint(" [error] page fault: %x, @%x:%x\r\n", fault_addr, cs, eip);
}

void x87_exception_handler(void)
{
        kprint(" [error] x87 exception\r\n");
}

void alignment_check_handler(void)
{
        kprint(" [error] unaligned exception\r\n");
}

void machine_check_handler(void)
{
        kprint(" [error] machine check exception\r\n");
}

void simd_exception_handler(void)
{
        kprint(" [error] simd exception\r\n");
}
