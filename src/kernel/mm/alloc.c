
#include <mm/alloc.h>
#include <mm/paging.h>
#include <mm/pmm.h>
#include <dbg.h>
#include <string.h>
#include <panic.h>

typedef struct
{
        size_t size;
        size_t pages;
} block_header_t;

#define KHEAP_START 0xD0000000
#define KHEAP_END   0xE0000000

static uintptr_t heap_next = KHEAP_START;

uintptr_t vmalloc(size_t size)
{
        size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        uintptr_t addr = heap_next;
        heap_next += size;
        if (heap_next >= KHEAP_END)
                return 0;
        return addr;
}

void vfree(uintptr_t v, size_t size)
{
        (void)v;
        (void)size;
        return;
}

__attribute__((ownership_returns(__kmalloc))) void *__kmalloc(size_t size, const char *FILE, long LINE)
{
        (void)LINE;
        (void)FILE;
        if (size == 0)
                return NULL;
        size_t total = size + sizeof(block_header_t);
        size_t pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;
        uintptr_t virt = vmalloc(pages * PAGE_SIZE);
        if (!virt)
                panic(PANIC_RAN_OUT_OF_MEMORY);
        for (size_t i = 0; i < pages; i++)
        {
                uintptr_t phys = (uintptr_t)virt_to_phys(pmm_alloc());
                if (!phys)
                {
                        for (size_t j = 0; j < i; j++)
                        {
                                uintptr_t old_phys =
                                        virt_to_phys((void *)(virt + j * PAGE_SIZE));
                                paging_unmap(virt + j * PAGE_SIZE);
                                pmm_free((void *)old_phys);
                        }

                        panic(PANIC_RAN_OUT_OF_MEMORY);
                }

                paging_map(
                        virt + i * PAGE_SIZE,
                        phys,
                        PAGE_PRESENT | PAGE_WRITE
                );
        }

        block_header_t *header = (void *)virt;
        header->size = size;
        header->pages = pages;
        memset(
                (void *)(virt + sizeof(block_header_t)),
                0,
                size
        );

        LOG(" [kmalloc] allocating %x[%d] %s:%d\r\n", virt + sizeof(block_header_t), size, FILE, LINE);
        return (void *)(virt + sizeof(block_header_t));
}

__attribute__((ownership_takes(__kmalloc, 1))) void __kfree(void *ptr, const char *FILE, long LINE)
{
        (void)LINE;
        (void)FILE;
        if (!ptr)
                return;
        LOG(" [kfree] freeing %x %s:%d\r\n", ptr, FILE, LINE);
        block_header_t *header =
        (block_header_t *)((uintptr_t)ptr - sizeof(block_header_t));
        uintptr_t virt = (uintptr_t)header;
        size_t pages = header->pages;
        for (size_t i = 0; i < pages; i++)
        {
                uintptr_t page = virt + i * PAGE_SIZE;
                uintptr_t phys = virt_to_phys((void *)page);

                paging_unmap(page);
                pmm_free((void *)phys);
        }

        vfree(virt, pages * PAGE_SIZE);
}
