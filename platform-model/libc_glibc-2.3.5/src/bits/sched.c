#include <glibc-fs/prelude.h>

// quarl 2006-10-13
//     Some linux versions have the "..." argument and some don't; we avoid
//     including the system header so that gcc won't complain about the
//     mismatch.

// #include <sched.h>

int clone (int (*fn) (void *arg), void *child_stack, int flags, void *arg, ...)
{
    return fn(arg);
}
