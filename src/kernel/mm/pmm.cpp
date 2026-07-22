
/**
 * Architecture Independent PMM (Physical Memory Manager)
 * Requires:
 *    - @arch/mm/paging.h, PAGE_SIZE
 */

#include <mm/pmm.hpp>
extern "C"
{
  #include <panic.h>
  #include <string.h>
};

size_t PhysicalMemoryManager::Remaining(void) const
{
  size_t free(0);
  for(size_t i(0);
      free += !(this->bitmap[i>>3]&(1<<(i&7))),
      i<TOTAL_BITMAP-1;
      ++i)
    ;

  return free;
}

int PhysicalMemoryManager::FindFreePage(size_t starting_from)
{
  for(size_t i=starting_from; i<TOTAL_BITMAP; ++i)
  {
    const size_t byte = i >> 3;
    const size_t bit  = i &  7;
    if(!(this->bitmap[byte]&(1<<bit)))
    {
      hint = i + 1;
      return i;
    }
  }

  return -1;
}

int PhysicalMemoryManager::FindFreePage(void)
{
  int n = this->FindFreePage(this->hint);
  if (n > 0)
    return n;
  return this->FindFreePage(0);
}

void PhysicalMemoryManager::Free(uintptr_t addr)
{
  const size_t index =
    (addr - (uintptr_t)this->physstart) / PAGE_SIZE;
  const size_t byte  = index >> 3;
  const size_t bit   = index &  7;
  this->ClearBit(byte, bit);
}

uintptr_t PhysicalMemoryManager::Allocate(void)
{
  int index = this->FindFreePage();
  if (index==-1)
  {
    ::panic(PANIC_TODO);
  }

  uintptr_t page =
    ((uintptr_t)this->physstart + (index * PAGE_SIZE));
  return page;
}
