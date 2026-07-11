
#include <mm/paging.h>
#include <mm/pmm.h>
#include <sched/core.h>
#include <cpu/cpu.h>
#include <drivers/kprint.h>
#include <string.h>

static bool paging_active = false;

uintptr_t virt_to_phys(void *ptr)
{
        if (!paging_active)
                return (uintptr_t)ptr;
        uintptr_t virt = (uintptr_t)ptr;

        uint32_t dir = PAGE_DIR_INDEX(virt);
        uint32_t tab = PAGE_TAB_INDEX(virt);

        pde_t pde = active_task->pd->entries[dir];

        if (!(pde & PAGE_PRESENT))
                return 0;

        page_table_t *pt =
        phys_to_virt(pde & ~0xFFF);
        if (!pt)
                return 0;

        pte_t pte = pt->entries[tab];

        if (!(pte & PAGE_PRESENT))
                return 0;

        return (pte & ~0xFFF) + PAGE_OFFSET(virt);
}

void *phys_to_virt(uintptr_t addr)
{
        if (!paging_active)
                return (void *)addr;
        uintptr_t phys = addr & ~0xFFF;
        uintptr_t offset = addr & 0xFFF;
        for (size_t i = 0; i < active_task->mappings.count; i++)
        {
                mapping_t *m = &active_task->mappings.items[i];

                if (m->phys == phys)
                {
                        return (void *)(m->virt + offset);
                }
        }

        return NULL;
}

void paging_switch(page_directory_t *pd)
{
        uintptr_t phys = virt_to_phys(pd);
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

void paging_map(uintptr_t virt, uintptr_t phys, uint32_t flags)
{
        uint32_t dir = PAGE_DIR_INDEX(virt);
        uint32_t tbl = PAGE_TAB_INDEX(virt);
        if (!(active_task->pd->entries[dir] & PAGE_PRESENT))
        {
                page_table_t *pt = pmm_alloc();
                memset(pt, 0, PAGE_SIZE);
                uintptr_t pt_phys = virt_to_phys(pt);
                active_task->pd->entries[dir] =
                        pt_phys |
                        PAGE_PRESENT |
                        PAGE_WRITE;
                mapping_t *m =
                        &active_task->mappings.items[active_task->mappings.count++];
                        m->phys = pt_phys & ~0xFFF;
                        m->virt = (uintptr_t)pt & ~0xFFF;
        }

        page_table_t *pt =
                phys_to_virt(active_task->pd->entries[dir] & ~0xFFF);
        pt->entries[tbl] =
                (phys & ~0xFFF) |
                flags |
                PAGE_PRESENT;
        phys &= ~0xFFF;
        virt &= ~0xFFF;

        mapping_t *m =
            &active_task->mappings.items[active_task->mappings.count++];
        m->phys = phys;
        m->virt = virt;
        __asm volatile("invlpg (%0)" :: "r"(virt) : "memory");
}

bool remove_mapping(uintptr_t phys)
{
        phys &= ~0xFFF;
        for (size_t i = 0; i < active_task->mappings.count; i++)
        {
                if (active_task->mappings.items[i].phys == (phys & ~0xFFF))
                {
                        // Move the last entry over this one
                        active_task->mappings.items[i] = active_task->mappings.items[active_task->mappings.count - 1];
                        active_task->mappings.count--;
                        return true;
                }
        }

        return false;
}

void paging_unmap(uintptr_t virt)
{
        return;
        uint32_t dir = PAGE_DIR_INDEX(virt);
        uint32_t tab = PAGE_TAB_INDEX(virt);

        pde_t pde = active_task->pd->entries[dir];
        if (!(pde & PAGE_PRESENT))
                return;
        kprint(" [krnl] unmapping virt %x (%x)\r\n", virt, phys_to_virt(pde & ~0xFFF));
        remove_mapping(virt);
        //pmm_free((void *)virt_to_phys((void *)virt));
        page_table_t *pt = phys_to_virt(pde & ~0xFFF);
        pt->entries[tab] = 0;
        __asm volatile("invlpg (%0)" :: "r"(virt) : "memory");
}

void paging_init(void)
{
        active_task = &early_task;
        paging_disable();
        active_task->pd = pmm_alloc();
        mapping_t *m =
            &active_task->mappings.items[active_task->mappings.count++];
        m->phys = virt_to_phys(active_task->pd) & ~0xFFF;
        m->virt = (uintptr_t)active_task->pd & ~0xFFF;
        memset(active_task->pd, 0, PAGE_SIZE);

        /*
        * Identity map first 128 KiB (pmm bitmap)
        */
        for (uintptr_t addr = 0;
             addr < 0x20000;
             addr += PAGE_SIZE)
        {
                paging_map(addr, addr, PAGE_WRITE);
        }

        /*
        * Identity map rest 15 MiB
        */
        for (uintptr_t addr = 0x100000;
             addr < 0x01000000;
             addr += PAGE_SIZE)
        {
                paging_map(addr, addr, PAGE_WRITE);
        }

        /*
        * Load page directory
        */
        paging_switch(active_task->pd);

        /*
        * Enable paging
        */
        paging_enable();
        paging_active = true;
}
