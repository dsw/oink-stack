#include <glibc-fs/prelude.h>
#include <ftw.h>

// typedef int (*__ftw_func_t) (__const char *__filename,
//                              __const struct stat *__status, int __flag);

// struct FTW
// {
//     int base;
//     int level;
// };

// typedef int (*__nftw_func_t) (__const char *__filename,
//                               __const struct stat *__status, int __flag,
//                               struct FTW *__info);

int ftw (const char *dir, __ftw_func_t func, int descriptors)
{
    char $tainted *filename;
    func(filename, NULL, 0);
}

int ftw64 (const char *dir, __ftw64_func_t func, int descriptors)
{
    char $tainted *filename;
    func(filename, NULL, 0);
}

int nftw (const char *dir, __nftw_func_t func, int descriptors, int flag)
{
    char $tainted *filename;
    func(filename, NULL, 0, NULL);
}

int nftw64 (const char *dir, __nftw64_func_t func, int descriptors, int flag)
{
    char $tainted *filename;
    func(filename, NULL, 0, NULL);
}
