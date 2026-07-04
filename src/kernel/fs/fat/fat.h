
#ifndef FAT_H
#define FAT_H

#include <fs/fs.h>
#include <stdint.h>

typedef struct __attribute__((__packed__))
{
        char     _0XEBXX90[3];
        char     OEM[8];
        uint16_t sector_size;
        uint8_t  cluster_size;
        uint16_t reserved_sectors;
        uint8_t  number_of_fats;
        uint16_t root_dir_entries;
        uint16_t short_sector_count;
        uint8_t  type;
        uint16_t sectors_per_fat;
        uint16_t sectors_per_track;
        uint16_t head_count;
        uint32_t hidden;
        uint32_t long_sector_count;

        union
        {
                struct
                {
                        uint8_t  drive_number;
                        uint8_t  reserved;
                        uint8_t  sign_x28_x29;
                        char     volume_serial[4];
                        char     volume[11];
                        char     identifier[8];
                        uint8_t  bootcode[448];
                        uint16_t _0xAA55;
                } fat12_16;

                struct
                {
                        uint32_t sectors_per_fat;
                        uint16_t flags;
                        uint16_t version;
                        uint32_t root_dir_cluster;
                        uint16_t fsinfo;
                        uint16_t backup;
                        uint8_t  reserved[12];
                        uint8_t  drive_number;
                        uint8_t  reserved2;
                        uint8_t  sign_x28_x29;
                        char     volume_serial[4];
                        char     volume[11];
                        char     identifier[8];
                        uint8_t  bootcode[420];
                        uint16_t _0xAA55;
                } fat32;
        };
} fat_bpb_t;

typedef struct __attribute__((__packed__))
{
        uint32_t _0x41615252;
        uint8_t  reserved[480];
        uint32_t _0x61417272;
        uint32_t last_known_free;
        uint32_t hint;
        uint8_t  reserved2[12];
        uint32_t _0xAA550000;
} fat_fsinfo_t;

#endif
