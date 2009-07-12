#include <glibc-fs/prelude.h>
#include <sys/sysctl.h>

int sysctl (int *name, int nlen, void $tainted *oldval,
            size_t *oldlenp, void *newval, size_t newlen) {}
