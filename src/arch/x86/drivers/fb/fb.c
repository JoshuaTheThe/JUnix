#include <drivers/fb/fb.h>
#include <stdint.h>
#include <stddef.h>
#include <mm/alloc.h>
#include <panic.h>
#include <string.h>
#include <dbg.h>

struct multiboot_tag
{
        uint32_t type;
        uint32_t size;
};

struct multiboot_tag_framebuffer
{
        uint32_t type;          // 8
        uint32_t size;
        uint64_t framebuffer_addr;
        uint32_t framebuffer_pitch;
        uint32_t framebuffer_width;
        uint32_t framebuffer_height;
        uint8_t framebuffer_bpp;
        uint8_t framebuffer_type;  // 0=Indexed, 1=RGB, 2=EGA text
        uint8_t reserved[14];          // Padding for alignment
        
        struct
        {
                uint8_t red_position;
                uint8_t red_mask_size;
                uint8_t green_position;
                uint8_t green_mask_size;
                uint8_t blue_position;
                uint8_t blue_mask_size;
        } __attribute__((packed)) color_info;
} __attribute__((packed));

static int fb_read(file_t *file, void *buf, size_t count)
{
        fb_t *fb = (fb_t *)file->vnode->priv;
        if (!fb || !buf || count == 0)
                return -1;
        size_t fb_size = fb->p * fb->h;
        if (count > fb_size)
                count = fb_size;
        memcpy(buf, &((char *)fb->base)[file->offset], count);
        file->offset += count;
        return count;
}

static int fb_write(file_t *file, const void *buf, size_t count)
{
        fb_t *fb = (fb_t *)file->vnode->priv;
        if (!fb || !buf || count == 0)
                return -1;
        size_t fb_size = fb->p * fb->h;
        if (count > fb_size-file->offset)
                count = fb_size;
        memcpy(&((char *)fb->base)[file->offset], buf, count);
        file->offset += count;
        return count;
}

static int fb_info_read(file_t *file, void *buf, size_t count)
{
        fb_t *fb = (fb_t *)file->vnode->priv;
        if (!fb || !buf)
                return -1;
        if (count > sizeof(*fb))
                count = sizeof(*fb);
        memcpy(buf, fb, count);
        return count;
}

static int fb_info_write(file_t *file, const void *buf, size_t count)
{
        (void)file;
        (void)buf;
        (void)count;
        return -1;
}

static file_ops_t fb_ops =
{
        .length = NULL,
        .mkdir = NULL,
        .readdir = NULL,
        .read = fb_read,
        .write = fb_write,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
};

static file_ops_t fb_info_ops =
{
        .length = NULL,
        .mkdir = NULL,
        .readdir = NULL,
        .read = fb_info_read,
        .write = fb_info_write,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
};

void fb_init(int magic, uintptr_t addr)
{
        LOG(" [fb] magic: %x, addr = %x\r\n", magic, addr);
        if (magic != 0x36d76289)
                panic(PANIC_INCORRECT_BOOTLOADER);
        
        /*
         * Map grub shit
         */
        for (uintptr_t addr = 0x00100000;
             addr < 0x00200000;
             addr += PAGE_SIZE)
        {
                paging_map(&kernel_address_space, addr, addr, PAGE_WRITE);
        }

        unsigned int offset = 8;
        struct multiboot_tag *tag;
        while (1)
        {
                tag = (struct multiboot_tag *)(addr + offset);

                if (tag->type == 0)
                        break;
                LOG(" [fb] tag type: %d\r\n", tag->type);
                if (tag->type == 8)
                {
                        struct multiboot_tag_framebuffer *_fb = 
                                (struct multiboot_tag_framebuffer *)tag;
                        uint64_t base   = _fb->framebuffer_addr;
                        uint32_t width  = _fb->framebuffer_width;
                        uint32_t height = _fb->framebuffer_height;
                        uint8_t bpp     = _fb->framebuffer_bpp;
                        uint32_t pitch  = _fb->framebuffer_pitch;

                        vnode_t *fb_vn = vfs_create("/dev", "fb", 0);
                        fb_vn->ops = &fb_ops;
                        fb_vn->priv = kmalloc(sizeof(fb_t));
                        if (!fb_vn->priv)
                                panic(PANIC_TODO);
                        fb_t *fb = fb_vn->priv;
                        fb->base = base;
                        fb->w = width;
                        fb->h = height;
                        fb->bpp = bpp;
                        fb->p = pitch;
                        
                        vnode_t *info_vn = vfs_create("/dev", "fb-info", 0);
                        info_vn->ops = &fb_info_ops;
                        info_vn->priv = fb;
                        LOG(" [fb] Framebuffer initialized: %dx%d@%d bpp, base=0x%x\r\n",
                               width, height, bpp, base);
                        break;
                }

                offset += (tag->size + 7) & ~7;
        }

        for (uintptr_t addr = 0x00100000;
             addr < 0x00200000;
             addr += PAGE_SIZE)
        {
                paging_unmap(&kernel_address_space, addr);
        }

        LOG(" [fb] OK");
}
