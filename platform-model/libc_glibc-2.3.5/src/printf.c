#include <glibc-fs/prelude.h>
#include <printf.h>
#include <stdio.h>

// struct printf_info
// {
//     int prec;
//     int width;
//     wchar_t spec;
//     unsigned int is_long_double:1;
//     unsigned int is_short:1;
//     unsigned int is_long:1;
//     unsigned int alt:1;
//     unsigned int space:1;
//     unsigned int left:1;
//     unsigned int showsign:1;
//     unsigned int group:1;
//     unsigned int extra:1;
//     unsigned int is_char:1;
//     unsigned int wide:1;
//     unsigned int i18n:1;
//     wchar_t pad;
// };

// typedef int printf_function (FILE *stream, const struct printf_info *info, const void *const *__args);

// typedef int printf_arginfo_function (const struct printf_info *info, size_t n, int *argtypes);

int register_printf_function (int spec, printf_function func, printf_arginfo_function arginfo) {
    struct printf_info pinfo;
    char $tainted ** p;
    func(stdin, &pinfo, (const void **) p);
}

size_t parse_printf_format (const char $untainted * fmt, size_t n, int * argtypes) {}

int printf_size (FILE * fp, const struct printf_info *info, const void *const * __args) {}

int printf_size_info (const struct printf_info * info, size_t n, int * argtypes) {}
