
#include <junix.hpp>
#include <mm/pmm.hpp>

extern uint8_t mem_start[];

extern "C" void init(int magic, uintptr_t multiboot_info)
{
  (void)magic;
  (void)multiboot_info;
  PhysicalMemoryManager pmm((uintptr_t)&mem_start[0]);
  while(1)
    ;
  return;
}
