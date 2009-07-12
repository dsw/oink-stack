#include <glibc-fs/prelude.h>
#include <glob.h>

// typedef struct
// {
//     __size_t gl_pathc;
//     char **gl_pathv;
//     __size_t gl_offs;
//     int gl_flags;

//     void (*gl_closedir) (void *);
//     struct dirent *(*gl_readdir) (void *);
//     __ptr_t (*gl_opendir) (const char *);
//     int (*gl_lstat) (const char *, struct stat *);
//     int (*gl_stat) (const char *, struct stat *);
// } glob_t;

// typedef struct
// {
//     __size_t gl_pathc;
//     char **gl_pathv;
//     __size_t gl_offs;
//     int gl_flags;

//     void (*gl_closedir) (void *);
//     struct dirent64 *(*gl_readdir) (void *);
//     __ptr_t (*gl_opendir) (const char *);
//     int (*gl_lstat) (const char *, struct stat64 *);
//     int (*gl_stat) (const char *, struct stat64 *);
// } glob64_t;

int glob (const char * pattern, int flags, int (*errfunc) (const char *, int), glob_t * pglob) {
    __DO_TAINT(pglob->gl_pathv[0]);
    char $tainted *p;
    errfunc(p, 0);
}

void globfree (glob_t *pglob) {}

int glob64 (const char * pattern, int flags, int (*errfunc) (const char *, int), glob64_t * pglob) {
    __DO_TAINT(pglob->gl_pathv[0]);
    char $tainted *p;
    errfunc(p, 0);
}

void globfree64 (glob64_t *pglob) {}

int glob_pattern_p (const char *pattern, int quote) {}
