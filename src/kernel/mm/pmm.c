
#include <mm/pmm.h>
#include <string.h>
#include <panic.h>

extern uint8_t mem_start[];
extern uint8_t page_bitmap[];

size_t pmm_mstat(void)
{
        size_t free = 0;
        for (size_t i = 0; i < TOTAL_BITMAP; i++)
        {
                size_t byte = i / 8;
                size_t bit = i % 8;
                if (!(page_bitmap[byte] & (1 << bit)))
                {
                        free += 1;
                }
        }

        return free;
}

int pmm_next(void)
{
        for (size_t i = 0; i < TOTAL_BITMAP; i++)
        {
                size_t byte = i / 8;
                size_t bit = i % 8;
                if (!(page_bitmap[byte] & (1 << bit)))
                {
                        page_bitmap[byte] |= (1 << bit);
                        return i;
                }
        }
        return -1;
}

void pmm_free(void *page)
{
        uintptr_t addr = (uintptr_t)page;
        size_t index = (addr - (uintptr_t)&mem_start) / PAGE_SIZE;
        size_t byte = index / 8;
        size_t bit = index % 8;
        page_bitmap[byte] &= ~(1 << bit);
}

void *pmm_alloc(void)
{
        int index = pmm_next();
        if (index == -1)
                panic(PANIC_RAN_OUT_OF_MEMORY);
        void *page = (void*)((uintptr_t)&mem_start + (index * PAGE_SIZE));
        memset(page, 0, PAGE_SIZE);
        return page;
}
