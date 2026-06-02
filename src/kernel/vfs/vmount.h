
#ifndef MOUNT_H
#define MOUNT_H

#include <vfs/vnode.h>
#include <vfs/vfd.h>

#include <stdbool.h>

// store MOUNTM as driverdata

// implemented per FS
typedef struct _MOUNTM
{
        // return false on "WRONG-FS"
        bool (*mount)(VNode *Media, VNode *In); // mount
        bool (*umount)(VNode *Mnt); // unmount, also destroy vnode tree
        bool (*commit)(VNode *Mnt); // write to disk
        bool (*fetch)(VNode *Mnt); // create tree
        struct _MOUNTM *NextMethod;
} MOUNTM;

MOUNTM *CreateMountMethod(
        bool (*onmount)(VNode *Media, VNode *In),
        bool (*onumount)(VNode *Mnt),
        bool (*oncommit)(VNode *Mnt),
        bool (*onfetch)(VNode *Mnt));

void DeleteMountMethod(MOUNTM *M);
void Mount(VNode *Media, VNode *Parent); // dispatch to all methods
void UMount(VNode *Media);

extern MOUNTM *Methods;

#endif
