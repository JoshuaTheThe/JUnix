
#ifndef ALLOC_H
#define ALLOC_H

#include <mm/pmm.h>

#define kfree(p)   (__kfree(p, __FILE__, __LINE__))
#define kmalloc(p) (__kmalloc(p, __FILE__, __LINE__))

__attribute__((ownership_returns(__kmalloc), alloc_size(1)))  void *__kmalloc(size_t siz, const char *FILE, long LINE);
__attribute__((ownership_takes(__kmalloc, 1)))                void __kfree(void *base, const char *FILE, long LINE);

#endif
