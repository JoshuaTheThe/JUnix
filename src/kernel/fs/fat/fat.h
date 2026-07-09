
#ifndef FAT_H
#define FAT_H

#include <fs/fs.h>
#include <stdint.h>
#include <stdbool.h>

#define FAT_BOOT_SECTOR_SIGN (0xAA55)

#define FAT_EXT_BOOT_RECORD_16_SIGN_A (0x28)
#define FAT_EXT_BOOT_RECORD_16_SIGN_B (0x29)

#define FAT_INFO_MAGIC_A (0x41615252)
#define FAT_INFO_MAGIC_B (0x61417272)
#define FAT_INFO_MAGIC_C (0xAA550000)

#define FAT_READ_ONLY (0x01)
#define FAT_HIDDEN (0x02)
#define FAT_SYSTEM (0x04)
#define FAT_VOLUME_ID (0x08)
#define FAT_DIRECTORY (0x10)
#define FAT_ARCHIVE (0x20)
#define FAT_LONG_FILE_NAME (0xF)

#define FAT_UNUSED (0xE5)


typedef enum
{
        FAT_UNKNOWN,
        FAT_12,
        FAT_16,
        FAT_32,
} fat_type_t;

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

_Static_assert(sizeof(fat_bpb_t) == 512, "BPB size incorrect");

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

typedef struct __attribute__((__packed__))
{
        uint8_t hour : 5;
        uint8_t minutes : 6;
        uint8_t seconds : 5; /* Multiply By 2 */
} fat_time_t;

typedef struct __attribute__((__packed__))
{
        uint8_t year : 7;
        uint8_t month : 4;
        uint8_t day : 5;
} fat_date_t;

typedef struct __attribute__((__packed__))
{
        uint8_t name[8], ext[3];
        uint8_t flags;
        uint8_t reserved;
        uint8_t __time_ignore; /* we do not care for the purposes of this OS */
        uint16_t time;
        uint16_t date;
        uint16_t last_accessed_date;
        uint16_t entry_first_cluster_high;
        uint16_t last_modified_time;
        uint16_t last_modified_date;
        uint16_t entry_first_cluster_low;
        uint32_t size; /* Bytes */
} fat_dir_t;

typedef struct
{
        uint32_t  cluster;
        uint32_t  index;
} fat_file_location_t;

filesystem_t fat_create_fs(void);

#endif
