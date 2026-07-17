
#include <fs/fat/fat.h>
#include <fs/fs.h>
#include <drivers/kprint.h>
#include <mm/alloc.h>
#include <string.h>
#include <dbg.h>

struct private
{
        fat_bpb_t bpb;
        file_t   *base;
        fat_type_t type;
};

static fat_bpb_t read_boot_sector(file_t *f)
{
        fat_bpb_t sect;
        vfs_lseek(f, 0, SEEK_SET);
        vfs_read(f, &sect, sizeof(sect));
        return sect;
}

static uint32_t total_sectors(fat_bpb_t *bt)
{
        return (bt->short_sector_count) ? bt->short_sector_count : bt->long_sector_count;
}

static uint32_t size(fat_bpb_t *bt)
{
        return (bt->sectors_per_fat) ? bt->sectors_per_fat : bt->fat32.sectors_per_fat;
}

static uint32_t root_dir_size(fat_bpb_t *bt)
{
        return ((bt->root_dir_entries * sizeof(fat_dir_t)) + (bt->sector_size - 1)) / bt->sector_size;
}

static uint32_t first_data(fat_bpb_t *bt)
{
        return (bt->reserved_sectors + (bt->number_of_fats * size(bt)) + root_dir_size(bt));
}

static uint32_t first_fat(fat_bpb_t *bt)
{
        return (bt->reserved_sectors);
}

static uint32_t data_cnt(fat_bpb_t *bt)
{
        return total_sectors(bt) - (bt->reserved_sectors + (bt->number_of_fats * size(bt)) + root_dir_size(bt));
}

static uint32_t cluster_count(fat_bpb_t *bt)
{
        return data_cnt(bt) / bt->cluster_size;
}

static fat_type_t identify(fat_bpb_t *bt)
{
        uint32_t total = cluster_count(bt);
        if (total < 4085 &&
            (bt->fat12_16.sign_x28_x29 == FAT_EXT_BOOT_RECORD_16_SIGN_A ||
             bt->fat12_16.sign_x28_x29 == FAT_EXT_BOOT_RECORD_16_SIGN_B))
                return FAT_12;
        else if (total < 65525 &&
                 (bt->fat12_16.sign_x28_x29 == FAT_EXT_BOOT_RECORD_16_SIGN_A ||
                  bt->fat12_16.sign_x28_x29 == FAT_EXT_BOOT_RECORD_16_SIGN_B))
                return FAT_16;
        else if (bt->fat32.sign_x28_x29 == FAT_EXT_BOOT_RECORD_16_SIGN_A ||
                 bt->fat32.sign_x28_x29 == FAT_EXT_BOOT_RECORD_16_SIGN_B)
                return FAT_32;
        return FAT_UNKNOWN;
}

__attribute__((__used__))
static uint32_t first_root(fat_bpb_t *bt)
{
        return first_data(bt) - root_dir_size(bt);
}

static uint32_t root_cluster(fat_bpb_t *bt)
{
        return bt->fat32.root_dir_cluster;
}

static uint32_t first_sector_for_cluster(fat_bpb_t *bt, uint32_t cluster)
{
        return ((cluster - 2) * bt->cluster_size) + first_data(bt);
}

static uint32_t next_cluster_32(fat_bpb_t *bt, uint32_t cluster, file_t *file)
{
        uint32_t fat_off  = cluster * 4;
        uint32_t fat_sect = first_fat(bt) + (fat_off / bt->sector_size);
        uint32_t ent_off  = fat_off % bt->sector_size;

        char sector[bt->sector_size];
        vfs_lseek(file, bt->sector_size * fat_sect, SEEK_SET);
        vfs_read(file, sector, bt->sector_size);
        uint32_t entry    = *(uint32_t *)&sector[ent_off];

        return entry & 0x0FFFFFFF;
}

