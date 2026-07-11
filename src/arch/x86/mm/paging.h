
#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>

#define PAGE_SIZE (4096)

#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITE    (1 << 1)
#define PAGE_USER     (1 << 2)
#define PAGE_PWT      (1 << 3)
#define PAGE_PCD      (1 << 4)
#define PAGE_ACCESSED (1 << 5)
#define PAGE_DIRTY    (1 << 6)
#define PAGE_4MB      (1 << 7)
#define PAGE_GLOBAL   (1 << 8)

#define PAGE_DIR_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TAB_INDEX(x) (((x) >> 12) & 0x3FF)
#define PAGE_OFFSET(x)    ((x) & 0xFFF)

typedef uint32_t pte_t;
typedef uint32_t pde_t;

typedef struct
{
        uint32_t phys, virt;
} mapping_t;

typedef struct
{
        pte_t entries[1024];
} page_table_t;

typedef struct
{
        pde_t entries[1024];
} page_directory_t;

void      paging_map(uintptr_t virt, uintptr_t phys, uint32_t flags);
void      paging_unmap(uintptr_t virt);
uintptr_t virt_to_phys(void *);
void     *phys_to_virt(uintptr_t);
void      paging_switch(page_directory_t *);
void      paging_init(void);
void      paging_enable(void);
void      paging_disable(void);

#endif
