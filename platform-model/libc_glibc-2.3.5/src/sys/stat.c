#include <glibc-fs/prelude.h>
#define __inline__
#include <sys/stat.h>

// TODO: is it worth tainting contents of stat buffer?

// #undef stat
// int stat (const char * file, struct stat * buf) {}

// #undef fstat
// int fstat (int fd, struct stat *buf) {}

// #undef stat64
// int stat64 (const char * file, struct stat64 * buf) {}

// #undef fstat64
// int fstat64 (int fd, struct stat64 *buf) {}

// #undef lstat
// int lstat (const char * file, struct stat * buf) {}

// #undef lstat64
// int lstat64 (const char * file, struct stat64 * buf) {}

#undef chmod
int chmod (const char *file, __mode_t mode) {}

#undef lchmod
int lchmod (const char *file, __mode_t mode) {}

#undef fchmod
int fchmod (int fd, __mode_t mode) {}

#undef umask
__mode_t umask (__mode_t mask) {}

#undef getumask
__mode_t getumask (void) {}

#undef mkdir
int mkdir (const char *path, __mode_t mode) {}

// #undef mknod
// int mknod (const char *path, __mode_t mode, __dev_t dev) {}

#undef mkfifo
int mkfifo (const char *path, __mode_t mode) {}

#undef __fxstat
int __fxstat (int ver, int fildes, struct stat *stat_buf) {}
#undef __xstat
int __xstat (int ver, const char *filename, struct stat *stat_buf) {}
#undef __lxstat
int __lxstat (int ver, const char *filename, struct stat *stat_buf) {}

#undef __fxstat64
int __fxstat64 (int ver, int fildes, struct stat64 *stat_buf) {}

#undef __xstat64
int __xstat64 (int ver, const char *filename, struct stat64 *stat_buf) {}

#undef __lxstat64
int __lxstat64 (int ver, const char *filename, struct stat64 *stat_buf) {}

#undef __mknod
int __xmknod (int ver, const char *path, __mode_t mode, __dev_t *dev) {}

// Linux 2.6.xx (Fedora Core 5) has __fxstatat, __fxstatat64, mknodat
#undef __fxstatat
int __fxstatat (int ver, int fildes, const char *filename,
                struct stat *stat_buf, int flag) {}

#undef __fxstatat
int __fxstatat64 (int ver, int fildes, const char *filename,
                  struct stat64 *stat_buf, int flag) {}

// #undef mknodat
// int mknodat (int fd, const char *path, __mode_t mode, __dev_t dev) {}

#undef __xmknodat
int __xmknodat (int ver, int fd, const char *path,
                __mode_t mode, __dev_t *dev) {}
