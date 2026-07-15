
#include <mm/paging.h>
#include <mm/pmm.h>
#include <mm/alloc.h>
#include <cpu/cpu.h>
#include <dbg.h>
#include <string.h>
#include <panic.h>

static bool paging_active = false;

mapping_t       kernel_mappings[MAX_KERNEL_MAPPINGS] = {0};
address_space_t kernel_address_space = {0};

static address_space_t *__space = NULL;

address_space_t *paging_get_address_space(void)
{
        return __space;
}

uintptr_t virt_to_phys(address_space_t *as, void *ptr)
{
        if ((uintptr_t)ptr >= 0xC0000000 &&
            (uintptr_t)ptr <  0xC1000000 && !paging_active)
        {
                return (uintptr_t)ptr - (0xC0000000 - 0x200000);
        }

        if (!paging_active)
                return (uintptr_t)ptr;
        uintptr_t virt = (uintptr_t)ptr;

        uint32_t dir = PAGE_DIR_INDEX(virt);
        uint32_t tab = PAGE_TAB_INDEX(virt);

        pde_t pde = as->pd->entries[dir];

        if (!(pde & PAGE_PRESENT))
                return 0;

        page_table_t *pt =
        phys_to_virt(as, pde & ~0xFFF);
        if (!pt)
                return 0;

        pte_t pte = pt->entries[tab];

        if (!(pte & PAGE_PRESENT))
                return 0;

        return (pte & ~0xFFF) + PAGE_OFFSET(virt);
}

void *phys_to_virt(address_space_t *as, uintptr_t addr)
{
        if (addr >= 0x200000 &&
            addr <  0x210000 && !paging_active)
        {
                return (void *)(addr + (0xC0000000 - 0x200000));
        }
        if (!paging_active)
                return (void *)addr;
        uintptr_t phys = addr & ~0xFFF;
        uintptr_t offset = addr & 0xFFF;
        for (size_t i = 0; i < as->count; i++)
        {
                mapping_t *m = &as->items[i];

                if (m->phys == phys)
                {
                        return (void *)(m->virt + offset);
                }
        }

        return NULL;
}

void paging_switch(address_space_t *as)
{
        __space = as;
        uintptr_t phys = virt_to_phys(as, as->pd);
        __asm volatile(
                "mov %0, %%cr3"
                :
                : "r"(phys)
                : "memory"
        );
}

void paging_disable(void)
{
        uint32_t cr0;
        __asm volatile(
                "mov %%cr0, %0"
                : "=r"(cr0)
        );

        cr0 &= ~0x80000000; // CR0.PG
        __asm volatile(
                "mov %0, %%cr0"
                :
                : "r"(cr0)
                : "memory"
        );
}

void paging_enable(void)
{
        uint32_t cr0;

        __asm volatile(
                "mov %%cr0, %0"
                : "=r"(cr0)
        );

        cr0 |= 0x80000000; // CR0.PG
        __asm volatile(
                "mov %0, %%cr0"
                :
                : "r"(cr0)
                : "memory"
        );
}

void paging_map(address_space_t *as, uintptr_t virt, uintptr_t phys, uint32_t flags)
{
        not_optional(as);
        not_optional(as->pd);
        not_optional(as->items);
        LOG(" [mm] mapping virt %x to phys %x\r\n", virt, phys);
        uint32_t dir = PAGE_DIR_INDEX(virt);
        uint32_t tbl = PAGE_TAB_INDEX(virt);

        page_table_t *new_pt = NULL;

        if (!(as->pd->entries[dir] & PAGE_PRESENT))
        {
                new_pt = pmm_alloc();
                memset(new_pt, 0, PAGE_SIZE);
        }

        uint32_t flags_saved = save_flags();
        cpu_di();

        if (new_pt)
        {
                uintptr_t pt_phys = virt_to_phys(as, new_pt);

                as->pd->entries[dir] =
                        pt_phys |
                        PAGE_PRESENT |
                        PAGE_WRITE;

                mapping_t *m =
                    &as->items[as->count++];

                m->phys = pt_phys & ~0xFFF;
                m->virt = (uintptr_t)new_pt & ~0xFFF;
        }

        page_table_t *pt =
                phys_to_virt(as, as->pd->entries[dir] & ~0xFFF);

        pt->entries[tbl] =
                (phys & ~0xFFF) |
                flags |
                PAGE_PRESENT;

        mapping_t *m =
            &as->items[as->count++];

        m->phys = phys & ~0xFFF;
        m->virt = virt & ~0xFFF;

        if (paging_active && as == __space)
                __asm volatile("invlpg (%0)" :: "r"(virt) : "memory");

        restore_flags(flags_saved);
}

bool paging_validate_address(address_space_t *as, void *p)
{
        not_optional(as);
        not_optional(as->pd);
        uint32_t dir = PAGE_DIR_INDEX(((uintptr_t)p));
        uint32_t tbl = PAGE_TAB_INDEX(((uintptr_t)p));
        if (!(as->pd->entries[dir] & PAGE_PRESENT)) // invalid dir
        {
                return false;
        }

        page_table_t *pt = phys_to_virt(as, as->pd->entries[dir] & ~0xFFF);
        return pt->entries[tbl] & PAGE_PRESENT ? true : false;
}

