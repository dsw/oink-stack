#include <glibc-fs/prelude.h>
#include <sched.h>

int sched_setparam (__pid_t pid, const struct sched_param *param) {}

int sched_getparam (__pid_t pid, struct sched_param *param) {}

int sched_setscheduler (__pid_t pid, int policy,
                        const struct sched_param *param) {}

int sched_getscheduler (__pid_t pid) {}

int sched_yield (void) {}

int sched_get_priority_max (int algorithm) {}

int sched_get_priority_min (int algorithm) {}

int sched_rr_get_interval (__pid_t pid, struct timespec *t) {}

int sched_setaffinity (__pid_t pid, size_t __cpusetsize, const cpu_set_t *mask) {}

int sched_getaffinity (__pid_t pid, size_t __cpusetsize, cpu_set_t *mask) {}
