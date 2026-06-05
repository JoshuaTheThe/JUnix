#include <fb/render.h>
#include <string.h>

void fb_putchar(char chr,
                fb_t *fb,
                font_t *font,
                size_t x, size_t y,
                uint32_t fg, uint32_t bg)
{
        if (!fb || !font || !font->font_bitmap)
                return;
                
        if (chr < font->first_char || chr > font->last_char)
                return;
        
        size_t char_index = chr - font->first_char;
        size_t bytes_per_char = (font->char_width * font->char_height + 7) / 8;
        size_t offset = char_index * bytes_per_char;
        
        uint8_t bytes_per_pixel = fb->bpp / 8;
        if (bytes_per_pixel == 0) return;
        
        for (size_t row = 0; row < font->char_height; row++)
        {
                if (y + row >= fb->h) break;
                unsigned char bitmap_row = font->font_bitmap[offset + row];
                for (size_t col = 0; col < font->char_width; col++)
                {
                        if (x + col >= fb->w) break;
                        if (bitmap_row & (1 << (7 - col)))
                        {
                                uint32_t *pixel = (uint32_t *)(fb->base + 
                                            ((y + row) * fb->p) + 
                                            ((x + col) * bytes_per_pixel));
                                *pixel = fg;
                        }
                        else if (bg != (uint32_t)-1)
                        {
                                uint32_t *pixel = (uint32_t *)(fb->base + 
                                            ((y + row) * fb->p) + 
                                            ((x + col) * bytes_per_pixel));
                                *pixel = bg;
                        }
                }
        }
}

void fb_puts(const char *str,
             fb_t *fb,
             font_t *font,
             size_t x, size_t y,
             uint32_t fg, uint32_t bg)
{
        if (!str || !fb || !font) return;
        
        size_t current_x = x;
        size_t current_y = y;
        
        for (size_t i = 0; str[i] != '\0'; i++)
        {
                if (str[i] == '\r')
                {
                        current_x = x;
                        continue;
                }

                if (str[i] == '\n')
                {
                        current_x = x;
                        current_y += font->char_height;
                        continue;
                }
                
                fb_putchar(str[i], fb, font, current_x, current_y, fg, bg);
                current_x += font->char_width;
                if (current_x + font->char_width > fb->w)
                {
                        current_x = x;
                        current_y += font->char_height;
                }
                
                if (current_y + font->char_height > fb->h)
                        break;
        }
}

void fb_clear(fb_t *fb, uint32_t color)
{
        if (!fb) return;
        
        uint8_t bytes_per_pixel = fb->bpp / 8;
        size_t fb_size = fb->p * fb->h;
        
        for (size_t i = 0; i < fb_size; i += bytes_per_pixel)
        {
                *(uint32_t *)(fb->base + i) = color;
        }
}

void fb_draw_rect(fb_t *fb, size_t x, size_t y, size_t w, size_t h, uint32_t color)
{
        if (!fb) return;
        
        uint8_t bytes_per_pixel = fb->bpp / 8;
        
        for (size_t i = 0; i < w; i++)
        {
                if (x + i >= 0 && x + i < fb->w)
                {
                        if (y >= 0 && y < fb->h)
                        {
                                uint32_t *pixel = (uint32_t *)(fb->base + 
                                            (y * fb->p) + 
                                            ((x + i) * bytes_per_pixel));
                                *pixel = color;
                        }
                        if (y + h - 1 >= 0 && y + h - 1 < fb->h)
                        {
                                uint32_t *pixel = (uint32_t *)(fb->base + 
                                            ((y + h - 1) * fb->p) + 
                                            ((x + i) * bytes_per_pixel));
                                *pixel = color;
                        }
                }
        }
        
        for (size_t i = 0; i < h; i++)
        {
                if (y + i >= 0 && y + i < fb->h)
                {
                        if (x >= 0 && x < fb->w)
                        {
                                uint32_t *pixel = (uint32_t *)(fb->base + 
                                            ((y + i) * fb->p) + 
                                            (x * bytes_per_pixel));
                                *pixel = color;
                        }
                        if (x + w - 1 >= 0 && x + w - 1 < fb->w)
                        {
                                uint32_t *pixel = (uint32_t *)(fb->base + 
                                            ((y + i) * fb->p) + 
                                            ((x + w - 1) * bytes_per_pixel));
                                *pixel = color;
                        }
                }
        }
}

void fb_draw_pixel(fb_t *fb, size_t x, size_t y, uint32_t color)
{
        if (!fb) return;
        if (x < 0 || x >= fb->w || y < 0 || y >= fb->h) return;
        
        uint8_t bytes_per_pixel = fb->bpp / 8;
        uint32_t *pixel = (uint32_t *)(fb->base + (y * fb->p) + (x * bytes_per_pixel));
        *pixel = color;
}

