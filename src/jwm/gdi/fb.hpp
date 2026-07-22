
#pragma once
#include <stdint.h>

typedef struct
{
        uint64_t _; // kernel only
        uint32_t w;
        uint32_t h;
        uint8_t  bpp;
        uint32_t p;
} fb_t;
