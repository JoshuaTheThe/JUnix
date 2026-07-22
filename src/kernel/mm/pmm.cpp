
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
  size_t free_count = 0;
  for (size_t i = 0; i < TOTAL_BITMAP; i++)
  {
    if (!(this->bitmap[i >> 6] & (1ULL << (i & 63))))
      free_count++;
  }
  return free_count;
}

int PhysicalMemoryManager::FindFreePage(size_t start)
{
  const size_t start_word = start >> 6;
  const size_t start_bit = start & 63;
  
  for (size_t word = start_word; word < (TOTAL_BITMAP >> 6); word++)
  {
    uint64_t used = this->bitmap[word];
    uint64_t free_bits = ~used;
    if (word == start_word)
      free_bits &= (~0ULL << start_bit);
    if (free_bits)
    {
      size_t bit = __builtin_ctzll(free_bits);
      size_t page_index = (word << 6) + bit;
      if (page_index < TOTAL_BITMAP)
        return page_index;
    }
  }
  return -1;
}

int PhysicalMemoryManager::FindFreePage(void)
{
  int n = this->FindFreePage(this->hint);
  if (n >= 0)
    return n;
  return this->FindFreePage(0);
}

void PhysicalMemoryManager::Free(uintptr_t addr)
{
  const size_t index =
    (addr - (uintptr_t)this->physstart) / PAGE_SIZE;
  const size_t byte  = index >> 6;
  const size_t bit   = index &  63;
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
