
#include <sched/core.h>
#include <fs/fs.h>
#include <panic.h>
#include <string.h>
#include <mm/alloc.h>
#include <dbg.h>
#include <cpu/cpu.h>
#include <mm/paging.h>

task_state_registers_t scratch_proc = {0};
uint32_t ticks_since_boot = 0;

vnode_t *proc = NULL, *current_process_fil = NULL, *override_next = NULL;

task_t  early_task  = {0};
task_t *active_task;

int task_open(task_t *task, char *path, int flags, int mode)
{
        (void)flags;
        (void)mode;
        int fd;
        for (fd=0;!task->fd.items[fd]&&(size_t)fd<task->fd.capacity;++fd)
                ;
        if ((size_t)fd==task->fd.capacity||!task->fd.items)
        {
                void *new = kmalloc(task->fd.capacity + 8 * sizeof(file_t *));
                if (task->fd.capacity > 0 && task->fd.items)
                {
                        memcpy(new, task->fd.items, task->fd.capacity * sizeof(file_t *));
                        kfree(task->fd.items);
                }
                task->fd.capacity += 8;
                task->fd.items = new;
                fd = task->fd.capacity - 8;
        }

        if (vfs_open(path, &task->fd.items[fd]) < 0)
                return -1;
        return fd;
}

void task_close(task_t *task, int fd)
{
        if ((size_t)fd >= task->fd.capacity)
                panic(PANIC_TODO);
        file_t *file = task->fd.items[fd];
        task->fd.items[fd] = NULL;
        vfs_close(file);
}

vnode_t *scheduler_mkdir(vnode_t *p, char *name, uint32_t flags)
{
        vnode_t *new = kmalloc(sizeof(*new));
        new->name = name;
        new->flags = flags;
        new->ops = p->ops;
        vfs_append_child(p, new);
        return new;
}

static file_ops_t ops = {
        .length = NULL,
        .mkdir  = scheduler_mkdir,
        .readdir = NULL, // for now
        .read = NULL,
        .write = NULL,
        .open = NULL,
        .close = NULL,
        .lseek = NULL,
        .release = NULL,
        .capture = NULL,
};

void scheduler_load(void)
{
        task_t *task = (task_t *)current_process_fil->private;
        not_optional(task);
        memcpy(&scratch_proc, &task->regs, sizeof(task->regs));
}

void scheduler_next(void)
{
        not_optional(current_process_fil);
        not_optional(proc);
        vnode_t *to_remove = NULL;
        while(1)
        {
                if (!override_next)
                {
                        current_process_fil = current_process_fil->next;
                        if (!current_process_fil)
                                current_process_fil = proc->children;
                }
                else
                {
                        current_process_fil = override_next;
                        override_next = NULL;
                }

                if (current_process_fil &&
                   ((task_t *)current_process_fil->private)->active)
                        break;

                if (current_process_fil)
                {
                        to_remove = current_process_fil;
                        task_t *task = (task_t *)to_remove->private;
                        if (to_remove->next)
                                to_remove->next->prev = to_remove->prev;
                        if (to_remove->prev)
                                to_remove->prev->next = to_remove->next;
                        if (to_remove->parent->children == to_remove)
                                to_remove->parent->children = to_remove->next;
                        kfree(task);
                        kfree(to_remove);
                        current_process_fil = proc->children;
                }
                else
                {
                        break;
                }
        }

        active_task = current_process_fil->private;
        ackint();
}

void scheduler_save(void)
{
        task_t *task = (task_t *)current_process_fil->private;
        not_optional(task);
        memcpy(&task->regs, &scratch_proc, sizeof(task->regs));
}

vnode_t *scheduler_add_process(task_state_registers_t initial_regs, char *name)
{
        static uint64_t new_pid = 0;
        task_t *task = kmalloc(sizeof(*task));
        not_optional(task);
        task->regs = initial_regs;
        task->pid  = new_pid++;
        task->active = true;

        vnode_t *vn = vfs_mkdir(proc, name, VFS_DIRECTORY);
        not_optional(vn);
        vn->private = task;
        LOG(" [krnl] created process %s (%d)\r\n", vn->name, task->pid);
        return vn;
}

int scheduler_mount(vnode_t *node, void *data) // not allowed to unmount
{
        (void)node;
        (void)data;
        node->ops = &ops;
        current_process_fil = scheduler_add_process((task_state_registers_t){0}, "krnl");
        ((task_t *)current_process_fil->private)->mappings = early_task.mappings;
        ((task_t *)current_process_fil->private)->pd       = early_task.pd;
        active_task = current_process_fil->private;
        return 0;
}

static filesystem_t fs = {.name="proc/fs", .mount=scheduler_mount};

void scheduler_init(void)
{
        // stage #1, create proc dir
        // (if not found)
        if (vfs_lookup("/proc", &proc) < 0)
        {
                vnode_t *root;
                if (vfs_lookup("/",&root) < 0)
                        panic(PANIC_TODO);
                proc=scheduler_mkdir(root, "proc", VFS_DIRECTORY);
        }

        vfs_mount("/proc", &fs, NULL);
        LOG(" [krnl] scheduler started\r\n");
}

vnode_t *scheduler_find_process(pid_t pid)
{
        vnode_t *node = proc->children;
        while (node)
        {
                if (((task_t *)node->private)->pid == pid)
                        return node;
                node = node->next;
        }

        return NULL;
}
