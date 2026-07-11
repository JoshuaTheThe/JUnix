
#ifndef CPU_H
#define CPU_H

#include<stddef.h>
#include<stdint.h>

void cpu_init(void);
char *cpu_get_vendor(void); // e.g. intel
char *cpu_get_arch(void);   // x86
int32_t cpu_get_temp_mc(void);

static inline void cpu_halt(void)  { __asm volatile("hlt");   }
static inline void cpu_pause(void) { __asm volatile("pause"); cpu_halt(); }
static inline void cpu_di(void)    { __asm volatile("cli");   }
static inline void cpu_ei(void)    { __asm volatile("sti");   }

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

#endif
