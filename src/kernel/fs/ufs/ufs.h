
#ifndef UFS_H
#define UFS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <fs/fs.h>

#define UFS_SIGN    "UFS\x7F"
#define UFS_MAX_VER 0

// 512 byte blocks
// magics for next, allocated is (u)-1>n>0
#define UFS_FREE (0)
#define UFS_END  (-1)

// if first letter of name is null, the file doesn't exist

typedef struct __attribute__((__packed__))
{
        char     skip[3];         // JMP SHORT XX, NOP
        char     sign[4];         // UFS\7f
        uint16_t ver;             // version
        uint16_t sector_count;    // amount of sectors in volume
        uint16_t root_block;      // LBA offset of first file dir block
        char     vol_id[16];      // volume id
        char     bootcode[481];
        uint16_t _AA55;
} ufs_boot_sector_t;

typedef struct __attribute__((__packed__))
{
        char     name[11];
        struct
        {
                bool is_dir : 1;
                bool is_readable : 1;
                bool is_writeable : 1;
                bool is_executable : 1;
                bool is_system : 1;
                bool is_hidden : 1;
        };

        uint16_t first_block;
} ufs_file_t;

typedef struct __attribute__((__packed__))
{
        uint16_t next;
        union
        {
                ufs_file_t files[510 / sizeof(ufs_file_t)];
                uint8_t    data[510];
        };
} ufs_block_t;

filesystem_t ufs_create_fs(void);

#endif
