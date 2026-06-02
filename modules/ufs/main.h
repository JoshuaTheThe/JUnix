
#ifndef UFS_H
#define UFS_H

#include <stdint.h>

#define MAGIC ("\x7fUFS")

typedef struct
{
        char     jmp_nop[3];
        char     magic[4];
        char     volume_name[20];
        uint32_t next_volume_lba;
        uint32_t blocks_in_volume;
        uint32_t bytes_in_sector;
        uint32_t root_dir_block;
        uint32_t reserved_bootloader_blocks;
        uint32_t next_free_block;
        char     bootloader[456];
        char     _AAH_55H;
} UFS_BootSector;

typedef enum
{
        UFS_TYPE_NONE,
        UFS_TYPE_DIR,
        UFS_TYPE_DATA,
} UFS_BlockType;

typedef struct
{
        char     name[8];
        uint32_t first_block;
        uint16_t size;
        uint8_t  padd;
        uint8_t  type;
} UFS_FileEntry;

typedef struct
{
        union
        {
                UFS_FileEntry dir[255];
                char          data[4088];
        } as;
        
        uint32_t next_block; // 0 on empty
        uint8_t  type;
        uint8_t  padd[3];
} UFS_Block;

#endif
