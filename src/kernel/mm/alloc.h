
#ifndef ALLOC_H
#define ALLOC_H

#include <mm/pmm.h>

void *kmalloc(size_t siz);
void kfree(void *base);

#endif
