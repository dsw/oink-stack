#include <glibc-fs/prelude.h>
#include <sys/statvfs.h>

int statvfs (const char * file, struct statvfs * buf) {}

int statvfs64 (const char * file, struct statvfs64 * buf) {}

int fstatvfs (int fildes, struct statvfs *buf) {}

int fstatvfs64 (int fildes, struct statvfs64 *buf) {}
