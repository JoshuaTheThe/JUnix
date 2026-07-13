
#include <mm/alloc.h>
#include <mm/paging.h>
#include <cpu/cpu.h>
#include <sched/core.h>
#include <string.h>
#include <panic.h>
#include <dbg.h>

static proc_t *processes    = NULL, *tail = NULL;
static proc_t *kernel_proc  = NULL;
static task_t *kernel_task  = NULL;
task_t        *current_task = NULL;
proc_t        *current_proc = NULL;

uint64_t ticks_since_boot = 0;

static bool override_store = false; // cancel store to current proc, e.g. interrupt after exit()

task_state_registers_t scratch;

task_t *task_create(proc_t *proc)
{
        if (proc->taskcount >= MAX_TASKS)
                return NULL;
        task_t *task = &proc->tasks[proc->taskcount];

        uintptr_t phys = (uintptr_t)virt_to_phys(kernel_proc->space, pmm_alloc());
        task->kernel_stack = (void *)KERNEL_STACK_VIRT;
        paging_map(proc->space,
                   KERNEL_STACK_VIRT + PAGE_SIZE * proc->taskcount++,
                   phys,
                   PAGE_PRESENT | PAGE_WRITE);
        task->state = TASK_BLOCKED;
        task->parent = proc;
        return task;
}

proc_t *proc_create(void)
{
        static uint64_t pid = 0;
        proc_t *proc        = kmalloc(sizeof(*proc));
        proc->pid           = pid++;
        proc->taskcount     = 0;
        proc->cwd           = root_vnode;
        if (tail)
                tail->next = proc;
        proc->prev = tail;
        tail       = proc;
        if (!processes)
                processes = proc;
        return proc;
}

void proc_clear(proc_t *proc)
{
        memset(proc->tasks, 0, sizeof(proc->tasks));
        for (size_t i = 0; i < proc->fd.count; ++i)
        {
                vfs_close(proc->fd.items[i]);
        }

        memset(&proc->fd, 0, sizeof(proc->fd));
        paging_clear_address_space(proc->space);
        kfree(proc->space->items);
        memset(proc->space, 0, sizeof(*proc->space));
        kfree(proc->space);
        proc->space = NULL;
}

void proc_kill(proc_t *proc)
{
        proc_clear(proc);
        if (proc->next)
                proc->next->prev = proc->prev;
        if (proc->prev)
                proc->prev->next = proc->next;
        if (proc == current_proc)
        {
                override_store = true;
                cpu_ei();
                while (1)
                        ;
        }
}

void sched_next(void)
{
    for (;;)
    {
        size_t task_index = current_task - current_proc->tasks;

        if (task_index + 1 < current_proc->taskcount)
        {
            current_task = &current_proc->tasks[task_index + 1];
        }
        else
        {
            current_proc = current_proc->next;
            if (!current_proc)
                current_proc = processes;

            if (current_proc->taskcount == 0)
                continue;

            current_task = &current_proc->tasks[0];
        }

        if (current_task->state == TASK_RUNNING)
            break;
    }

    ackint();
}

void sched_load(void)
{
        if (!current_task)
                panic(PANIC_TODO);
        scratch = current_task->regs;
        paging_switch(current_proc->space);
}

void sched_save(void)
{
        if (!current_task)
                panic(PANIC_TODO);
        if (override_store)
                return;
        current_task->regs = scratch;
}

void sched_init(void)
{
        current_proc = kernel_proc = proc_create();
        kernel_proc->space = &kernel_address_space;
        current_task = kernel_task = task_create(kernel_proc);
        kernel_task->state = TASK_RUNNING;
        LOG(" [proc] OK\r\n");
}

int proc_open(proc_t *proc, char *path, int flags, int mode)
{
        (void)flags;
        (void)mode;
        size_t fd = 0;
        if (!proc->fd.items)
        {
                goto new;
        }

        for (fd = 0; fd < proc->fd.capacity; ++fd)
        {
                if (!proc->fd.items[fd])
                {
                        goto open;
                }
        }

new:
        if (fd == proc->fd.capacity || !proc->fd.items)
        {
                void *new = kmalloc(proc->fd.capacity + 8 * sizeof(file_t *));
                if (proc->fd.capacity > 0 && proc->fd.items)
                {
                        memcpy(new, proc->fd.items, proc->fd.capacity * sizeof(file_t *));
                        kfree(proc->fd.items);
                }
                proc->fd.capacity += 8;
                proc->fd.items = new;
                fd = proc->fd.capacity - 8;
        }

open:
        if (vfs_open(path, &proc->fd.items[fd]) < 0)
                return -1;
        return fd;
}

void proc_close(proc_t *proc, int fd)
{
        if ((size_t)fd >= proc->fd.capacity)
                panic(PANIC_TODO);
        file_t *file = proc->fd.items[fd];
        proc->fd.items[fd] = NULL;
        vfs_close(file);
}
