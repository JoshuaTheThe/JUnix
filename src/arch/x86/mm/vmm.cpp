
/**
 * ix86 VMM (Virtual Memory Manager)
 * Version 1.0
 */

#include <mm/vmm.hpp>
#include <mm/pmm.hpp>

/**
 * Virtual Address Space implementation
 */

bool VirtualMemoryManager::VirtualAddressSpace::Remove(uintptr_t virt)
{
  virt &= ~0xFFF;
  for(size_t i=0; i<this->count; ++i)
  {
    if(this->items[i].virt==virt)
    {
      this->items[i]=this->items[--this->count];
      return true;
    }
  }

  return false;
}

bool VirtualMemoryManager::VirtualAddressSpace::Validate(uintptr_t virt)
{
  const uint32_t dir = VirtualMemoryManager::DIR(virt);
  const uint32_t tbl = VirtualMemoryManager::TAB(virt);
  if(!(this->page_dir.e[dir]&VirtualMemoryManager::FLAG_PRESENT))
    return false;
  PageTable *pt = (PageTable *)this->PhysicalToVirtual(this->page_dir.e[dir] & ~0xfff);
  return pt->e[tbl] & VirtualMemoryManager::FLAG_PRESENT ? true : false;
}

uintptr_t VirtualMemoryManager::VirtualAddressSpace::VirtualToPhysical(uintptr_t virt)
{
  if(this->isbootstrap)
  {
    if(virt>=0xC0000000&&virt<0xC1000000)
      return virt-(0xC0000000-0x200000);
    return virt;
  }

  const size_t dir = VirtualMemoryManager::DIR(virt);
  const size_t tbl = VirtualMemoryManager::TAB(virt);
  if(dir>=1024||tbl>=1024)
    return 0;
  const PageDirectoryEntry pde = this->page_dir.e[dir];
  if(!(pde&VirtualMemoryManager::FLAG_PRESENT))
    return 0;
  const PageTable *const pt = (PageTable *)PhysicalToVirtual(pde & ~0xfff);
  if(!pt)
    return 0;
  const PageTableEntry pte = pt->e[tbl];
  if(pte>=1024)
    return 0;
  if (!(pte&VirtualMemoryManager::FLAG_PRESENT))
    return 0;
  return (pte&~0xfff)+VirtualMemoryManager::OFF(virt);
}

uintptr_t VirtualMemoryManager::VirtualAddressSpace::PhysicalToVirtual(uintptr_t phys)
{
  if(this->isbootstrap)
  {
    if(phys>=0x200000&&phys<0x240000)
      return phys+(0xC0000000-0x200000);
    return phys;
  }

  const size_t base(phys&~0xfff);
  const size_t off(phys&0xfff);
  for(size_t i=0; i<this->count; ++i)
  {
    const Mapping &m(this->items[i]);
    if(m.phys==phys)
      return m.virt+off;
  }

  return 0;
}

void VirtualMemoryManager::VirtualAddressSpace::Map(uintptr_t virt,
                                                    uintptr_t phys,
                                                    uint32_t  flags)
{
  const size_t dir = VirtualMemoryManager::DIR(virt);
  const size_t tbl = VirtualMemoryManager::TAB(virt);
  if(dir>=1024||tbl>=1024)
    return;
  uintptr_t new_pt = 0;
  if(!(this->page_dir.e[dir] & VirtualMemoryManager::FLAG_PRESENT))
  {
    new_pt = this->vmm->pmm.Allocate();
    page_dir.e[dir] =
      new_pt | VirtualMemoryManager::FLAG_PRESENT | VirtualMemoryManager::FLAG_WRITE;
    Mapping &m = this->items[this->Allocate()];
    m.phys = new_pt & ~0xfff;
    m.virt = new_pt & ~0xfff;
  }

  PageTable *pt = (PageTable *)this->PhysicalToVirtual(this->page_dir.e[dir] & ~0xfff);
  if(!pt)
  {
    return;
  }

  pt->e[tbl] =
    (phys & ~0xfff) |
    flags | VirtualMemoryManager::FLAG_PRESENT;
  Mapping &m = this->items[this->Allocate()];
  m.phys = phys & ~0xfff;
  m.virt = virt & ~0xfff;
}

void VirtualMemoryManager::VirtualAddressSpace::UnMap(uintptr_t virt)
{
  const size_t dir = VirtualMemoryManager::DIR(virt);
  const size_t tbl = VirtualMemoryManager::TAB(virt);
  if(dir>=1024||tbl>=1024)
    return;
  const PageDirectoryEntry pde = this->page_dir.e[dir];
  if(!(pde&VirtualMemoryManager::FLAG_PRESENT))
    return;
  const uintptr_t phys(this->VirtualToPhysical(virt));
  this->Remove(virt);
  PageTable *pt((PageTable *)this->PhysicalToVirtual(pde&~0xfff));
  if(!pt)
    return;
  pt->e[tbl]=0;
}

/**
 * Virtual Memory Manager implementation
 */

void VirtualMemoryManager::Switch(VirtualMemoryManager::VirtualAddressSpace &New)
{
  Current = New;
  const uintptr_t phys(Current.VirtualToPhysical((uintptr_t)&Current.page_dir.e[0]));
  __asm__ volatile (
    "mov %0, %%cr3"
    : : "r" (phys) : "memory"
  );
}

VirtualMemoryManager::VirtualMemoryManager(PhysicalMemoryManager &pmm)
  : KernelAddressSpace(*this, true), Current(KernelAddressSpace), pmm(pmm)
{
  KernelAddressSpace.items        = &KernelMappings[0];
  KernelAddressSpace.count        = VirtualMemoryManager::MAX_KERNEL_MAPPINGS;
  KernelAddressSpace.capacity     = VirtualMemoryManager::MAX_KERNEL_MAPPINGS;
  KernelAddressSpace.pagedir_phys = pmm.Allocate();
  VirtualAddressSpace::Mapping &m = KernelAddressSpace.items[KernelAddressSpace.Allocate()];
  m.phys = KernelAddressSpace.pagedir_phys;
  m.virt = KernelAddressSpace.pagedir_phys;

  for(uintptr_t addr = 0x200000; addr<0x1000000; addr+=VirtualMemoryManager::PAGE_SIZE)
  {
    KernelAddressSpace.Map(addr + (0xC0000000 - 0x200000), addr, FLAG_WRITE);
  }

  this->Switch(KernelAddressSpace);
}