__attribute__((__used__))
static uint16_t next_cluster_16(fat_bpb_t *bt, uint16_t cluster, file_t *file)
{
        uint32_t fat_off  = cluster * 2;
        uint32_t fat_sect = first_fat(bt) + (fat_off / bt->sector_size);
        uint32_t ent_off  = fat_off % bt->sector_size;

        char sector[bt->sector_size];
        vfs_lseek(file, bt->sector_size * fat_sect, SEEK_SET);
        vfs_read(file, sector, bt->sector_size);
        return *(uint16_t *)&sector[ent_off];
}

__attribute__((__used__))
static uint16_t next_cluster_12(fat_bpb_t *bt, uint16_t cluster, file_t *file)
{
        uint32_t fat_off  = cluster + (cluster / 2);
        uint32_t fat_sect = first_fat(bt) + (fat_off / bt->sector_size);
        uint32_t ent_off  = fat_off % bt->sector_size;
        char sector[bt->sector_size];
        vfs_lseek(file, bt->sector_size * fat_sect, SEEK_SET);
        vfs_read(file, sector, bt->sector_size);
        uint16_t entry = *(uint16_t *)&sector[ent_off];

        if (cluster & 1)
                entry >>= 4;
        else
                entry &= 0x0FFF;

        return entry;
}

static uint32_t next_cluster(fat_bpb_t *bt, uint32_t cluster, file_t *file)
{
        return next_cluster_32(bt, cluster, file);
}

static uint32_t find_free_cluster(fat_bpb_t *bt, file_t *file)
{
        const uint32_t total = cluster_count(bt);
        const uint32_t start = first_fat(bt);

        char sector[bt->sector_size];
        for (uint32_t i = 2; i < total + 2; i++)
        {
                uint32_t off = i * 4;
                uint32_t sec = start + (off / bt->sector_size);
                uint32_t ent = off % bt->sector_size;

                vfs_lseek(file, bt->sector_size * sec, SEEK_SET);
                vfs_read(file, sector, bt->sector_size);

                uint32_t entry = *(uint32_t *)&sector[ent];
                entry &= 0x0FFFFFFF;

                if (entry == 0)
                {
                        return i;
                }
        }
        return 0;
}

static void set_cluster(fat_bpb_t *bt, uint32_t cluster, uint32_t value, file_t *file)
{
        uint32_t off = cluster * 4;
        uint32_t sec = first_fat(bt) + (off / bt->sector_size);
        uint32_t ent = off % bt->sector_size;
        char sector[bt->sector_size];
        vfs_lseek(file, bt->sector_size * sec, SEEK_SET);
        vfs_read(file, sector, bt->sector_size);

        uint32_t *entry = (uint32_t *)&sector[ent];
        *entry = (*entry & 0xF0000000) | (value & 0x0FFFFFFF);

        vfs_lseek(file, bt->sector_size * sec, SEEK_SET);
        vfs_write(file, sector, sizeof(sector));
        uint32_t fat2_sector = sec + size(bt);
        vfs_lseek(file, bt->sector_size * fat2_sector, SEEK_SET);
        vfs_write(file, sector, sizeof(sector));
}

__attribute__((__used__))
static uint32_t allocate_cluster(fat_bpb_t *bt, file_t *file)
{
        uint32_t free = find_free_cluster(bt, file);
        if (free == 0)
        {
                return 0;
        }

        set_cluster(bt, free, 0x0FFFFFF8, file);
        return free;
}

static fat_dir_t fetch_directory(fat_bpb_t *bt, uint32_t cluster, file_t *file, size_t entry)
{
        const uint32_t first = first_sector_for_cluster(bt, cluster);
        const size_t   entries = bt->sector_size / sizeof(fat_dir_t);
        const size_t   sec     = entry / entries; 
        const size_t   index   = entry % entries;
        char sector[bt->sector_size];

        vfs_lseek(file, bt->sector_size * (first+sec), SEEK_SET);
        vfs_read(file, sector, sizeof(sector));
        fat_dir_t *dir = (fat_dir_t *)sector;

        return dir[index];
}

