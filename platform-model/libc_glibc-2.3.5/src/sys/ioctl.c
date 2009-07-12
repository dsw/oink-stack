#include <glibc-fs/prelude.h>
#include <sys/ioctl.h>

int ioctl (int fd, unsigned long int request, ... $tainted) {
    va_list arg;
    va_start(arg, request);
    __taint_vararg_environment(arg);
    va_end(arg);
}
