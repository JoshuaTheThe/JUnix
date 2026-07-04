
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <stdarg.h>
#include <stddef.h>
#include <panic.h>

static void itoh(uint32_t value, char buffer[static 9], size_t bufsize)
{
        static const char hex_digits[16] = "0123456789abcdef";
        if (bufsize < 2)
        {
                if (bufsize > 0) buffer[0] = '\0';
                return;
        }
        
        int digits = 0;
        uint32_t temp = value;
        do
        {
                digits++;
                temp >>= 4;
        } while (temp != 0 && digits < 8);
        
        if (digits < 2) digits = 2;
        int max_digits = (bufsize < 9) ? (int)bufsize - 1 : 8;
        if (digits > max_digits) digits = max_digits;
        
        int start_pos = digits - 1;
        for (int i = start_pos; i >= 0; i--)
        {
                buffer[i] = hex_digits[value & 0xF];
                value >>= 4;
        }
        
        buffer[digits] = '\0';
}

static void puthex(uint32_t value)
{
        char hex_buffer[9] = {0};
        itoh(value, hex_buffer, sizeof(hex_buffer));
        for (size_t i = 0; hex_buffer[i] != '\0'; i++)
        {
                serial_putchar(hex_buffer[i]);
        }
}

void kprint(const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        not_optional(fmt);
        while (*fmt)
        {
                if (*fmt == '%')
                {
                        // UNSAFE
                        fmt++;
                        if (*fmt == 'c')
                        {
                                char c = (char)va_arg(args, int);
                                serial_putchar(c);
                        }
                        else if (*fmt == 's')
                        {
                                // UNSAFE
                                char *str = va_arg(args, char *);
                                while (*str)
                                {
                                        serial_putchar(*str);
                                        // UNSAFE
                                        str++;
                                }
                        }
                        else if (*fmt == 'd')
                        {
                                int num = va_arg(args, int);
                                char buffer[20] = {0};
                                size_t i = 0;

                                if (num < 0)
                                {
                                        serial_putchar('-');
                                        num = -num;
                                }
                                if (num == 0)
                                {
                                        // UNSAFE
                                        buffer[i++] = '0';
                                }
                                else
                                {
                                        while (num > 0 && i < sizeof(buffer) - 1)
                                        {
                                                buffer[i++] = (num % 10) + '0';
                                                num /= 10;
                                        }
                                }
                                for (int j = i - 1; j >= 0; j--)
                                {
                                        // UNSAFE
                                        serial_putchar(buffer[j]);
                                }
                        }
                        else if (*fmt == 'x')
                        {
                                uint32_t num = va_arg(args, uint32_t);
                                puthex(num);
                        }
                }
                else
                {
                        serial_putchar(*fmt);
                }
                // UNSAFE
                fmt++;
        }

        va_end(args);
}
