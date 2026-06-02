
#include <mm/bitmap.h>
#include <string.h>
#include <panic.h>

extern uint8_t MemoryStart[];
uint8_t PageBitmap[TOTAL_BITMAP / 8] = {0};

size_t pagealloc_mstat(void)
{
        size_t Free = 0;
        for (size_t i = 0; i < TOTAL_BITMAP; i++)
        {
                size_t byte = i / 8;
                size_t bit = i % 8;
                if (!(PageBitmap[byte] & (1 << bit)))
                {
                        Free += 1;
                }
        }

        return Free;
}

int pagealloc_nextfree(void)
{
        for (size_t i = 0; i < TOTAL_BITMAP; i++)
        {
                size_t byte = i / 8;
                size_t bit = i % 8;
                if (!(PageBitmap[byte] & (1 << bit)))
                {
                        PageBitmap[byte] |= (1 << bit);
                        return i;
                }
        }
        return -1;
}

void pagealloc_free(void *page)
{
        uintptr_t addr = (uintptr_t)page;
        size_t index = (addr - (uintptr_t)&MemoryStart) / PAGE_SIZE;
        size_t byte = index / 8;
        size_t bit = index % 8;
        PageBitmap[byte] &= ~(1 << bit);
}

void *pagealloc(void)
{
        int index = pagealloc_nextfree();
        if (index == -1)
                panic(PANIC_RAN_OUT_OF_MEMORY);
        void *page = (void*)((uintptr_t)&MemoryStart + (index * PAGE_SIZE));
        memset(page, 0, PAGE_SIZE);
        return page;
}
