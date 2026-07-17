
#include <drivers/pci/pci.h>
#include <fs/fs.h>
#include <string.h>
#include <mm/alloc.h>
#include <panic.h>
#include <math.h>
#include <mm/paging.h>
#include <dbg.h>

pci_device_t devices[MAX_DEVICES];
uint32_t device_count = 0;

uint32_t pciGetDeviceCount(void)
{
        return device_count;
}

size_t pciGetDevices(pci_device_t *destination, size_t start, size_t end)
{
        size_t count = 0;
        for (size_t i = start; i <= end && i < device_count; ++i)
        {
                destination[count] = devices[i];
                count++;
        }
        return count;
}

pci_device_t *pciGetOriginalDevice(size_t Index)
{
        if (Index >= MAX_DEVICES)
                return NULL;
        return &devices[Index];
}


const char *pciClassToFileName(uint8_t class_id, uint8_t subclass_id)
{
        switch (class_id)
        {
        case 0x00:
                if (subclass_id == 0x01)
                        return "lvga";
                return "unknown";

        case 0x01:
                switch (subclass_id)
                {
                case 0x00:
                        return "scsi";
                case 0x01:
                        return "ide";
                case 0x02:
                        return "floppy";
                case 0x03:
                        return "ipi";
                case 0x04:
                        return "raid";
                case 0x05:
                        return "ata";
                case 0x06:
                        return "sata";
                case 0x07:
                        return "sas";
                case 0x80:
                        return "mass";
                default:
                        return "unknown";
                }

        case 0x02:
                switch (subclass_id)
                {
                case 0x00:
                        return "eth";
                case 0x01:
                        return "token";
                case 0x02:
                        return "fddi";
                case 0x03:
                        return "atm";
                case 0x04:
                        return "isdn";
                case 0x05:
                        return "worldfip";
                case 0x06:
                        return "picmg";
                case 0x80:
                        return "network";
                default:
                        return "unknown";
                }

        case 0x03:
                switch (subclass_id)
                {
                case 0x00:
                        return "vga";
                case 0x01:
                        return "xga";
                case 0x02:
                        return "vga3d";
                case 0x80:
                        return "display";
                default:
                        return "unknown";
                }

        case 0x06:
                switch (subclass_id)
                {
                case 0x00:
                        return "host";
                case 0x01:
                        return "isa";
                case 0x02:
                        return "eisa";
                case 0x03:
                        return "mca";
                case 0x04:
                        return "pcipci";
                case 0x05:
                        return "pcmcia";
                case 0x06:
                        return "nubus";
                case 0x07:
                        return "cardbus";
                case 0x08:
                        return "raceway";
                case 0x09:
                        return "pcipcisemi";
                case 0x0A:
                        return "infinibandpci";
                case 0x80:
                        return "bridge";
                default:
                        return "unknown";
                }

        case 0x0C:
                switch (subclass_id)
                {
                case 0x00:
                        return "firewire";
                case 0x01:
                        return "access";
                case 0x02:
                        return "ssa";
                case 0x03:
                        return "usb";
                case 0x04:
                        return "fibre";
                case 0x05:
                        return "sm";
                case 0x06:
                        return "infiniband";
                case 0x07:
                        return "ipmi";
                case 0x08:
                        return "sercos";
                case 0x09:
                        return "canbus";
                default:
                        return "unknown";
                }

        default:
                return "unknown";
        }
}