static int iterate_directory(fat_bpb_t *bt, uint32_t dir, file_t *file, int (*callback)(uint32_t,fat_dir_t *,size_t))
{
        uint32_t cluster = dir;
        const size_t entries = bt->sector_size / sizeof(fat_dir_t);
        char sector[bt->sector_size];
        while (cluster < 0x0FFFFFF8)
        {
                uint32_t first = first_sector_for_cluster(bt, cluster);

                for (uint32_t s = 0; s < bt->cluster_size; s++)
                {
                        vfs_lseek(file, bt->sector_size * (first+s), SEEK_SET);
                        vfs_read(file, sector, sizeof(sector));
                        fat_dir_t *dir = (fat_dir_t *)sector;

                        for (size_t i = 0; i < entries; i++)
                        {
                                if (dir[i].name[0] == 0x00)
                                        return 0;
                                if (dir[i].name[0] == 0xE5)
                                        continue;
                                if (dir[i].flags == FAT_LONG_FILE_NAME)
                                {
                                        LOG(" [krnl] error: <long file name>s are not supported\r\n");
                                        continue;
                                }
                                if (dir[i].flags & FAT_VOLUME_ID)
                                        continue;
                                size_t entry = s * entries + i;
                                int res = callback(cluster,&dir[i],entry);

                                if (res < 0)
                                        return res;
                        }
                }

                cluster = next_cluster(bt, cluster, file);
        }

        return 0;
}

static int read(file_t *file, void *buf, size_t n)
{
        fat_file_location_t *location = file->vnode->private;
        struct private *priv = location->_priv;
        fat_dir_t dir = fetch_directory(&priv->bpb,
                                    location->cluster,
                                    priv->base,
                                    location->index);
        uint32_t first_cluster =
        ((uint32_t)dir.entry_first_cluster_high << 16) |
         dir.entry_first_cluster_low;
        if ((uint32_t)file->offset >= (uint32_t)dir.size)
                return 0;
        if (file->offset + n > dir.size)
                n = dir.size - file->offset;
        uint8_t *out = buf;
        uint32_t cluster = first_cluster;
        uint32_t cluster_bytes =
        priv->bpb.sector_size * priv->bpb.cluster_size;
        uint32_t skip = file->offset / cluster_bytes;
        for (uint32_t i = 0; i < skip; i++)
                cluster = next_cluster(&priv->bpb, cluster, priv->base);
        uint32_t cluster_offset = file->offset % cluster_bytes;
        while (n)
        {
                uint32_t first_sector =
                        first_sector_for_cluster(&priv->bpb, cluster);
                uint32_t sector =
                        first_sector +
                        (cluster_offset / priv->bpb.sector_size);

                uint32_t offset =
                        cluster_offset % priv->bpb.sector_size;
                uint8_t sector_buf[priv->bpb.sector_size];
                vfs_lseek(priv->base,
                          sector * priv->bpb.sector_size,
                          SEEK_SET);
                vfs_read(priv->base,
                         sector_buf,
                         sizeof(sector_buf));
                size_t copy = priv->bpb.sector_size - offset;
                if (copy > n)
                        copy = n;
                memcpy(out, sector_buf + offset, copy);
                out += copy;
                n -= copy;
                cluster_offset += copy;
                if (cluster_offset >= cluster_bytes && n)
                {
                        cluster = next_cluster(&priv->bpb,
                                               cluster,
                                               priv->base);
                        cluster_offset = 0;
                }
        }

        size_t done = (uint8_t *)out - (uint8_t *)buf;
        file->offset += done;
        return done;
}

static file_ops_t ops = {
        .read = read,
};

static uint32_t dir_cluster = 0;
static vnode_t *current_parent = NULL;
static struct private *_priv = NULL;

static char tolower(char x)
{
        return x >= 'A' && x <= 'Z' ? x | 0x20 : x;
}

static bool is_dot_entry(fat_dir_t *dir)
{
    return dir->name[0] == '.' &&
           dir->name[1] == ' ' &&
           dir->name[2] == ' ' &&
           dir->name[3] == ' ' &&
           dir->name[4] == ' ' &&
           dir->name[5] == ' ' &&
           dir->name[6] == ' ' &&
           dir->name[7] == ' ';
}

