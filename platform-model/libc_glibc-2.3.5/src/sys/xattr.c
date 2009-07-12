#include <glibc-fs/prelude.h>
#include <sys/xattr.h>

int setxattr (const char *path, const char *name,
              const void *value, size_t size, int flags) {}

int lsetxattr (const char *path, const char *name,
               const void *value, size_t size, int flags) {}

int fsetxattr (int fd, const char *name, const void *value,
               size_t size, int flags) {}

ssize_t getxattr (const char *path, const char *name,
                  void $tainted *value, size_t size) {}

ssize_t lgetxattr (const char *path, const char *name,
                   void $tainted *value, size_t size) {}

ssize_t fgetxattr (int fd, const char *name, void $tainted *value, size_t size) {}

ssize_t listxattr (const char *path, char $tainted *list, size_t size) {}

ssize_t llistxattr (const char *path, char $tainted *list, size_t size) {}

ssize_t flistxattr (int fd, char $tainted *list, size_t size) {}

int removexattr (const char *path, const char *name) {}

int lremovexattr (const char *path, const char *name) {}

int fremovexattr (int fd, const char *name) {}