const char *pciClassToString(uint8_t class_id, uint8_t subclass_id)
{
        switch (class_id)
        {
        case 0x00:
                if (subclass_id == 0x01)
                        return "VGA Compatible (Legacy)";
                return "Unclassified";

        case 0x01:
                switch (subclass_id)
                {
                case 0x00:
                        return "SCSI Controller";
                case 0x01:
                        return "IDE Controller";
                case 0x02:
                        return "Floppy Disk Controller";
                case 0x03:
                        return "IPI Bus Controller";
                case 0x04:
                        return "RAID Controller";
                case 0x05:
                        return "ATA Controller";
                case 0x06:
                        return "SATA Controller";
                case 0x07:
                        return "SAS Controller";
                case 0x80:
                        return "Other Mass Storage Controller";
                default:
                        return "Unknown Storage Controller";
                }

        case 0x02:
                switch (subclass_id)
                {
                case 0x00:
                        return "Ethernet Controller";
                case 0x01:
                        return "Token Ring Controller";
                case 0x02:
                        return "FDDI Controller";
                case 0x03:
                        return "ATM Controller";
                case 0x04:
                        return "ISDN Controller";
                case 0x05:
                        return "WorldFip Controller";
                case 0x06:
                        return "PICMG Controller";
                case 0x80:
                        return "Other Network Controller";
                default:
                        return "Unknown Network Controller";
                }

        case 0x03:
                switch (subclass_id)
                {
                case 0x00:
                        return "VGA Compatible Controller";
                case 0x01:
                        return "XGA Controller";
                case 0x02:
                        return "3D Controller (Not VGA-Compatible)";
                case 0x80:
                        return "Other Display Controller";
                default:
                        return "Unknown Display Controller";
                }

        case 0x06:
                switch (subclass_id)
                {
                case 0x00:
                        return "Host Bridge";
                case 0x01:
                        return "ISA Bridge";
                case 0x02:
                        return "EISA Bridge";
                case 0x03:
                        return "MCA Bridge";
                case 0x04:
                        return "PCI-to-PCI Bridge";
                case 0x05:
                        return "PCMCIA Bridge";
                case 0x06:
                        return "NuBus Bridge";
                case 0x07:
                        return "CardBus Bridge";
                case 0x08:
                        return "RACEway Bridge";
                case 0x09:
                        return "PCI-to-PCI Bridge (Semi-transparent)";
                case 0x0A:
                        return "InfiniBand-to-PCI Host Bridge";
                case 0x80:
                        return "Other Bridge";
                default:
                        return "Unknown Bridge";
                }

        case 0x0C:
                switch (subclass_id)
                {
                case 0x00:
                        return "FireWire (IEEE 1394) Controller";
                case 0x01:
                        return "ACCESS Bus";
                case 0x02:
                        return "SSA";
                case 0x03:
                        return "USB Controller";
                case 0x04:
                        return "Fibre Channel";
                case 0x05:
                        return "SMBus";
                case 0x06:
                        return "InfiniBand";
                case 0x07:
                        return "IPMI Interface";
                case 0x08:
                        return "SERCOS Interface";
                case 0x09:
                        return "CANbus Controller";
                default:
                        return "Unknown Serial Bus Controller";
                }

        default:
                return "Unknown Device";
        }
}

static inline uint32_t pciConfigReadDword(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
        uint32_t address = ((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)func << 8) |
                           ((uint32_t)offset & 0xFC) | 0x80000000;
        outl(PCI_CONFIG_ADDRESS, address);
        return inl(PCI_CONFIG_DATA);
}

static inline void pciConfigWriteDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value)
{
        uint32_t address = (bus << 16) | (slot << 11) | (func << 8) |
                           (offset & 0xFC) | 0x80000000;
        outl(PCI_CONFIG_ADDRESS, address);
        outl(PCI_CONFIG_DATA, value);
}

static inline uint16_t pciConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
        uint32_t data = pciConfigReadDword(bus, slot, func, offset);
        return (uint16_t)((data >> ((offset & 2) * 8)) & 0xFFFF);
}

static inline void pciConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t value)
{
        uint32_t old = pciConfigReadDword(bus, slot, func, offset);
        int shift = (offset & 2) * 8;
        uint32_t mask = 0xFFFF << shift;
        uint32_t data = (old & ~mask) | ((value & 0xFFFF) << shift);
        pciConfigWriteDword(bus, slot, func, offset, data);
}

static inline uint8_t pciConfigReadByte(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
        uint32_t data = pciConfigReadDword(bus, slot, func, offset);
        return (uint8_t)((data >> ((offset & 3) * 8)) & 0xFF);
}

