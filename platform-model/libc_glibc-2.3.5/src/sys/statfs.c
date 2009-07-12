#include <glibc-fs/prelude.h>
#include <sys/statfs.h>

int statfs (const char *file, struct statfs *buf) {}
int statfs64 (const char *file, struct statfs64 *buf) {}

int fstatfs (int fildes, struct statfs *buf) {}
int fstatfs64 (int fildes, struct statfs64 *buf) {}