bool remove_mapping(address_space_t *as, uintptr_t virt)
{
        not_optional(as);
        virt &= ~0xFFF;
        for (size_t i = 0; i < as->count; i++)
        {
                if (as->items[i].virt == (virt & ~0xFFF))
                {
                        // Move the last entry over this one
                        as->items[i] = as->items[as->count - 1];
                        as->count--;
                        return true;
                }
        }

        return false;
}

void paging_unmap(address_space_t *as, uintptr_t virt)
{
        not_optional(as);
        uint32_t dir = PAGE_DIR_INDEX(virt);
        uint32_t tab = PAGE_TAB_INDEX(virt);
        pde_t pde = as->pd->entries[dir];
        if (!(pde & PAGE_PRESENT))
                return;
        uintptr_t phys = virt_to_phys(as, (void *)virt);
        LOG(" [mm] unmapping virt %x phys %x\r\n", virt, phys);
        remove_mapping(as, virt);
        page_table_t *pt =
            phys_to_virt(as, pde & ~0xFFF);
        pt->entries[tab] = 0;
        if (phys)
                pmm_free((void *)phys);
        if (paging_active && as == __space)
                __asm volatile("invlpg (%0)" :: "r"(virt) : "memory");
}

void paging_init(void)
{
        kernel_address_space.capacity = MAX_KERNEL_MAPPINGS;
        kernel_address_space.count    = 0;
        kernel_address_space.items    = kernel_mappings;
        kernel_address_space.pd       = pmm_alloc();
        mapping_t *m =
            &kernel_address_space.items[kernel_address_space.count++];
        m->phys = virt_to_phys(&kernel_address_space, kernel_address_space.pd) & ~0xFFF;
        m->virt = (uintptr_t)kernel_address_space.pd & ~0xFFF;
        memset(kernel_address_space.pd, 0, PAGE_SIZE);

        /*
         * Map kernel
         */
        LOG(" [mm] mapping 0xC0000000-0xC1000000 as 0x00200000 - 0x01000000\r\n");
        for (uintptr_t addr = 0x200000;
             addr < 0x1000000;
             addr += PAGE_SIZE)
        {
                paging_map(&kernel_address_space, addr + (0xC0000000 - 0x200000), addr, PAGE_WRITE);
        }
        
        /*
         * Load page directory
         */
        LOG(" [mm] loading page dir\r\n");
        paging_switch(&kernel_address_space);

        paging_active = true;
}

void paging_clear_address_space(address_space_t *as)
{
        while (as->count)
        {
                bool removed = false;

                for (size_t i = 0; i < as->count; i++)
                {
                        if (as->items[i].virt < 0xC0000000)
                        {
                                paging_unmap(as, as->items[i].virt);
                                removed = true;
                                break;
                        }
                }

                if (!removed)
                        break;
        }
}

/**
 * copy the virt address space, allocate new for phys, good for e.g. fork
 * execpt kernel, keep phys unchanged for 0xC000_0000 and above
 */
address_space_t paging_copy_space(address_space_t *as)
{
        address_space_t new = {0};
        new.capacity = as->count;
        if (as->capacity < MAX_KERNEL_MAPPINGS)
                return (address_space_t){0};
        new.count = 0;
        new.items = kmalloc(sizeof(mapping_t) * MAX_KERNEL_MAPPINGS);
        new.pd = pmm_alloc();
        memset(new.pd, 0, PAGE_SIZE);
        for (size_t i = PAGE_DIR_INDEX(0xC0000000); i < 1024; i++)
        {
                new.pd->entries[i] = as->pd->entries[i];
        }

        for (size_t i = 0; i < as->count; i++)
        {
                if (as->items[i].virt >= 0xC0000000)
                {
                        new.items[new.count++] = as->items[i];
                }
        }

        for (size_t i = 0; i < as->count; i++)
        {
                mapping_t *m = &as->items[i];
                if (m->virt >= 0xC0000000)
                        continue;
                const size_t old_phys = as->items[i].phys;
                const size_t new_phys = virt_to_phys(__space, pmm_alloc());
                char page[PAGE_SIZE];

                // Load data from old page
                paging_map(__space, TEMPORARY_PAGE, old_phys, PAGE_WRITE);
                memcpy(page, (void *)TEMPORARY_PAGE, PAGE_SIZE);
                paging_unmap(__space, TEMPORARY_PAGE);

                // Store data to new page
                paging_map(__space, TEMPORARY_PAGE, new_phys, PAGE_WRITE);
                memcpy((void *)TEMPORARY_PAGE, page, PAGE_SIZE);
                paging_unmap(__space, TEMPORARY_PAGE);
                paging_map(&new, as->items[i].virt, new_phys, PAGE_WRITE);
        }

        return new;
}