__attribute__((__used__))
static inline void pciConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t value)
{
        uint32_t old = pciConfigReadDword(bus, slot, func, offset);
        int shift = (offset & 3) * 8;
        uint32_t mask = 0xFF << shift;
        uint32_t data = (old & ~mask) | ((value & 0xFF) << shift);
        pciConfigWriteDword(bus, slot, func, offset, data);
}

static inline int pciDeviceExists(uint16_t bus, uint8_t slot, uint8_t function)
{
        return pciConfigReadWord(bus, slot, function, 0x00) != 0xFFFF;
}

static inline void pciReadDeviceInfo(pci_device_t *dev, uint16_t bus, uint8_t slot, uint8_t func)
{
        dev->bus = (uint8_t)bus;
        dev->slot = slot;
        dev->function = func;
        dev->vendor_id = pciConfigReadWord(bus, slot, func, 0x00);
        dev->device_id = pciConfigReadWord(bus, slot, func, 0x02);
        dev->revision = pciConfigReadByte(bus, slot, func, 0x08);
        dev->prog_if = pciConfigReadByte(bus, slot, func, 0x09);
        dev->subclass_id = pciConfigReadByte(bus, slot, func, 0x0A);
        dev->class_id = pciConfigReadByte(bus, slot, func, 0x0B);
        dev->header_type = pciConfigReadByte(bus, slot, func, 0x0E);
        dev->irq_line = pciConfigReadByte(bus, slot, func, 0x3C);

        for (int i = 0; i < 6; i++)
        {
                uint32_t bar_val = pciConfigReadDword(bus, slot, func, (uint8_t)(0x10 + i * 4));
                dev->bar[i] = bar_val;
        }
}

__attribute__((__used__))
static inline void pciEnableBusMastering(pci_device_t *dev)
{
        uint16_t command = pciConfigReadWord(dev->bus, dev->slot, dev->function, 0x04);
        command |= (1 << 2); // Bus mastering enable bit
        pciConfigWriteWord(dev->bus, dev->slot, dev->function, 0x04, command);
}

size_t pciGetBARSize(pci_device_t *dev, int bar)
{
        if (bar < 0 || bar >= 6)
                return 0;

        uint8_t offset = 0x10 + (bar * 4);

        uint32_t original_low =
                pciConfigReadDword(dev->bus, dev->slot, dev->function, offset);

        if (original_low == 0)
                return 0;

        /* I/O BAR */
        if (original_low & 1)
        {
                pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                                    offset, 0xFFFFFFFF);

                uint32_t mask =
                    pciConfigReadDword(dev->bus, dev->slot, dev->function,
                                       offset);

                pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                                    offset, original_low);

                mask &= ~0x3;

                return (~mask) + 1;
        }

        /* Memory BAR */
        uint32_t type = (original_low >> 1) & 0x3;
        if (type == 0x2)
        {
                if (bar == 5)
                        return 0;

                uint32_t original_high =
                    pciConfigReadDword(dev->bus, dev->slot, dev->function,
                                       offset + 4);

                pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                                    offset, 0xFFFFFFFF);
                pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                                    offset + 4, 0xFFFFFFFF);

                uint32_t mask_low =
                    pciConfigReadDword(dev->bus, dev->slot, dev->function,
                                       offset);

                uint32_t mask_high =
                    pciConfigReadDword(dev->bus, dev->slot, dev->function,
                                       offset + 4);

                pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                                    offset, original_low);
                pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                                    offset + 4, original_high);

                uint64_t mask = ((uint64_t)mask_high << 32) | mask_low;

                mask &= ~0xFULL;

                return (size_t)((~mask) + 1);
        }

        /* 32-bit memory BAR */
        pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                            offset, 0xFFFFFFFF);

        uint32_t mask =
            pciConfigReadDword(dev->bus, dev->slot, dev->function,
                               offset);

        pciConfigWriteDword(dev->bus, dev->slot, dev->function,
                            offset, original_low);

        mask &= ~0xF;

        return (~mask) + 1;
}

