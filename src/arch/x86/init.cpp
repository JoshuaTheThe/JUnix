
#include <junix.hpp>
#include <mm/pmm.hpp>
#include <mm/vmm.hpp>

extern uint8_t mem_start[];

extern "C" void newinit(int magic, uintptr_t multiboot_info)
{
  (void)magic;
  (void)multiboot_info;
  PhysicalMemoryManager pmm;
  VirtualMemoryManager  vmm(pmm);
  
  while(1)
    ;
  return;
}