static bool is_dotdot_entry(fat_dir_t *dir)
{
    return dir->name[0] == '.' &&
           dir->name[1] == '.' &&
           dir->name[2] == ' ' &&
           dir->name[3] == ' ' &&
           dir->name[4] == ' ' &&
           dir->name[5] == ' ' &&
           dir->name[6] == ' ' &&
           dir->name[7] == ' ';
}

static int add_file(uint32_t clus, fat_dir_t *dir, size_t index)
{
        if (is_dot_entry(dir) || is_dotdot_entry(dir))
                return 0;
        char *name = kmalloc(9+4);
        int i;
        for (i = 0; i < 8 && dir->name[i] != ' ';++i)
        {
                name[i] = tolower(dir->name[i]);
        }

        if (dir->ext[0] > 32)
        {
                name[i++] = '.';
                for (int j=0; j < 3 && dir->ext[j] != ' ';++i,++j)
                {
                        name[i] = tolower(dir->ext[j]);
                }
        }

        vnode_t *save = current_parent;
        vnode_t *node = vfs_create_in(current_parent, name, 0);
        node->ops = &ops;
        node->private = kmalloc(sizeof(fat_file_location_t));
        ((fat_file_location_t *)node->private)->_priv   = _priv;
        ((fat_file_location_t *)node->private)->cluster = clus;
        ((fat_file_location_t *)node->private)->index   = index;
        int res = 0;
        if (dir->flags & FAT_DIRECTORY)
        {
                node->flags |= VFS_DIRECTORY;
                current_parent = node;
                uint32_t cluster =
                        ((uint32_t)dir->entry_first_cluster_high << 16) |
                        (uint32_t)dir->entry_first_cluster_low;
                res = iterate_directory(&_priv->bpb,
                                        cluster,
                                        _priv->base,
                                        add_file);
        }

        current_parent = save;
        (void)node;
        return res;
}

static int mount(vnode_t *node, vnode_t *in)
{
        current_parent = node;
        struct private *priv = kmalloc(sizeof(*priv));
        if (vfs_open_direct(in, &priv->base) < 0)
                return -1;
        _priv = priv;
        priv->bpb = read_boot_sector(priv->base);
        priv->type = identify(&priv->bpb);
        node->private = priv;
        if (priv->type != FAT_32)
        {
                LOG(" [krnl] error: fat of type %d provided, only supports fat32, dumping bt\r\n", priv->type);
                dump(&priv->bpb, 512);
                LOG("._0XEBXX90: [%x, %x, %x]\r\n",
                        priv->bpb._0XEBXX90[0] & 255,
                        priv->bpb._0XEBXX90[1] & 255,
                        priv->bpb._0XEBXX90[2] & 255);
                LOG(".OEM: \"%s\"\r\n", priv->bpb.OEM);
                LOG(".sector_size: %d\r\n", priv->bpb.sector_size);
                LOG(".cluster_size: %d\r\n", priv->bpb.cluster_size);
                LOG(".reserved_sectors: %d\r\n", priv->bpb.reserved_sectors);
                LOG(".number_of_fats: %d\r\n", priv->bpb.number_of_fats);
                LOG(".root_dir_entries: %d\r\n", priv->bpb.root_dir_entries);
                LOG(".short_sector_count: %d\r\n", priv->bpb.short_sector_count);
                LOG(".type: %d\r\n", priv->bpb.type);
                LOG(".sectors_per_fat: %d\r\n", priv->bpb.sectors_per_fat);
                LOG(".sectors_per_track: %d\r\n", priv->bpb.sectors_per_track);
                LOG(".head_count: %d\r\n", priv->bpb.head_count);
                LOG(".hidden: %d\r\n", priv->bpb.hidden);
                LOG(".long_sector_count: %d\r\n", priv->bpb.long_sector_count);
                return -1;
        }
        (void)node;
        (void)in;
        dir_cluster=root_cluster(&priv->bpb);
        return iterate_directory(&priv->bpb,
                                 root_cluster(&priv->bpb),
                                 priv->base,
                                 add_file);
}

filesystem_t fat_create_fs(void)
{
        return (filesystem_t){
                .name="fat",
                .mount=mount,
        };
}
