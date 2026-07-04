
#ifndef USER_H
#define USER_H

// TODO - load users from file

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t userid_t;

typedef struct _user_t
{
        char name[32];
        char passhash[32];
        bool r,w,x,admin;
        struct _user_t *next;
} user_t;

#endif
