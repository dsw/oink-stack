#include <glibc-fs/prelude.h>
#include <sys/profil.h>

// struct prof
// {
//     void *pr_base;
//     size_t pr_size;
//     size_t pr_off;
//     unsigned long int pr_scale;
// };

int sprofil (struct prof *profp, int profcnt, struct timeval *tvp, unsigned int flags)
{
    __DO_TAINT( profp->pr_base );
}
