
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <mm/bitmap.h>
#include <arch.h>
#include <string.h>
#include <panic.h>

void kmain(void)
{
        arch_init();
        serial_init();
        kprint("Hello, World!");
        sti();
        panic(PANIC_TODO);
}
