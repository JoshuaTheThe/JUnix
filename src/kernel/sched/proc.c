
#include <sched/core.h>
#include <sched/proc.h>
#include <sys.h>

bool process_exists(pid_t pid)
{
        vnode_t *proc = scheduler_find_process(pid);
        if (!proc || !((task_t *)proc->private)->active)
                return false;
        return true;
}
