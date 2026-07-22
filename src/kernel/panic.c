
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <panic.h>
#include <cpu/cpu.h>
#include <string.h>

void list(vnode_t *node, size_t depth)
{
        if (!node)
                return;
        for (size_t i = 0; i < depth; ++i)
                kprint(" ");
        kprint("%s\r\n", node->name);
        list(node->children, depth+1);
        list(node->next, depth);
}

_Noreturn void panic_impl(const char *const File, long Line, panic_code_t Code, const char *const CodeAsStr, panic_class_t Class)
{
        cpu_di(); // disable interrupts and say, context switching timers for given architecture
        kprint("\r\n -- KERNEL PANIC VIA %s IN PROCESS %d -- \r\n",
                    Class == PANIC_CLASS_SUPERVISOR ? "SUPERVISOR" : "USERSPACE", 0);
        kprint("Kernel Panic Function %x (%s) was raised\r\n", Code, CodeAsStr);
        kprint("Temperature x1000: %d\r\n", cpu_get_temp_mc());
        if (File)
                kprint("Source Location: %s:%d\r\n", File, Line);
        list(root_vnode, 0);
        while (true)
                cpu_pause();
}
