
#pragma once
#include <jx.hpp>
#include <gdi/fb.hpp>

const char read_fb_info_fail[] = "could not read framebuffer information\r\n";

class Display
{
private:
        JX::File<> framebuffer;
        JX::File<> framebuffer_info;

        fb_t fb;

        void *back = NULL;
public:
        Display() : framebuffer("/dev/fb"), framebuffer_info("/dev/fb-info")
        {
                int l = framebuffer_info.read(&fb, sizeof(fb));
                if (l < 0)
                {
                        JX::stdout.write(read_fb_info_fail, sizeof(read_fb_info_fail));
                        ::exit(1);
                }

                back = ::malloc(fb.p * fb.h * (fb.bpp >> 3));
        }

        void plot(size_t x, size_t y, int colour)
        {
                if (x >= fb.w || y >= fb.h)
                        return;
                framebuffer.lseek(y * fb.w + x);
                framebuffer.write(&colour, fb.bpp >> 3);
        }
};
