
#ifndef PROC_H
#define PROC_H

#include <sched/core.h>

bool process_exists(pid_t pid);
bool process_alive(pid_t pid);

#endif
