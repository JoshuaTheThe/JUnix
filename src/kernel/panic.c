
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <panic.h>
#include <arch.h>
#include <string.h>

_Noreturn void panic_impl(const char *const File, long Line, panic_code_t Code, const char *const CodeAsStr, panic_class_t Class)
{
        cli(); // disable interrupts and say, context switching timers for given architecture
        kprint("\r\n -- KERNEL PANIC VIA %s IN PROCESS %d -- \r\n",
                    Class == PANIC_CLASS_SUPERVISOR ? "SUPERVISOR" : "USERSPACE", 0);
        kprint("Kernel Panic Function %x (%s) was raised\r\n", Code, CodeAsStr);
        #ifdef HAS_TEMPERATURE
        kprint("Temperature x1000: %d\r\n", ArchGetTemperatureMC());
        #endif
        if (File)
                kprint("Source Location: %s:%d\r\n", File, Line);
        kprint("Trace Dump:\r\n");
        while (true)
                pause();
}
