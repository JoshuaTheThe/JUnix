
#include <drivers/storage/ide.h>
#include <init/pci.h>
#include <fs/fs.h>

bool pci_init(void)
{
        vfs_create("/dev", "pci", 0);
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
                                LOG(" [krnl] model=%s type=%d size=%dMB\r\n", IDEState.IDEDev[d].Model, IDEState.IDEDev[d].Type, (IDEState.IDEDev[d].Size * 512) / 1024 / 1024);
                        }
                }
        }

        return true;
}
