
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <drivers/fb/fb.h>
#include <drivers/pci/pci.h>
#include <drivers/rtc.h>

#include <sched/core.h>

#include <fs/fs.h>
#include <fs/null.h>
#include <fs/random.h>
#include <fs/ramfs/ramfs.h>
#include <fs/fat/fat.h>

#include <mm/pmm.h>
#include <mm/alloc.h>

#include <cpu/features/feature.h>
#include <cpu/cpu.h>

#include <elf.h>
#include <dbg.h>
#include <version.h>
#include <string.h>
#include <panic.h>

#include <init/db.h>
#include <init/pci.h>

void init(int m, uintptr_t a)
{
        kprint("J/Unix kernel build %d\r\n", JUNIX_BUILD);
        LOG("-- Пойехали! --\r\n\r\n");
        cpu_di();
        cpu_init();
        if (vfs_init() != 0 || vfs_init_dev_mnt() != 0)
                panic(PANIC_TODO);
        sched_init();
        serial_init();
        fb_init(m,a);
        rtcInit();
        if (!db_init())
                panic(PANIC_TODO);

        // "special interprocess communication fs" which is just ramfs, create a file and pass the path to a created process
        // i can implement linux like in future, but i just want something that works
        filesystem_t *rfs = get_file_system("ramfs");
        vnode_t *ipcnode = vfs_create("/", "ipc", 0);
        if (vfs_mount("/ipc", rfs, ipcnode) < 0)
                panic(PANIC_TODO);

        CreateNullDevice("null");
        CreateRandomDevice("random");

        if (!pci_init())
                panic(PANIC_TODO);

        filesystem_t *fs = get_file_system("fat");
        vnode_t *node;
        if (vfs_lookup("/dev/ide0p0", &node) < 0)
                panic(PANIC_TODO);
        if (vfs_mount("/mnt", fs, node) < 0)
                panic(PANIC_TODO);

        file_t *rtc;
        if (vfs_open("/dev/rtc", &rtc) < 0)
        {
                LOG(" [krnl] no rtc present!\r\n");
                return;
        }

        rtcTime_t time;
        if ((size_t)vfs_read(rtc, &time, sizeof(time)) < sizeof(time))
        {
                LOG(" [krnl] failed to read the rtc\r\n");
                return;
        }

        LOG(" [krnl] time is: %d/%d/%d %d:%d:%d\r\n",
                time.day,
                time.month,
                time.year,
                time.hour,
                time.minute,
                time.second);
        vfs_close(rtc);
}
