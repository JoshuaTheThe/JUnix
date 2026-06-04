
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

void create_b_main(void)
{
        kprint("Hello, World!\r\n");
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
        arch_init();
        cli();
        vfs_init();
        serial_init();
        scheduler_init();
        filesystem_t ramfs = ramfs_create_fs();
        if (vfs_mount("/mnt", &ramfs, NULL) != 0)
        {
                kprint("could not create ramfs");
                panic(PANIC_TODO);
        }

        vnode_t *mnt;
        if (vfs_lookup("/mnt", &mnt) != 0)
        {
                kprint("could not create in ramfs");
                panic(PANIC_TODO);
        }

        create_b();
        sti();
        vfs_mkdir(mnt, "test", 0);
        vnode_t *root = root_vnode;
        list(root, 0);

        file_t *f;
        if (vfs_open("/dev/serial", &f) != 0)
        {
                kprint("could not find");
                panic(PANIC_TODO);
        }

        vfs_write(f, "Hello, VFS World!\r\n", 19);
        vfs_close(f);
        kprint("created ramfs\r\n");
        while(true);
        panic(PANIC_TODO);
}
