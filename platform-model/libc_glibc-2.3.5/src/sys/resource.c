#include <glibc-fs/prelude.h>
#include <sys/resource.h>

int getrlimit (__rlimit_resource_t resource, struct rlimit *rlimits) {}
int getrlimit64 (__rlimit_resource_t resource, struct rlimit64 *rlimits) {}

int setrlimit (__rlimit_resource_t resource, const struct rlimit *rlimits) {}
int setrlimit64 (__rlimit_resource_t resource, const struct rlimit64 *rlimits) {}

int getrusage (__rusage_who_t who, struct rusage *usage) {}

int getpriority (__priority_which_t which, id_t who) {}

int setpriority (__priority_which_t which, id_t who, int prio) {}
