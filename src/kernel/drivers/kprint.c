
#include <drivers/serial.h>
#include <drivers/kprint.h>
#include <stdarg.h>
#include <stddef.h>
#include <panic.h>

static void itoh(uint32_t value, char *buffer)
{
        // UNSAFE
        static const char hex_digits[] = "0123456789abcdef";
        not_optional(buffer);

        for (int i = 7; i >= 0; i--)
        {
                buffer[i] = hex_digits[value & 0xF];
                value >>= 4;
        }
        // UNSAFE
        buffer[8] = '\0';
}

static void puthex(uint32_t value)
{
        char hex_buffer[9];
        int start = 0;
        itoh(value, hex_buffer);
        // UNSAFE
        while (start < 7 && hex_buffer[start] == '0')
        {
                start++;
        }

        for (int i = start; i < 8; i++)
        {
                // UNSAFE
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
                                int i = 0;

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
                                        while (num > 0)
                                        {
                                                // UNSAFE
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
