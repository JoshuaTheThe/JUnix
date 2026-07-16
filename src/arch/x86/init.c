
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <drivers/fb/fb.h>
#include <sched/core.h>
#include <fs/fs.h>
#include <fs/null.h>
#include <fs/random.h>
#include <fs/ramfs/ramfs.h>
#include <mm/pmm.h>
#include <mm/alloc.h>
#include <string.h>
#include <panic.h>
#include <drivers/storage/ide.h>
#include <drivers/pci.h>
#include <cpu/features/feature.h>
#include <cpu/cpu.h>
#include <drivers/rtc.h>
#include <fs/fat/fat.h>
#include <elf.h>
#include <dbg.h>
#include <version.h>

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

        vfs_create("/dev", "pci", 0);
        // "special interprocess communication fs" which is just ramfs, create a file and pass the path to a created process
        // i can implement linux like in future, but i just want something that works
        filesystem_t *rfs = get_file_system("ramfs");
        vnode_t *ipcnode = vfs_create("/", "ipc", 0);
        if (vfs_mount("/ipc", rfs, ipcnode) < 0)
                panic(PANIC_TODO);
        pciEnumerateDevices(pciRegister);
        CreateNullDevice("null");
        CreateRandomDevice("random");
        
        const size_t devcnt = pciGetDeviceCount();
        pci_device_t *dev = pciGetOriginalDevice(0);
        for (size_t i = 0; i < devcnt; ++i)
        {
                if (dev[i].class_id == 0x01 && dev[i].subclass_id == 0x01)
                {
                        IDEState.Dev = &dev[i];
                        IDEFind(i);

                        for (int d = 0; d < 4; d++)
                        {
                                if (IDEState.IDEDev[d].Reserved != 1)
                                        continue;
                                LOG(" [krnl] model=%s type=%d size=%dMB\r\n", IDEState.IDEDev[d].Model, IDEState.IDEDev[d].Type, (IDEState.IDEDev[d].Size * 512) / 1024 / 1024);
                        }
                }
        }

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
