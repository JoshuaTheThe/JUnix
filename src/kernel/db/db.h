
/**
 * J/Unix Database Manager
 * v1
 */

#ifndef _DB_H
#define _DB_H

#include <stdint.h>
#include <stdbool.h>
#include <fs/fs.h>

#define NONE (UINT64_MAX)

#define COL_BITS (5)
#define COL_MASK ((1 << COL_BITS) - 1)
#define MAX_COLS (1 << COL_BITS)

typedef uint64_t generic_id_t;
typedef generic_id_t  string_id_t;
typedef generic_id_t  tabl_id_t;
typedef generic_id_t  row_id_t;

typedef enum
{
        db_null,
        db_integer,
        db_text,
        db_timestamp,
} db_type_t;

typedef struct
{
        union
        {
                int32_t integer;
                string_id_t text;
                uint64_t timestamp;
        };
} db_cell_t;

typedef struct
{
        size_t  len;
        char   *data;
} db_str_t;

typedef struct
{
        string_id_t name;
        size_t      row_count;
        size_t      row_capacity;
        size_t      col_count;
        db_type_t  *col_types;
        db_cell_t  *cells; // IX = Y * COLS + X (Y IS BOUNDED BY: ROWS)
} db_table_t;

typedef struct
{
        size_t      table_count;
        db_table_t *tables;
        size_t      string_count;
        size_t      string_capacity;
        db_str_t   *strings;
} db_t;

filesystem_t db_create_fs(void);
vnode_t     *db_create_db(vnode_t *parent, char *name, uint32_t flags);
tabl_id_t    db_create_table(vnode_t *db, char *name);
bool         db_set_table_geometry(vnode_t *db, tabl_id_t id, size_t count, db_type_t geometry[static count]); // must be clear
bool         db_clear_table(vnode_t *db, tabl_id_t id);
row_id_t     db_insert_row(vnode_t *db, tabl_id_t id, size_t count, db_cell_t data[static count]);
bool         db_fetch_row(vnode_t *db, tabl_id_t t_id, row_id_t r_id, size_t count, db_cell_t data[static count]);
bool         db_update_row(vnode_t *db, tabl_id_t t_id, row_id_t r_id, size_t count, db_cell_t data[static count]);
string_id_t  db_add_string(vnode_t *db, size_t len, char data[static len]);
bool         db_get_string(vnode_t *db, string_id_t id, size_t len, char data[static len]);

#endif
