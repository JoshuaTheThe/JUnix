#pragma once
#include <junix.hpp>
#include <mm/vmm.hpp>

extern uint8_t mem_start[];

class PhysicalMemoryManager
{
public:
  static constexpr  int TOTAL_BITMAP = 1024 * 1024;
  static constexpr  int BITMAP_WORDS = TOTAL_BITMAP >> 6;
private:
  uint64_t  bitmap[TOTAL_BITMAP >> 6] = {0};
  uint8_t   refcnt[TOTAL_BITMAP] = {0};
  uintptr_t physstart;
  size_t    hint = 0;
public:
  PhysicalMemoryManager() : physstart(0)
  {
    const size_t RESERVED_BYTES = (size_t)&mem_start[0];
    const size_t RESERVED_PAGES = RESERVED_BYTES / VirtualMemoryManager::PAGE_SIZE;
    
    for (size_t i = 0; i < RESERVED_PAGES; i++)
    {
      size_t word = i >> 6;
      size_t bit = i & 63;
      this->bitmap[word] |= (1ULL << bit);
    }
  }

  uintptr_t Allocate(void);
  void Free(uintptr_t);
  size_t Remaining(void) const;

  void Reference(uintptr_t phys);
private:
  int FindFreePage(void);
  int FindFreePage(size_t);
  
  void SetBit(size_t row, size_t col);
  void ClearBit(size_t row, size_t col);
  bool IsBitSet(size_t row, size_t col);
};
