
#ifndef ALLOC_H
#define ALLOC_H

#include <mm/pmm.h>

__attribute__((ownership_returns(kmalloc), alloc_size(1)))  void *kmalloc(size_t siz);
__attribute__((ownership_takes(kmalloc, 1)))                void kfree(void *base);

#endif
