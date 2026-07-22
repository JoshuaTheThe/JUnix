
#ifndef PANIC_H
#define PANIC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <drivers/serial.h>
#include <fs/fs.h>

#define panic(Code) panic_impl(__FILE__, __LINE__, Code, #Code, PANIC_CLASS_SUPERVISOR)
#define not_optional(e) do { if ((e) == NULL) panic(PANIC_NULL_POINTER_DEREFERENCE); } while (0)
#define assert(e) do { if (!(e)) panic(PANIC_FAILED_ASSERT); } while (0)

typedef enum
{
        PANIC_TODO,
        PANIC_RAN_OUT_OF_MEMORY,
        PANIC_NULL_POINTER_DEREFERENCE,
        PANIC_UNINTENDED_CALL,
        PANIC_TRACE_OVERFLOW,
        PANIC_NOT_FOUND,
        PANIC_SEGMENTATION_FAULT,
        PANIC_DOUBLE_OPEN,
        PANIC_FD_NOT_FOUND,
        PANIC_INCORRECT_BOOTLOADER,
        PANIC_UNHANDLED_INTERRUPT,
        PANIC_OVERHEAT,
        PANIC_UNSUPPORTED_FS_OP,
        PANIC_FAILED_ASSERT,
        PANIC_NO_PAGE_DIR,
        PANIC_NO_PAGE,
        PANIC_DOUBLE_VIRT_ALLOC,
        PANIC_CORRUPT_FS,
        PANIC_REQUIRED_FEATURE,
        PANIC_DIVIDE_BY_ZERO,
        PANIC_PAGE_FAULT,
} panic_code_t;

typedef enum
{
        PANIC_CLASS_SUPERVISOR,  // Kernel bug (no user involvement)
        PANIC_CLASS_USERSPACE,   // Kernel bug triggered by user process (e.g. bad address)
} panic_class_t;

void panic_impl(const char *const File, long Line, panic_code_t Code, const char *const CodeAsStr, panic_class_t Class);
void list(vnode_t *node, size_t depth);

#endif
