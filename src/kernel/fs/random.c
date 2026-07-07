#include <fs/random.h>
#include <panic.h>
#include <string.h>
#include <mm/alloc.h>

static uint32_t rng_state = 0xDEADBEEF;

static uint32_t xorshift32(void)
{
        uint32_t x = rng_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        rng_state = x;
        return x;
}

static void mix_entropy(void)
{
        static uint32_t counter = 0;
        extern uint32_t ticks_since_boot;
        rng_state ^= counter++ ^ ticks_since_boot;
}

static int read(file_t *file, void *buf, size_t count)
{
        not_optional(buf);
        not_optional(file);
        mix_entropy();
        uint8_t *byte_buf = buf;
        for (unsigned long i = 0; i < count; i++)
        {
                if ((i & 3) == 0)
                {
                        uint32_t r = xorshift32();
                        byte_buf[i] = r & 0xFF;
                        byte_buf[i + 1] = (r >> 8) & 0xFF;
                        byte_buf[i + 2] = (r >> 16) & 0xFF;
                        byte_buf[i + 3] = (r >> 24) & 0xFF;
                }
        }

        return count;
}

static int write(file_t *file, const void *buf, size_t count)
{
        not_optional(buf);
        not_optional(file);
        const uint8_t *byte_buf = buf;
        for (unsigned long i = 0; i < count; i++)
        {
                rng_state ^= byte_buf[i] << ((i & 31) * 8);
        }
        return count;
}

static file_ops_t ops = {
        .read = read,
        .write = write,
        NULL,NULL,NULL,NULL,NULL,
};

vnode_t *CreateRandomDevice(char *name)
{
        not_optional(name);
        vnode_t *dir = vfs_create("/dev", name, 0);
        dir->ops  = &ops;
        return dir;
}

