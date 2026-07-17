
#ifndef ALLOC_H
#define ALLOC_H

#include <mm/pmm.h>

#define kfree(p)   (__kfree(p, __FILE__, __LINE__))
#define kmalloc(p) (__kmalloc(p, __FILE__, __LINE__))
#define krealloc(p, s) (__krealloc(p, s, __FILE__, __LINE__))

void *__kmalloc(size_t siz, const char *FILE, long LINE);
void *__krealloc(void *base, size_t siz, const char *FILE, long LINE);
void __kfree(void *base, const char *FILE, long LINE);

#endif
