#include <glibc-fs/prelude.h>
#include <inttypes.h>

intmax_t imaxabs (intmax_t n) {}

imaxdiv_t imaxdiv (intmax_t numer, intmax_t denom) {}

intmax_t strtoimax (const char $_1 * nptr, char $_1 ** endptr, int base) {}

uintmax_t strtoumax (const char $_1 * nptr, char $_1 **  endptr, int base) {}

intmax_t wcstoimax (const __gwchar_t $_1 * nptr, __gwchar_t $_1 ** endptr, int base) {}

uintmax_t wcstoumax (const __gwchar_t $_1 * nptr, __gwchar_t $_1 **  endptr, int base) {}

// this should be in stdlib.h:
//  long int __strtol_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}
//  unsigned long int __strtoul_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}
//  long long int __strtoll_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}
//  unsigned long long int __strtoull_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}

// this should be in wchar.h:
//  long int __wcstol_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}
//  unsigned long long int __wcstoull_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}
//  long long int __wcstoll_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}
//  unsigned long long int __wcstoull_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}
