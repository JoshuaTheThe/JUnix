
#include <db/db.h>
#include <string.h>
#include <stdlib.h>
#include <mm/alloc.h>

#define db_alloc(x)      kmalloc(x)
#define db_calloc(n, x)  kmalloc((n)*(x))
#define db_realloc(p,x)  krealloc(p, x)
#define db_free(x)       kfree(x)

// operations on /db
static file_ops_t dbsops =
{
        .read = NULL,
        .write = NULL,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
        .mkdir = db_create_db,
};

// operations on /db/*
static file_ops_t dbops =
{
        .read = NULL,
        .write = NULL,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
        .mkdir = NULL,
};

static int mount(vnode_t *mountpoint, vnode_t *source)
{
        (void)mountpoint;
        (void)source;
        mountpoint->ops = &dbsops;
        return 0;
}

filesystem_t db_create_fs(void)
{
        return (filesystem_t){
                .mount = mount,
                .name  = "dbfs",
        };
}

static db_t *db_get(vnode_t *node)
{
        return (db_t *)node->priv;
}


static db_table_t *db_table(vnode_t *db, tabl_id_t id)
{
        db_t *database = db_get(db);

        if (!database)
                return NULL;

        if (id >= database->table_count)
                return NULL;

        return &database->tables[id];
}


vnode_t *db_create_db(vnode_t *parent, char *name, uint32_t flags)
{
        vnode_t *node = vfs_create_in(parent, name, flags);

        if (!node)
                return NULL;

        db_t *database = db_calloc(1, sizeof(db_t));

        if (!database)
                return NULL;

        node->priv = database;
        node->ops = &dbops;

        return node;
}


tabl_id_t db_create_table(vnode_t *db, char *name)
{
        db_t *database = db_get(db);

        if (!database)
                return NONE;

        size_t id = database->table_count;

        db_table_t *tables = db_realloc(
                database->tables,
                sizeof(db_table_t) * (id + 1)
        );

        if (!tables)
                return NONE;

        database->tables = tables;
        database->table_count++;

        db_table_t *table = &tables[id];

        memset(table, 0, sizeof(*table));

        table->name = db_add_string(
                db,
                strlen(name),
                name
        );

        if (table->name == NONE)
                return NONE;

        return id;
}


bool db_set_table_geometry(
        vnode_t *db,
        tabl_id_t id,
        size_t count,
        db_type_t geometry[static count]
)
{
        db_table_t *table = db_table(db, id);

        if (!table)
                return false;

        if (count > MAX_COLS)
                return false;

        /*
         * Table geometry can only be defined once.
         */
        if (table->col_types)
                return false;

        table->col_types = db_alloc(
                sizeof(db_type_t) * count
        );

        if (!table->col_types)
                return false;

        memcpy(
                table->col_types,
                geometry,
                sizeof(db_type_t) * count
        );

        table->col_count = count;

        return true;
}

bool db_clear_table(vnode_t *db, tabl_id_t id)
{
        db_table_t *table = db_table(db, id);

        if (!table)
                return false;

        db_free(table->cells);

        table->cells = NULL;
        table->row_count = 0;
        table->row_capacity = 0;

        return true;
}

row_id_t db_insert_row(
        vnode_t *db,
        tabl_id_t id,
        size_t count,
        db_cell_t data[static count]
)
{
        db_table_t *table = db_table(db, id);

        if (!table)
                return NONE;

        if (count != table->col_count)
                return NONE;


        if (table->row_count >= table->row_capacity)
        {
                size_t new_capacity =
                        table->row_capacity ?
                        table->row_capacity * 2 :
                        8;


                db_cell_t *cells = db_realloc(
                        table->cells,
                        sizeof(db_cell_t) *
                        new_capacity *
                        table->col_count
                );

                if (!cells)
                        return NONE;

                table->cells = cells;
                table->row_capacity = new_capacity;
        }


        row_id_t row = table->row_count;

        memcpy(
                &table->cells[row * table->col_count],
                data,
                sizeof(db_cell_t) * count
        );

        table->row_count++;

        return row;
}

bool db_fetch_row(
        vnode_t *db,
        tabl_id_t t_id,
        row_id_t r_id,
        size_t count,
        db_cell_t data[static count]
)
{
        db_table_t *table = db_table(db, t_id);

        if (!table)
                return false;

        if (count != table->col_count)
                return false;

        if (r_id >= table->row_count)
                return false;


        memcpy(
                data,
                &table->cells[r_id * table->col_count],
                sizeof(db_cell_t) * count
        );

        return true;
}

bool db_update_row(
        vnode_t *db,
        tabl_id_t t_id,
        row_id_t r_id,
        size_t count,
        db_cell_t data[static count]
)
{
        db_table_t *table = db_table(db, t_id);

        if (!table)
                return false;

        if (count != table->col_count)
                return false;

        if (r_id >= table->row_count)
                return false;


        memcpy(
                &table->cells[r_id * table->col_count],
                data,
                sizeof(db_cell_t) * count
        );

        return true;
}

string_id_t db_add_string(
        vnode_t *db,
        size_t len,
        char data[static len]
)
{
        db_t *database = db_get(db);

        if (!database)
                return NONE;


        if (database->string_count >= database->string_capacity)
        {
                size_t capacity =
                        database->string_capacity ?
                        database->string_capacity * 2 :
                        16;


                db_str_t *strings = db_realloc(
                        database->strings,
                        sizeof(db_str_t) * capacity
                );

                if (!strings)
                        return NONE;

                database->strings = strings;
                database->string_capacity = capacity;
        }


        string_id_t id = database->string_count;


        char *copy = db_alloc(len + 1);

        if (!copy)
                return NONE;


        memcpy(copy, data, len);
        copy[len] = 0;


        database->strings[id].len = len;
        database->strings[id].data = copy;

        database->string_count++;

        return id;
}

bool db_get_string(
        vnode_t *db,
        string_id_t id,
        size_t len,
        char data[static len]
)
{
        db_t *database = db_get(db);

        if (!database)
                return false;

        if (id >= database->string_count)
                return false;

        db_str_t *str = &database->strings[id];

        if (len < str->len + 1)
                return false;


        memcpy(
                data,
                str->data,
                str->len + 1
        );

        return true;
}
