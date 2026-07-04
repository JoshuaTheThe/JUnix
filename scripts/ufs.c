#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#ifndef UFS_H
#define UFS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

#endif

// Forward declarations
static bool build_fs(const char *dir_path, const char *image_path, const char *vol_id);
static bool process_directory(FILE *img, const char *dir_path, uint16_t parent_block,
                            uint16_t *next_free_block, uint16_t *block_count);
static uint16_t allocate_block(uint16_t *next_free_block);
static bool add_file_entry(ufs_block_t *block, const char *name, bool is_dir,
                          uint16_t first_block, uint16_t max_entries);
static bool write_data_blocks(FILE *img, const char *file_path, uint16_t first_block,
                            uint16_t *next_free_block, uint16_t *block_count);
static uint16_t get_filesize_blocks(const char *file_path);
static void normalize_filename(char *dest, const char *src);

// Error handling
#define ERROR(msg) do { fprintf(stderr, "Error: %s\n", msg); return false; } while(0)

bool make_ufs_image(const char *dir_path, const char *image_path, const char *vol_id) {
    FILE *img;
    ufs_boot_sector_t boot;
    uint16_t next_free_block = 1; // Block 0 is boot sector
    uint16_t block_count = 1;     // Start with boot sector

    // Validate input
    if (!dir_path || !image_path) {
        ERROR("Invalid parameters");
    }

    // Set default volume ID if not provided
    char vol_id_buf[16] = {0};
    if (vol_id) {
        strncpy(vol_id_buf, vol_id, 15);
    } else {
        strcpy(vol_id_buf, "UFS Volume");
    }

    // Create image file
    img = fopen(image_path, "wb+");
    if (!img) {
        ERROR("Failed to create image file");
    }

    // Initialize boot sector
    memset(&boot, 0, sizeof(boot));
    boot.skip[0] = 0xEB;  // JMP SHORT
    boot.skip[1] = 0xfe;  // Skip 8 bytes
    boot.skip[2] = 0x90;  // NOP
    memcpy(boot.sign, "UFS\x7F", 4);
    boot.ver = UFS_MAX_VER;
    boot.sector_count = 0; // Will be set later
    boot.root_block = 0;   // Root block will be allocated
    memcpy(boot.vol_id, vol_id_buf, 16);
    memset(boot.bootcode, 0, 481);
    boot._AA55 = 0xAA55;

    // Write boot sector
    if (fwrite(&boot, sizeof(boot), 1, img) != 1) {
        fclose(img);
        ERROR("Failed to write boot sector");
    }

    // Build the filesystem from directory
    if (!process_directory(img, dir_path, 0, &next_free_block, &block_count)) {
        fclose(img);
        ERROR("Failed to build filesystem");
    }

    // Update boot sector with final values
    boot.sector_count = block_count;
    boot.root_block = 1; // Root directory starts at block 1

    // Seek back and rewrite boot sector
    fseek(img, 0, SEEK_SET);
    if (fwrite(&boot, sizeof(boot), 1, img) != 1) {
        fclose(img);
        ERROR("Failed to update boot sector");
    }

    fclose(img);
    return true;
}

static bool process_directory(FILE *img, const char *dir_path, uint16_t parent_block,
                            uint16_t *next_free_block, uint16_t *block_count) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    ufs_block_t *dir_block;
    uint16_t dir_block_num;
    uint16_t entry_count = 0;
    char full_path[512];

    dir = opendir(dir_path);
    if (!dir) {
        perror("Failed to open directory");
        return false;
    }

    // Allocate block for directory entries
    dir_block_num = allocate_block(next_free_block);
    (*block_count)++;

    // Initialize directory block
    dir_block = calloc(1, sizeof(ufs_block_t));
    if (!dir_block) {
        closedir(dir);
        ERROR("Memory allocation failed");
    }
    dir_block->next = UFS_END;

    // Add parent directory entry if not root
    if (parent_block != 0) {
        if (!add_file_entry(dir_block, "..", true, parent_block, 510 / sizeof(ufs_file_t))) {
            free(dir_block);
            closedir(dir);
            ERROR("Failed to add parent directory entry");
        }
        entry_count++;
    }

    // Add self entry
    if (!add_file_entry(dir_block, ".", true, dir_block_num, 510 / sizeof(ufs_file_t))) {
        free(dir_block);
        closedir(dir);
        ERROR("Failed to add self directory entry");
    }
    entry_count++;

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (stat(full_path, &st) != 0) {
            perror("Failed to stat file");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            // Process subdirectory
            uint16_t subdir_block;
            if (!process_directory(img, full_path, dir_block_num, next_free_block, block_count)) {
                free(dir_block);
                closedir(dir);
                return false;
            }
            subdir_block = *next_free_block - 1; // Last allocated block

            if (!add_file_entry(dir_block, entry->d_name, true, subdir_block,
                              510 / sizeof(ufs_file_t))) {
                free(dir_block);
                closedir(dir);
                ERROR("Failed to add directory entry");
            }
            entry_count++;
        } else if (S_ISREG(st.st_mode)) {
            // Process regular file
            uint16_t first_block;

            // Write file data
            if (!write_data_blocks(img, full_path, *next_free_block, next_free_block, block_count)) {
                free(dir_block);
                closedir(dir);
                return false;
            }
            first_block = *next_free_block - 1;

            // Add file entry
            char filename[12];
            normalize_filename(filename, entry->d_name);
            if (!add_file_entry(dir_block, filename, false, first_block,
                              510 / sizeof(ufs_file_t))) {
                free(dir_block);
                closedir(dir);
                ERROR("Failed to add file entry");
            }
            entry_count++;
        }
    }

    // Write directory block to image
    fseek(img, dir_block_num * 512, SEEK_SET);
    if (fwrite(dir_block, sizeof(ufs_block_t), 1, img) != 1) {
        free(dir_block);
        closedir(dir);
        ERROR("Failed to write directory block");
    }

    free(dir_block);
    closedir(dir);
    return true;
}

