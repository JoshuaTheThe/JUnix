
#pragma once
#include <stdint.h>

typedef struct
{
        uint64_t __base; // kernel only
        uint32_t width;
        uint32_t height;
        uint8_t  bpp;
        uint32_t pitch;
} fb_t;
