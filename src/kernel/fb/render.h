#ifndef _RENDER_H
#define _RENDER_H

#include <drivers/fb/fb.h>
#include <fb/font.h>
#include <stddef.h>
#include <stdint.h>

void fb_putchar(char chr, fb_t *fb, font_t *font, size_t x, size_t y,
                uint32_t fg, uint32_t bg);

void fb_puts(const char *str, fb_t *fb, font_t *font, size_t x, size_t y,
             uint32_t fg, uint32_t bg);

void fb_clear(fb_t *fb, uint32_t color);
void fb_draw_rect(fb_t *fb, size_t x, size_t y, size_t w, size_t h,
                  uint32_t color);
void fb_draw_pixel(fb_t *fb, size_t x, size_t y, uint32_t color);

#endif
