
#include <elf.h>

#include <mm/paging.h>
#include <mm/pmm.h>
#include <mm/alloc.h>

#include <string.h>
#include <dbg.h>

#define PAGE_ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(x)   (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

static bool elf_verify(Elf_Ehdr *eh)
{
        if (eh->e_ident[EI_MAG0] != ELFMAG0 ||
            eh->e_ident[EI_MAG1] != ELFMAG1 ||
            eh->e_ident[EI_MAG2] != ELFMAG2 ||
            eh->e_ident[EI_MAG3] != ELFMAG3)
        {
                return false;
        }

        if (eh->e_ident[EI_CLASS] != ELF_CLASS)
                return false;
        if (eh->e_ident[EI_DATA] != ELF_DATA)
                return false;
        if (eh->e_machine != ELF_ARCH)
                return false;
        if (eh->e_version != EV_CURRENT)
                return false;
        if (eh->e_type != ET_EXEC)
                return false;
        return true;
}


/*
 * Load one PT_LOAD segment
 */
static int elf_load_segment(
        file_t *file,
        address_space_t *space,
        Elf_Phdr *ph)
{
        Elf_Addr start =
                PAGE_ALIGN_DOWN(ph->p_vaddr);
        Elf_Addr end =
                PAGE_ALIGN_UP(ph->p_vaddr + ph->p_memsz);

        /*
         * Allocate virtual memory
         */
        for (Elf_Addr addr = start;
             addr < end;
             addr += PAGE_SIZE)
        {
                void *page = pmm_alloc();
                if (!page)
                        return -1;
                uintptr_t phys =
                    virt_to_phys(
                        paging_get_address_space(),
                        page);
                paging_map(
                        space,
                        addr,
                        phys,
                        PAGE_WRITE | PAGE_USER); // USER for future
        }

        /*
         * Load file contents
         */
        Elf_Addr file_left = ph->p_filesz;
        Elf_Addr file_pos  = ph->p_offset;
        Elf_Addr virt      = ph->p_vaddr;

        vfs_lseek(file, file_pos, SEEK_SET);

        while (file_left)
        {
                Elf_Addr page =
                        PAGE_ALIGN_DOWN(virt);
                Elf_Addr offset =
                        virt & (PAGE_SIZE - 1);
                Elf_Addr amount =
                        PAGE_SIZE - offset;
                if (amount > file_left)
                        amount = file_left;

                /*
                 * Temporary identity mapping.
                 *
                 * Replace this with
                 * mapping helper later.
                 */
                void *dst =
                        (void *)(page + offset);
                vfs_read(file, dst, amount);
                virt += amount;
                file_left -= amount;
        }

        /*
         * Clear BSS
         */
        Elf_Addr zero_start =
                ph->p_vaddr + ph->p_filesz;
        Elf_Addr zero_end =
                ph->p_vaddr + ph->p_memsz;

        while (zero_start < zero_end)
        {
                *(uint8_t *)zero_start = 0;
                zero_start++;
        }

        return 0;
}


int elf_load(
        file_t *file,
        address_space_t *space,
        Elf_Addr *entry)
{
        Elf_Ehdr eh;
        vfs_lseek(file, 0, SEEK_SET);

        if (vfs_read(file, &eh, sizeof(eh)) != sizeof(eh))
                return -1;


        if (!elf_verify(&eh))
        {
                LOG(" [elf] invalid executable\r\n");
                return -1;
        }


        LOG(" [elf] loading entry %x\r\n",
                (uintptr_t)eh.e_entry);

        for (size_t i = 0; i < eh.e_phnum; i++)
        {
                Elf_Phdr ph;
                vfs_lseek(file, eh.e_phoff + i * eh.e_phentsize, SEEK_SET);
                if (vfs_read(file, &ph, sizeof(ph)) != sizeof(ph))
                        return -1;


                if (ph.p_type != PT_LOAD)
                        continue;


                LOG(" [elf] segment %x size %x\r\n",
                    (uintptr_t)ph.p_vaddr,
                    (uintptr_t)ph.p_memsz);


                if (elf_load_segment(
                    file,
                    space,
                    &ph))
                {
                        return -1;
                }
        }


        *entry = eh.e_entry;
        return 0;
}
