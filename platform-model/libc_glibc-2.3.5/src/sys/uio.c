#include <glibc-fs/prelude.h>
#include <sys/uio.h>
// struct iovec
// {
//     void *iov_base;
//     size_t iov_len;
// };

ssize_t readv (int fd, const struct iovec *vector, int count)
{
    __DO_TAINT(vector->iov_base);
}

ssize_t writev (int fd, const struct iovec *vector, int count)
{}
