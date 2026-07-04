#ifndef _FB_H
#define _FB_H

#include <stdint.h>
#include <fs/fs.h>

typedef struct
{
        uint64_t base;
        uint32_t w;
        uint32_t h;
        uint8_t bpp;
        uint32_t p;
} fb_t;

void fb_init(int magic, uintptr_t addr);

#endif
