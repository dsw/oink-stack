#include <glibc-fs/prelude.h>

// quarl 2006-10-13
//     Don't include sys/mman.h since linux versions vary on the type
//     signature of mremap.

// #include <sys/mman.h>
#include <sys/types.h>

void $tainted *mmap (void $tainted *addr, size_t len, int prot, int flags, int fd, __off_t offset) {}

void $tainted *mmap64 (void $tainted *addr, size_t len, int prot, int flags, int fd, __off64_t offset) {}

int munmap (void *addr, size_t len) {}

int mprotect (void *addr, size_t len, int prot) {}

int msync (void *addr, size_t len, int flags) {}

int madvise (void *addr, size_t len, int advice) {}

int posix_madvise (void *addr, size_t len, int advice) {}

int mlock (const void *addr, size_t len) {}

int munlock (const void *addr, size_t len) {}

int mlockall (int flags) {}

int munlockall (void) {}

// void $tainted *mremap (void $tainted *addr, size_t old_len, size_t new_len, int may_move) {}
void $tainted *mremap (void $tainted *addr, size_t old_len, size_t new_len, int flags, ...) {}
// TODO: does "..." in mremap need $tainted?

int mincore (void *start, size_t len, unsigned char $tainted *vec) {}

int remap_file_pages (void $tainted *start, size_t size, int prot, size_t pgoff, int flags) {}

int shm_open (const char *name, int oflag, mode_t mode) {}

int shm_unlink (const char *name) {}
