
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
#include <drivers/ide/ide.h>
#include <drivers/pci/pci.h>
#include <features/feature.h>

void init(int m, uintptr_t a)
{
        cli();
        if (vfs_init() != 0 || vfs_init_dev_mnt() != 0)
                panic(PANIC_TODO);
        serial_init();
        arch_init();
        scheduler_init();
        fb_init(m,a);
        pciEnumerateDevices(pciRegister);

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
                                kprint(" [krnl] model=%s type=%d size=%dMB\r\n", IDEState.IDEDev[d].Model, IDEState.IDEDev[d].Type, (IDEState.IDEDev[d].Size * 512) / 1024 / 1024);
                        }
                }
        }
}

