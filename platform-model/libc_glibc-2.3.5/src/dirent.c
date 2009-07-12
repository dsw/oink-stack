#include <glibc-fs/prelude.h>
#include <dirent.h>

// User does not see this so we shouldn't need to do anything within it:
//   typedef struct __dirstream DIR;

// struct dirent
// {
//     __ino64_t d_ino;
//     __off64_t d_off;
//     unsigned short int d_reclen;
//     unsigned char d_type;
//     char d_name[256];
// };

static inline void __taint_dirent(struct dirent* p)
{/*T:H*/
    // __DO_TAINT(&(p->d_type));
    __DO_TAINT(p->d_name);
}

static inline struct dirent* __get_tainted_dirent()
{/*T:H*/
    struct dirent* p;
    __taint_dirent(p);
    return p;
}

// struct dirent64
// {
//     __ino64_t d_ino;
//     __off64_t d_off;
//     unsigned short int d_reclen;
//     unsigned char d_type;
//     char d_name[256];
// };

static inline void __taint_dirent64(struct dirent64* p)
{/*T:H*/
    // __DO_TAINT(&(p->d_type));
    __DO_TAINT(p->d_name);
}

static inline struct dirent64* __get_tainted_dirent64()
{/*T:H*/
    struct dirent64* p;
    __taint_dirent64(p);
    return p;
}

DIR *opendir (const char *name) {}

int closedir (DIR *dirp) {}

struct dirent *readdir (DIR *dirp) { return __get_tainted_dirent(); }
struct dirent64 *readdir64 (DIR *dirp) { return __get_tainted_dirent64(); }

int readdir_r (DIR * dirp, struct dirent * entry, struct dirent ** result)
{
    __taint_dirent(entry);
    *result = entry;
}

int readdir64_r (DIR * dirp, struct dirent64 * entry, struct dirent64 ** result)
{
    __taint_dirent64(entry);
    *result = entry;
}

void rewinddir (DIR *dirp) {}

void seekdir (DIR *dirp, long int pos) {}

long int telldir (DIR *dirp) {}

int dirfd (DIR *dirp) {}

int scandir (const char * dir, struct dirent *** namelist,
             int (*__selector) (const struct dirent *),
             int (*__cmp) (const void *, const void *))
{
    __taint_dirent(**namelist);
    (*__selector)(**namelist);
    (*__cmp)(*namelist, *namelist);
}

int scandir64 (const char * dir, struct dirent64 *** namelist,
               int (*__selector) (const struct dirent64 *),
               int (*__cmp) (const void *, const void *))
{
    __taint_dirent64(**namelist);
    (*__selector)(**namelist);
    (*__cmp)(*namelist, *namelist);
}

int alphasort (const void *e1, const void *e2) {}

int alphasort64 (const void *e1, const void *e2) {}

int versionsort (const void *e1, const void *e2) {}

int versionsort64 (const void *e1, const void *e2) {}

__ssize_t getdirentries (int fd, char $tainted * buf, size_t nbytes, __off_t * basep) {}

__ssize_t getdirentries64 (int fd, char $tainted * buf, size_t nbytes, __off64_t * basep) {}
