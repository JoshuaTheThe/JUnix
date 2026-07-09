
#ifndef PART_H
#define PART_H

#include <fs/fs.h>

typedef struct __attribute__((packed))
{
        uint8_t  status;
        uint8_t  chs_first[3];
        uint8_t  type;
        uint8_t  chs_last[3];
        uint32_t start_lba;
        uint32_t sector_count;
} mbr_partition_t;

// private field for partition file
typedef struct
{
        file_t  *reference;
        size_t   offset;
} partition_t;

int search_for_partitions(vnode_t *node);

#endif
