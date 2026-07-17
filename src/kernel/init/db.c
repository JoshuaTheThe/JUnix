
#include <init/db.h>
#include <dbg.h>

#define l(n) (sizeof((n)) / sizeof((n)[0]))

static db_type_t table_users_geometry[]        = { db_text, db_text, db_integer };
static db_type_t table_applications_geometry[] = { db_text, db_text, db_integer, db_integer };
static db_type_t table_policies_geometry[]     = { db_integer, db_text, db_integer, db_integer };

bool db_init(void)
{
        vnode_t *node;
        filesystem_t *dbfs = get_file_system("dbfs");
        LOG(" [db] mounting db\r\n");
        vfs_create("/", "db", 0);
        if (vfs_lookup("/db", &node) < 0)
                return false;
        if (vfs_mount("/db", dbfs, node) < 0)
                return false;

        LOG(" [db] creating system database\r\n");
        vnode_t *system_database = db_create_db(node, "sys", 0);

        /**
          * +-USERNAME-+-PASSWRDHASH-+--POLICY--+
          * | root     | hash(123)   | 1        |
          */
        LOG(" [db] creating 'users' table\r\n");
        tabl_id_t users = db_create_table(system_database, "users");
        if (!db_set_table_geometry(system_database, users, l(table_users_geometry), table_users_geometry))
                return false;

        /**
          * +-NAME-----+-EXECUTABLE--+-OWNER-+--POLICY--+
          * | cc       | /mnt/bin/cc | 0     | 0        |
          */
        LOG(" [db] creating 'applications' table\r\n");
        tabl_id_t applications = db_create_table(system_database, "applications");
        if (!db_set_table_geometry(system_database, applications, l(table_applications_geometry), table_applications_geometry))
                return false;

        /** where by default its deny
          * +-POLICY-----+-RESOURCE-----+--ACCESS--+--TYPE---+
          * | 0          | /home/source | rw       | *       |
          * | 0          | /dev/kbd     | r        | *       |
          * | 1          | /home        | rw       | *       |
          */
        LOG(" [db] creating 'policies' table\r\n");
        tabl_id_t policies = db_create_table(system_database, "policies");
        if (!db_set_table_geometry(system_database, policies, l(table_policies_geometry), table_policies_geometry))
                return false;
        return true;
}
