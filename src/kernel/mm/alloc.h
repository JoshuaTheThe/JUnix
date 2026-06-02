
#ifndef ALLOC_H
#define ALLOC_H

#include <mm/bitmap.h>

typedef struct block_header_t
{
        size_t siz;
        struct block_header_t *nxt;
} block_header_t;

void *kmalloc(size_t siz);
void kfree(void *base);

#endif
