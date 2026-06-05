
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <sched/core.h>
#include <fs/fs.h>
#include <fs/ramfs/ramfs.h>
#include <mm/bitmap.h>
#include <mm/alloc.h>
#include <arch.h>
#include <string.h>
#include <panic.h>
#include <drivers/fb/fb.h>
#include <fb/render.h>

void create_b_main(void)
{
        kprint(" [b   ] Hello, World!\r\n");
        while(1)
                ;
}

void create_b(void)
{
        task_state_registers_t regs = {0};
        regs.eip = (uintptr_t)create_b_main;
        regs.ebp = 0;
        regs.esp = (uintptr_t)kmalloc(1024) + 1020;
        regs.cs  = 0x8;
        regs.ds  = 0x10;
        regs.es  = 0x10;
        regs.fs  = 0x10;
        regs.gs  = 0x10;
        regs.ss  = 0x10;
        regs.eflags  = 0x202;
        scheduler_add_process(regs, "b");
}

void kmain(void)
{
        create_b();
        sti();
        file_t *f;
        if (vfs_open("/dev/fb-info", &f) != 0)
        {
                kprint(" [krnl] could not find /dev/fb-info");
                panic(PANIC_TODO);
        }

        fb_t info = {0};
        vfs_read(f, &info, sizeof(info));

        fb_clear(&info, 0xFFFFFFFF);
        fb_puts(" [krnl] Hello, World!\n",
                &info,
                &font_8x8,
                0, 0,
                0x00000000,0xFFFFFFFF);

        vfs_close(f);
        panic(PANIC_TODO);
}