static uint16_t allocate_block(uint16_t *next_free_block) {
    return (*next_free_block)++;
}

static bool add_file_entry(ufs_block_t *block, const char *name, bool is_dir,
                          uint16_t first_block, uint16_t max_entries) {
    ufs_file_t *files = block->files;
    int i;

    for (i = 0; i < max_entries; i++) {
        if (files[i].name[0] == '\0') {
            char filename[12] = {0};
            normalize_filename(filename, name);
            memcpy(files[i].name, filename, 11);
            files[i].is_dir = is_dir;
            files[i].is_readable = true;
            files[i].is_writeable = !is_dir; // Directories read-only
            files[i].is_executable = !is_dir;
            files[i].is_system = false;
            files[i].is_hidden = false;
            files[i].first_block = first_block;
            return true;
        }
    }

    return false; // No free entries
}

static bool write_data_blocks(FILE *img, const char *file_path, uint16_t first_block,
                            uint16_t *next_free_block, uint16_t *block_count) {
    FILE *file;
    uint16_t current_block = first_block;
    ufs_block_t *data_block;
    size_t bytes_read;
    bool first = true;

    file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        return false;
    }

    data_block = calloc(1, sizeof(ufs_block_t));
    if (!data_block) {
        fclose(file);
        ERROR("Memory allocation failed");
    }

    while ((bytes_read = fread(data_block->data, 1, 510, file)) > 0) {
        if (!first) {
            // Allocate new block
            current_block = allocate_block(next_free_block);
            (*block_count)++;
        }
        first = false;

        // Set next pointer
        data_block->next = UFS_END;

        // Write data block
        fseek(img, current_block * 512, SEEK_SET);
        if (fwrite(data_block, sizeof(ufs_block_t), 1, img) != 1) {
            free(data_block);
            fclose(file);
            ERROR("Failed to write data block");
        }

        // Clear block for next iteration
        memset(data_block, 0, sizeof(ufs_block_t));
    }

    free(data_block);
    fclose(file);
    return true;
}

static uint16_t get_filesize_blocks(const char *file_path) {
    struct stat st;
    if (stat(file_path, &st) != 0)
        return 0;

    return (st.st_size + 509) / 510; // Ceiling division by 510
}

static void normalize_filename(char *dest, const char *src) {
    // Just copy the name as-is, up to 11 characters
    int i;
    int len = strlen(src);

    // Copy up to 11 characters
    for (i = 0; i < 11 && i < len; i++) {
        dest[i] = src[i];
    }

    // Pad with spaces if shorter than 11
    for (; i < 11; i++) {
        dest[i] = ' ';
    }

    dest[11] = '\0';
}

int main(int argc, char *argv[]) {
    const char *source_dir;
    const char *image_file;
    const char *vol_id = NULL;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source_directory> <image_file> [volume_id]\n", argv[0]);
        return 1;
    }

    source_dir = argv[1];
    image_file = argv[2];
    if (argc > 3)
        vol_id = argv[3];

    if (make_ufs_image(source_dir, image_file, vol_id)) {
        printf("UFS image created successfully: %s\n", image_file);
        return 0;
    } else {
        fprintf(stderr, "Failed to create UFS image\n");
        return 1;
    }
}
