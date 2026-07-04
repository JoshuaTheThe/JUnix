
#include <string.h>
#include <panic.h>
#include <mm/alloc.h>

int itoa(char *dest, int n, int base, bool uppercase)
{
        if (dest == NULL) return 0;
        if (base < 2 || base > 36)
        {
                dest[0] = '\0';
                return 0;
        }
    
        if (n == 0)
        {
                dest[0] = '0';
                dest[1] = '\0';
                return 1;
        }
    
        bool is_negative = (base == 10 && n < 0);
        unsigned int num = is_negative ? (unsigned int)(-n) : (unsigned int)n;
        char temp[64];
        int i = 0;
        while (num > 0)
        {
                int digit = num % base;
                if (digit < 10)
                {
                        temp[i++] = '0' + digit;
                }
                else
                {
                        temp[i++] = uppercase ? 
                                'A' + (digit - 10) : 
                                'a' + (digit - 10);
                }
                num /= base;
        }
    
        if (is_negative)
        {
                temp[i++] = '-';
        }
    
        int j = 0;
        while (i > 0)
        {
                dest[j++] = temp[--i];
        }
        dest[j] = '\0';
        return j;
}

char *UlToString(unsigned long Number)
{
        // each byte requires at most three digits
        char *const String = kmalloc(sizeof(Number) * 8);
        unsigned long i;
        for (i = 0; i < sizeof(Number) * 3; ++i)
        {
                String[i] = (Number % 10) + '0';
                Number /= 10;
                if (Number == 0) break;
        }

        for (unsigned long j = 0; j < i; ++j)
        {
                char Temporary = String[j];
                String[j] = String[i - j];
                String[i - j] = Temporary;
        }

        return String;
}

void memset(void *p, int v, long l)
{
        unsigned char *base = (unsigned char *)p;
        not_optional(base);
        for (int i = 0; i < l; ++i)
        {
                base[i] = (uint8_t)v; // why is std impl as int when it writes bytes... idk
        }
}

int strncmp(const char *const Lhs, const char *const Rhs, unsigned long Len)
{
        not_optional(Lhs);
        not_optional(Rhs);
        for (unsigned long i = 0; i < Len; i++)
        {
                if (Lhs[i] != Rhs[i] || Lhs[i] == '\0')
                {
                        return Lhs[i] - Rhs[i];
                }
        }
        return 0;
}

int strcmp(const char *const Lhs, const char *const Rhs)
{
        not_optional(Lhs);
        not_optional(Rhs);
        for (unsigned long i = 0;; i++)
        {
                if (Lhs[i] != Rhs[i] || Lhs[i] == '\0')
                {
                        return Lhs[i] - Rhs[i];
                }
        }
        return 0;
}

int strnlen(const char *const A, unsigned long Len)
{
        not_optional(A);
        for (unsigned long i = 0; i < Len; ++i)
        {
                if (A[i])
                        continue;
                return i;
        }

        return Len;
}

int strlen(const char *const A)
{
        not_optional(A);
        for (unsigned long i = 0;; ++i)
        {
                if (A[i])
                        continue;
                return i;
        }
}

void memcpy(void *Destination, const void *const Source, const unsigned long Len)
{
        not_optional(Destination);
        not_optional(Source);
        for (unsigned long i = 0; i < Len; ++i)
        {
                ((char *)Destination)[i] = ((const char *const)Source)[i];
        }
}

void strncpy(void *Destination, const void *const Source, const unsigned long Len)
{
        char       *Dst = (char *)Destination;
        const char *Src = (const char *)Source;
        unsigned long i = 0;
        while (i < Len && Src[i] != 0)
        {
                Dst[i] = Src[i];
                ++i;
        }
        while (i < Len)
        {
                Dst[i] = 0;
                ++i;
        }
}
