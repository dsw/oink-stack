#include <glibc-fs/prelude.h>
#include <fcntl.h>

// fcntl is system-dependent, so taint args just in case
int fcntl (int fd, int cmd, ... $tainted) {}

// alias for fcntl, used in e.g. xview
int __fcntl (int fd, int cmd, ... $tainted) {}

int open (const char *file, int oflag, ...) {}
int open64 (const char *file, int oflag, ...) {}

int creat (const char *file, __mode_t mode) {}
int creat64 (const char *file, __mode_t mode) {}

int lockf (int fd, int cmd, __off_t len) {}
int lockf64 (int fd, int cmd, __off64_t len) {}

int posix_fadvise (int fd, __off_t offset, __off_t len, int advise) {}
int posix_fadvise64 (int fd, __off64_t offset, __off64_t len, int advise) {}

int posix_fallocate (int fd, __off_t offset, __off_t len) {}
int posix_fallocate64 (int fd, __off64_t offset, __off64_t len) {}

ssize_t readahead (int fd, __off64_t offset, size_t count) {}
