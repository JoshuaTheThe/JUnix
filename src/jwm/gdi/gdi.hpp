
#pragma once
#include <jx.hpp>
#include <gdi/fb.hpp>

class Display
{
private:
        JX::File<char> framebuffer;
        JX::File<fb_t> framebuffer_info;

        fb_t fb;
public:
        Display() : framebuffer("/dev/fb"), framebuffer_info("/dev/fb-info")
        {
                if (framebuffer_info.read(&fb, 1) != 1)
                {}
        }
};
