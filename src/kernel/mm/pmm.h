
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <mm/paging.h>

#define TOTAL_BITMAP (1024 * 1024)

int pmm_next(void);
void pmm_free(void *page);
void *pmm_alloc(void);
size_t pmm_mstat(void);

#endif

