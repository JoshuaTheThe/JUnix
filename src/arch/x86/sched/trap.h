#ifndef TRAP_H
#define TRAP_H

#include <cpu/io.h>

__attribute__((__used__))
static void ackint(void)
{
        outb(0x20, 0x20);
}

#endif
