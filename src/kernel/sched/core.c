
#include <sched/core.h>
#include <fs/fs.h>
#include <panic.h>
#include <string.h>
#include <mm/alloc.h>
#include <drivers/kprint.h>

task_state_registers_t scratch_proc = {0};
uint64_t ticks_since_boot = 0;

vnode_t *proc = NULL, *current_process_fil = NULL;

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

// simple robin round scheduler
// iterate to next, wrap around to first
void scheduler_next(void)
{
        not_optional(current_process_fil);
        not_optional(proc);
        current_process_fil = current_process_fil->next;
        if (!current_process_fil)
                current_process_fil = proc->children;
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
        
        vnode_t *vn = vfs_mkdir(proc, name, VFS_DIRECTORY);
        not_optional(vn);
        vn->private = task;
        return vn;
}

int scheduler_mount(vnode_t *node, void *data) // not allowed to unmount
{
        (void)node;
        (void)data;
        node->ops = &ops;
        current_process_fil = scheduler_add_process((task_state_registers_t){0}, "krnl");
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
        kprint(" [krnl] scheduler started\r\n");
}

