#include <glibc-fs/prelude.h>
#include <sys/sendfile.h>

ssize_t sendfile (int out_fd, int in_fd, off_t *offset, size_t count) {}

ssize_t sendfile64 (int out_fd, int in_fd, __off64_t *offset, size_t count) {}
