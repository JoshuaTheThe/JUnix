#pragma once
#include <junix.hpp>

extern "C"
{
  #include <mm/paging.h>
};

#define TOTAL_BITMAP (1024 * 1024)

class PhysicalMemoryManager
{
private:
  uint64_t  bitmap[TOTAL_BITMAP >> 6] = {0};
  uintptr_t physstart;
  size_t    hint = 0;
public:
  PhysicalMemoryManager(uintptr_t phys)
    : physstart(phys)
  {}

  uintptr_t Allocate(void);
  void Free(uintptr_t);
  size_t Remaining(void) const;
private:
  int FindFreePage(void);
  int FindFreePage(size_t);
  
  inline void SetBit(size_t row, size_t col)
  { this->bitmap[row] |=  (uint64_t)(1u<<col); }
  
  inline void ClearBit(size_t row, size_t col)
  { this->bitmap[row] &= ~(uint64_t)(1u<<col); }
};
