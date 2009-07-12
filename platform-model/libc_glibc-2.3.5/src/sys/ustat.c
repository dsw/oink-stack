#include <glibc-fs/prelude.h>
#include <sys/ustat.h>

int ustat (__dev_t dev, struct ustat *ubuf)
{
    __DO_TAINT(ubuf->f_fname);
    __DO_TAINT(ubuf->f_fpack);
}
