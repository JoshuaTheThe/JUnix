
#include <arch.h>
#include <stdint.h>
#include <io.h>
#include <drivers/gdt.h>
#include <drivers/idt.h>
#include <drivers/pit.h>
#include <panic.h>
#include <boot/multiboot.h>

#include <features/feature.h>
#include <drivers/pde.h>

enum cpu_vendor
{
        CPU_INTEL,
        CPU_AMD,
        CPU_UNKNOWN
};

enum cpu_vendor arch_cpu_vendor(void)
{
        uint32_t eax, ebx, ecx, edx;
        __asm__("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
        if (ebx == 0x756E6547 && edx == 0x49656E69 && ecx == 0x6C65746E)
                return CPU_INTEL;
        if (ebx == 0x68747541 && edx == 0x69746E65 && ecx == 0x444D4163)
                return CPU_AMD;
        return CPU_UNKNOWN;
}

int arch_temp_mc(void)
{
        enum cpu_vendor vendor = arch_cpu_vendor();

        if (vendor == CPU_INTEL)
        {
                uint32_t eax, edx;
                __asm__("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0x1A2));
                uint32_t tjmax = (eax >> 16) & 0xFF;
                __asm__("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0x19C));
                uint32_t temp_offset = (eax >> 16) & 0x7F;

                if (temp_offset == 0)
                        return -1;
                return (tjmax - temp_offset) * 1000;
        }

        if (vendor == CPU_AMD)
        {
                // AMD Family 17h+ (Ryzen)
                uint32_t eax, edx;
                __asm__("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0xC0010400));
                if (!(eax & 0x1))
                        return -1;
                uint32_t temp = (eax >> 21) & 0x7FF;
                return temp * 125;
        }

        return -1;
}

void arch_init(void)
{
        cli();
        PDEInit();
        FeaturesInit();
        gdt_init();
        idt_init();
        pit_init(250);
        cli();
}

char *arch_identify(void)
{
        static char *Architecture = " [Info] ISA: Intel Architecture / 32,x86, CPU: i386\n";
        return Architecture;
}

void pause(void)
{
        __asm volatile("pause");
        __asm volatile("hlt");
}

void cli(void)
{
        __asm volatile("cli");
}

void sti(void)
{
        __asm volatile("sti");
}
