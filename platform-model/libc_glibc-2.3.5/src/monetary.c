#include <glibc-fs/prelude.h>
#include <monetary.h>

FAKEBUILTIN
ssize_t strfmon (char * s, size_t maxsize, const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_string_from_vararg(s, arg);
    va_end(arg);
}

// quarl 2006-07-06
//    'vstrfmon' is not a real glibc function; it was invented so that
//    __builtin_strfmon would have something to call with a
//    __builtin_va_list.
ssize_t vstrfmon(char * s, size_t maxsize, const char $untainted * format, va_list arg) {
    __taint_string_from_vararg(s, arg);
}

ssize_t strfmon_l (char * s, size_t maxsize, __locale_t loc, const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_string_from_vararg(s, arg);
    va_end(arg);
}
