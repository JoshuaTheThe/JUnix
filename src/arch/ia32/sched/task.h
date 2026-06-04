#ifndef TASK_H
#define TASK_H

#include <stdint.h>

typedef struct
{
        uint32_t eax,ecx,edx,ebx;
        uint32_t esp,ebp,esi,edi;
        uint32_t eip,cs,es,ds;
        uint32_t ss,fs,gs,eflags;
} task_state_registers_t;

#endif
