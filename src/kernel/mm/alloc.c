
#include <mm/alloc.h>
#include <panic.h>

void *kalloc(size_t siz)
{
        if (siz == 0)
                return NULL;
        size_t total = siz + sizeof(block_header_t);
        size_t PagesNeeded = (total + PAGE_SIZE - 1) / PAGE_SIZE;
        void *base = pagealloc(); // warning - currently unsafe due to memory fragmentation, will be replaced with virtual handler in future
        if (!base)
        {
                panic(PANIC_RAN_OUT_OF_MEMORY);
        }

        block_header_t *Header = (block_header_t *)base;
        Header->siz = total;
        Header->nxt = NULL;
        for (size_t i = 1; i < PagesNeeded; ++i)
        {
                void *nxtPage = pagealloc();
                if (!nxtPage)
                {
                        for (size_t j = 0; j < i; ++j)
                        {
                                pagealloc_free((char *)base + j * PAGE_SIZE);
                        }
                        panic(PANIC_RAN_OUT_OF_MEMORY);
                }
        }

        return (char *)base + sizeof(block_header_t);
}

void kfree(void *base)
{
        not_optional(base);
        block_header_t *Header = (block_header_t *)((char *)base - sizeof(block_header_t));
        size_t total = Header->siz;
        size_t used = (total + PAGE_SIZE - 1) / PAGE_SIZE;
        void *start = (char *)base - sizeof(block_header_t);
        for (size_t i = 0; i < used; ++i)
        {
                pagealloc_free((char *)start + i * PAGE_SIZE);
        }
}
