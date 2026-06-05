
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <drivers/fb/fb.h>
#include <sched/core.h>
#include <fs/fs.h>
#include <fs/ramfs/ramfs.h>
#include <mm/bitmap.h>
#include <mm/alloc.h>
#include <arch.h>
#include <string.h>
#include <panic.h>

void init(int m, uintptr_t a)
{
        cli();
        arch_init();
        if (vfs_init() != 0 || vfs_init_dev_mnt() != 0)
                panic(PANIC_TODO);
        serial_init();
        scheduler_init();
        fb_init(m,a);
}