void pciEnumerateDevices(void (*on_device_found)(pci_device_t *))
{
        for (uint16_t bus = 0; bus < 256; bus++)
        {
                for (uint8_t slot = 0; slot < 32; slot++)
                {
                        // Always check function 0 first
                        if (!pciDeviceExists(bus, slot, 0))
                                continue;

                        uint8_t header_type = pciConfigReadByte(bus, slot, 0, 0x0E);
                        uint8_t func_limit = (header_type & 0x80) ? 8 : 1;

                        for (uint8_t func = 0; func < func_limit; func++)
                        {
                                if (!pciDeviceExists(bus, slot, func))
                                        continue;

                                pci_device_t dev;
                                pciReadDeviceInfo(&dev, bus, slot, func);
                                on_device_found(&dev);
                        }
                }
        }
}

void pciDisplayDeviceInfo(pci_device_t *dev)
{
        const char *className = pciClassToString(dev->class_id, dev->subclass_id);

        LOG(" [pci] PCI Device @ Bus %d, Slot %d, Function %d\r\n", dev->bus, dev->slot, dev->function);
        LOG(" [pci]   Vendor ID: 0x%x\r\n", dev->vendor_id);
        LOG(" [pci]   Device ID: 0x%x\r\n", dev->device_id);
        LOG(" [pci]   Class: 0x%x (%s)\r\n", dev->class_id, className);
        LOG(" [pci]   Subclass: 0x%x\r\n", dev->subclass_id);
        LOG(" [pci]   Programming Interface: 0x%x\r\n", dev->prog_if);
        LOG(" [pci]   Revision: 0x%x\r\n", dev->revision);
        LOG(" [pci]   Header Type: 0x%x\r\n", dev->header_type);
        LOG(" [pci]   IRQ Line: %d\r\n", dev->irq_line);

        for (int i = 0; i < 6; i++)
        {
                if (dev->bar[i])
                        LOG(" [pci] BAR[%d]: 0x%x\r\n", i, dev->bar[i]);
        }
}

static int read(file_t *file, void *buf, size_t i)
{
        int a = min(i, sizeof(pci_device_t));
        not_optional(file);
        not_optional(buf);
        memcpy(buf, ((pci_device_t *)file->vnode->private), a);
        return a;
}

static file_ops_t fil = {
        .read = read,
};


void pciRegister(pci_device_t *dev)
{
        pciDisplayDeviceInfo(dev);
        const char *className = pciClassToString(dev->class_id, dev->subclass_id);
        devices[device_count++] = *dev;
        LOG(" [pci] Registered Device of class %s on irq %d\r\n", className, dev->irq_line);
        for (int i = 0; i < 6; i++)
        {
                uint32_t bar = dev->bar[i];
                if (!bar)
                        continue;
                /* I/O BAR */
                if (bar & 1)
                        continue;
                uintptr_t phys = bar & ~0xF;
                size_t size = pciGetBARSize(dev, i);
                for (uintptr_t off = 0; off < size; off += PAGE_SIZE)
                        paging_map(&kernel_address_space, phys + off, phys + off, PAGE_WRITE);
                LOG(" [pci] Registering BAR%d, ((char *)0x%x)[%d]\r\n", i, dev->bar[i], size);
        }

        vnode_t *node = vfs_create("/dev/pci",
                                (char *)
                                pciClassToFileName(dev->class_id,
                                                   dev->subclass_id),
                                VFS_DIRECTORY);
        node->ops = &fil;
        node->private = dev;
}

pci_device_t *pciFindOfType(uint8_t class_id, uint8_t subclass_id)
{
        for (int i = 0; i < MAX_DEVICES; ++i)
        {
                if (devices[i].class_id == class_id && devices[i].subclass_id == subclass_id)
                {
                        return &devices[i];
                }
        }
        return NULL;
}
