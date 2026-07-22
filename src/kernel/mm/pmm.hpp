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
  uint8_t   bitmap[TOTAL_BITMAP >> 3] = {0};
  uintptr_t physstart;
public:
  PhysicalMemoryManager(uintptr_t phys)
    : physstart(phys)
  {}

  uintptr_t Allocate(void);
  void Free(uintptr_t);
  size_t Remaining(void) const;
private:
  int FindFreePage(void);
  
  inline void SetBit(size_t row, size_t col)
  { this->bitmap[row] |=  (uint8_t)(1u<<col); }
  
  inline void ClearBit(size_t row, size_t col)
  { this->bitmap[row] &= ~(uint8_t)(1u<<col); }
};
