
#include <vfs/vmount.h>
#include <vmem/alloc.h>

MOUNTM *Methods = NULL;

MOUNTM *CreateMountMethod(
        bool (*onmount)(VNode *Media, VNode *In),
        bool (*onumount)(VNode *Mnt),
        bool (*oncommit)(VNode *Mnt),
        bool (*onfetch)(VNode *Mnt))
{
        MOUNTM *MountPoint = kalloc(sizeof(*MountPoint));
        MountPoint->mount  = onmount;
        MountPoint->umount = onumount;
        MountPoint->fetch  = onfetch;
        MountPoint->commit = oncommit;
        MountPoint->NextMethod = Methods;
        Methods = MountPoint;
        return MountPoint;
}

void DeleteMountMethod(MOUNTM *M)
{
        if (M == NULL) return;
        MOUNTM **prev = &Methods;
        MOUNTM  *curr = Methods;
        
        while (curr)
        {
                if (curr == M)
                {
                        *prev = curr->NextMethod;
                        kfree(curr);
                        return;
                }

                prev = &curr->NextMethod;
                curr = curr->NextMethod;
        }
}

void Mount(VNode *Media, VNode *Parent)
{
        MOUNTM *method = Methods;
        while (method)
        {
                if (method->mount(Media, Parent))
                {
                        Parent->Flags |= VFS_MOUNTED;
                        Parent->Link = (void*)method;
                        return;
                }
                method = method->NextMethod;
        }
}

void UMount(VNode *MountPoint)
{
        if (!(MountPoint->Flags & VFS_MOUNTED)) return;
        VNode *fs_root = MountPoint->Link;
        if (!fs_root) return;
        MOUNTM *method = MountPoint->DriverData;
        if (method && method->umount)
        {
                method->umount(MountPoint);
        }
        
        DeleteVNode(fs_root);
        MountPoint->Link = NULL;
        MountPoint->Flags &= ~VFS_MOUNTED;
        MountPoint->DriverData = NULL;
}
