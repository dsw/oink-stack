#include <glibc-fs/prelude.h>
#include <sys/wait.h>

__pid_t wait (__WAIT_STATUS stat_loc) {}

__pid_t waitpid (__pid_t pid, int *stat_loc, int options) {}

int waitid (idtype_t idtype, __id_t id, siginfo_t *infop, int options) {}

__pid_t wait3 (__WAIT_STATUS stat_loc, int options, struct rusage * usage) {}

__pid_t wait4 (__pid_t pid, __WAIT_STATUS stat_loc, int options, struct rusage *usage) {}
