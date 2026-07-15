
#include <sys/sys.h>
#include <sys/signal.h>
#include <string.h>
#include <mm/paging.h>
#include <dbg.h>

bool address_is_user(void *p)
{
        // simple check, above 0xc0000000 is kernel,
        // also last user page bcs weird cpu bugs
        if (current_proc->space != &kernel_address_space &&
            p >= (void*)0xbffff000)
                return false;
        return paging_validate_address(current_proc->space, (void *)((uintptr_t)p & ~0xFFF));
}

int copy_from_user(void *d, void *s, size_t n)
{
        if (!d || !s || n == 0)
                return -EINVAL;
        uint8_t *S = s;
        for (size_t i = 0; i < n; ++i)
        {
                if (!address_is_user(&S[i]))
                {
                        return -EFAULT;
                }
        }

        memcpy(d, s, n);
        return n;
}

int copy_to_user(void *d, void *s, size_t n)
{
        if (!d || !s || n == 0)
                return -EINVAL;
        uint8_t *D = d;
        for (size_t i = 0; i < n; ++i)
        {
                if (!address_is_user(&D[i]))
                {
                        return -EFAULT;
                }
        }

        memcpy(d, s, n);
        return n;
}

size_t user_strnlen(char *p, size_t n)
{
        size_t i;
        for (i=0;
             address_is_user(&p[i]) && p[i] && n > 0;
             ++i,--n);

        return i;
}
