
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>

#define TOTAL_BITMAP (1024 * 1024)
#define PAGE_SIZE (4096)

void pagealloc_init(uintptr_t StartAddress, size_t MemorySize);
int pagealloc_nextfree(void);
void pagealloc_free(void *page);
void *pagealloc(void);
size_t pagealloc_mstat(void);

#endif

