
#include <stdint.h>
#include <cpu/io.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <interrupts/timer.h>
#include <panic.h>
#include <boot/multiboot.h>
#include <cpu/features/feature.h>
#include <cpu/cpu.h>
#include <string.h>
#include <mm/alloc.h>
#include <mm/paging.h>

enum cpu_vendor
{
        CPU_INTEL,
        CPU_AMD,
        CPU_UNKNOWN
};

static enum cpu_vendor cpu_get_raw_vendor(void)
{
        uint32_t eax, ebx, ecx, edx;
        __asm__("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
        if (ebx == 0x756E6547 && edx == 0x49656E69 && ecx == 0x6C65746E)
                return CPU_INTEL;
        if (ebx == 0x68747541 && edx == 0x69746E65 && ecx == 0x444D4163)
                return CPU_AMD;
        return CPU_UNKNOWN;
}

int32_t cpu_get_temp_mc(void)
{
        enum cpu_vendor vendor = cpu_get_raw_vendor();

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

char *cpu_get_vendor(void)
{
        char *names[] = {"Intel", "Advanced Micro Devices", "Unknown Vendor"};
        return names[cpu_get_raw_vendor()];
}

char *cpu_get_arch(void)
{
        return "x86";
}

void cpu_init(void)
{
        FeaturesInit();
        paging_init();
        gdt_init();
        idt_init();
        timer_init(250);
}
