#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

class PhysicalMemoryManager;

class VirtualMemoryManager
{
public:
  static constexpr int       PAGE_SIZE           = 4096;
  static constexpr int       FLAG_PRESENT        = 1 << 0;
  static constexpr int       FLAG_WRITE          = 1 << 1;
  static constexpr int       FLAG_USER           = 1 << 2;
  static constexpr int       FLAG_PWT            = 1 << 3;
  static constexpr int       FLAG_PCD            = 1 << 4;
  static constexpr int       FLAG_ACCESSED       = 1 << 5;
  static constexpr int       FLAG_DIRTY          = 1 << 6;
  static constexpr int       FLAG_4MB            = 1 << 7;
  static constexpr int       FLAG_GLOBAL         = 1 << 8;
  static constexpr int       MAX_KERNEL_MAPPINGS = 20000;
  static constexpr uintptr_t TEMPORARY_PAGE  = 0xFFC00000ULL;
  static constexpr int       DIR(int x) { return (x >> 22) & 0x3FF; }
  static constexpr int       TAB(int x) { return (x >> 12) & 0x3FF; }
  static constexpr int       OFF(int x) { return x & 0xFFF; }

  typedef uint32_t PageTableEntry;
  typedef uint32_t PageDirectoryEntry;
  typedef struct { PageTableEntry     e[1024]; } PageTable;
  typedef struct { PageDirectoryEntry e[1024]; } PageDirectory;

  class VirtualAddressSpace
  {
  public:
    struct Mapping
    {
      uint32_t phys;
      uint32_t virt;
      uint32_t flags;
    };

  private:
    VirtualMemoryManager *vmm;
  public:
    size_t   count=0,capacity=0;
    Mapping      *items;
    uintptr_t     pagedir_phys;
    PageDirectory page_dir = {0};
    bool          isbootstrap = false; // is initial kernel mapping

    VirtualAddressSpace(VirtualMemoryManager &vmm) :
      vmm(&vmm), count(0), capacity(0), items(nullptr), pagedir_phys(0)
    {}

    VirtualAddressSpace(VirtualMemoryManager &vmm, bool b) :
      vmm(&vmm), count(0), capacity(0), items(nullptr), pagedir_phys(0), isbootstrap(b)
    {}

    uintptr_t VirtualToPhysical(uintptr_t adr);
    uintptr_t PhysicalToVirtual(uintptr_t adr);

    bool Remove(uintptr_t virt);
    bool Validate(uintptr_t virt);

    void Map(uintptr_t virt,
             uintptr_t phys,
             uint32_t  flags);
    void UnMap(uintptr_t virt);
    void Clear(void);

    int Allocate(void)
    {
      size_t index = this->count;
      if (index >= this->capacity)
      {
        return -1;
      }

      return this->count++;
    }
    VirtualAddressSpace Clone(void);
  };

private:
  VirtualAddressSpace KernelAddressSpace,
                     &Current;
  PhysicalMemoryManager &pmm;
  VirtualAddressSpace::Mapping KernelMappings[MAX_KERNEL_MAPPINGS] = {{0,0,0}};
public:
  const VirtualAddressSpace& Get(void) const { return Current; }
  void Switch(VirtualAddressSpace &New);
  VirtualMemoryManager(PhysicalMemoryManager &pmm);
};
