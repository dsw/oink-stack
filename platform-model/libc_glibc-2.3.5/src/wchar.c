#include <glibc-fs/prelude.h>
#include <wchar.h>

wchar_t $_1_2 *wcscpy (wchar_t $_1_2 * dest, const wchar_t $_1 * src) {}

wchar_t $_1_2 *wcsncpy (wchar_t $_1_2 * dest, const wchar_t $_1 * src, size_t n) {}

wchar_t $_1_2 *wcscat (wchar_t $_1_2 * dest, const wchar_t $_1 * src) {}

wchar_t $_1_2 *wcsncat (wchar_t $_1_2 * dest, const wchar_t $_1 * src, size_t n) {}

int wcscmp (const wchar_t *s1, const wchar_t *s2) {}

int wcsncmp (const wchar_t *s1, const wchar_t *s2, size_t n) {}

int wcscasecmp (const wchar_t *s1, const wchar_t *s2) {}

int wcsncasecmp (const wchar_t *s1, const wchar_t *s2, size_t n) {}

int wcscasecmp_l (const wchar_t *s1, const wchar_t *s2, __locale_t loc) {}

int wcsncasecmp_l (const wchar_t *s1, const wchar_t *s2, size_t n, __locale_t loc) {}

int wcscoll (const wchar_t *s1, const wchar_t *s2) {}

size_t wcsxfrm (wchar_t $_1_2 * s1, const wchar_t $_1 * s2, size_t n) {} // ??

int wcscoll_l (const wchar_t *s1, const wchar_t *s2, __locale_t loc) {}

size_t wcsxfrm_l (wchar_t $_1_2 *s1, const wchar_t $_1 *s2, size_t n, __locale_t loc) {} // ??

wchar_t $_1_2 *wcsdup (const wchar_t $_1 *s) {}

wchar_t $_1 *wcschr (const wchar_t $_1 *wcs, wchar_t wc) {}

wchar_t $_1 *wcsrchr (const wchar_t $_1 *wcs, wchar_t wc) {}

wchar_t $_1 *wcschrnul (const wchar_t $_1 *s, wchar_t wc) {}

size_t wcscspn (const wchar_t *wcs, const wchar_t *reject) {}

size_t wcsspn (const wchar_t *wcs, const wchar_t *accept) {}

wchar_t $_1 *wcspbrk (const wchar_t $_1 *wcs, const wchar_t *accept) {}

wchar_t $_1_2 *wcsstr (const wchar_t $_1_2 *haystack, const wchar_t $_2 *needle) {}

// already re-entrant version
wchar_t $_1_2 *wcstok (wchar_t $_1_2 * s, const wchar_t $_1 * delim, wchar_t $_1_2 ** ptr) {}

size_t wcslen (const wchar_t *s) {}

// presumably same as wcsstr
wchar_t $_1_2 *wcswcs (const wchar_t $_1_2 *haystack, const wchar_t $_2 *needle) {}

size_t wcsnlen (const wchar_t *s, size_t maxlen) {}

wchar_t $_1 *wmemchr (const wchar_t $_1 *s, wchar_t c, size_t n) {}

int wmemcmp (const wchar_t * s1, const wchar_t * s2, size_t n) {}

wchar_t $_1_2 *wmemcpy (wchar_t $_1_2 * s1, const wchar_t $_1 * s2, size_t n) {}

wchar_t $_1_2 *wmemmove (wchar_t $_1_2 *s1, const wchar_t $_1 *s2, size_t n) {}

wchar_t $_1_2 *wmemset (wchar_t $_1_2 *s, wchar_t $_1 c, size_t n) {}

wchar_t $_1_2 *wmempcpy (wchar_t $_1_2 * s1, const wchar_t $_1 * s2, size_t n) {}

wint_t $_1_2 btowc (int $_1 c) {}

int $_1_2 wctob (wint_t $_1 c) {}

int mbsinit (const mbstate_t *ps) {}

size_t mbrtowc (wchar_t $_1_2 * pwc, const char $_1 * s, size_t n, mbstate_t *p) {}

size_t wcrtomb (char $_1_2 * s, wchar_t $_1 wc, mbstate_t * ps) {}

size_t __mbrlen (const char * s, size_t n, mbstate_t * ps) {}
size_t mbrlen (const char * s, size_t n, mbstate_t * ps) {}

size_t mbsrtowcs (wchar_t $_1_2 * dst, const char $_1 ** src, size_t len, mbstate_t * ps) {}

size_t wcsrtombs (char $_1_2 * dst, const wchar_t $_1 ** src, size_t len, mbstate_t * ps) {}

size_t mbsnrtowcs (wchar_t $_1_2 * dst, const char $_1 ** src, size_t nmc, size_t len, mbstate_t * ps) {}

size_t wcsnrtombs (char $_1_2 * dst, const wchar_t $_1 ** src, size_t nwc, size_t len, mbstate_t * ps) {}

int $_1_2 wcwidth (wchar_t $_1 c) {}

int $_1_2 wcswidth (const wchar_t $_1 *s, size_t n) {}

double wcstod (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr) {}

float wcstof (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr) {}
long double wcstold (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr) {}

long int wcstol (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base) {}

unsigned long int wcstoul (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base) {}

long long int wcstoll (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base) {}

unsigned long long int wcstoull (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base) {}

long long int wcstoq (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base) {}

unsigned long long int wcstouq (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base) {}

long int wcstol_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, __locale_t loc) {}

unsigned long int wcstoul_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, __locale_t loc) {}

long long int wcstoll_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, __locale_t loc) {}

unsigned long long int wcstoull_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, __locale_t loc) {}

double wcstod_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, __locale_t loc) {}

float wcstof_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, __locale_t loc) {}

long double wcstold_l (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, __locale_t loc) {}

double __wcstod_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int group) {}
float __wcstof_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int group) {}
long double __wcstold_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int group) {}

long int __wcstol_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}

unsigned long int __wcstoul_internal (const wchar_t $_1 * npt, wchar_t $_1 ** endptr, int base, int group) {}

long long int __wcstoll_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}

unsigned long long int __wcstoull_internal (const wchar_t $_1 * nptr, wchar_t $_1 ** endptr, int base, int group) {}

wchar_t $_1_2 *wcpcpy (wchar_t $_1_2 *dest, const wchar_t $_1 *src) {}

wchar_t $_1_2 *wcpncpy (wchar_t $_1_2 *dest, const wchar_t $_1 *src, size_t n) {}

int fwide (__FILE *fp, int mode) {}

int fwprintf (__FILE * stream, const wchar_t $untainted * format, ...) {}

int wprintf (const wchar_t $untainted * format, ...) {}

int swprintf (wchar_t * s, size_t n, const wchar_t $untainted * format, ...)
{
    va_list arg;
    va_start(arg, format);
    __taint_wstring_from_vararg(s, arg);
    va_end(arg);
}

int vfwprintf (__FILE * s, const wchar_t $untainted * format, __gnuc_va_list arg) {}

int vwprintf (const wchar_t $untainted * format, __gnuc_va_list arg) {}

int vswprintf (wchar_t * s, size_t n, const wchar_t $untainted * format, __gnuc_va_list arg)
{
    __taint_wstring_from_vararg(s, arg);
}

int fwscanf (__FILE * stream, const wchar_t $untainted * format, ...)
{
    va_list arg;
    va_start(arg, format);
    __taint_vararg_scanf(arg);
    va_end(arg);
}

int wscanf (const wchar_t $untainted * format, ...)
{
    va_list arg;
    va_start(arg, format);
    __taint_vararg_scanf(arg);
    va_end(arg);
}

int swscanf (const wchar_t * s, const wchar_t $untainted * format, ...)
{
    va_list arg;
    va_start(arg, format);
    __taint_vararg_from_wstring(s, arg);
    va_end(arg);
}

int vfwscanf (__FILE * s, const wchar_t $untainted * format, __gnuc_va_list arg)
{
    __taint_vararg_scanf(arg);
}

int vwscanf (const wchar_t $untainted * format, __gnuc_va_list arg)
{
    __taint_vararg_scanf(arg);
}

int vswscanf (const wchar_t * s, const wchar_t $untainted * format, __gnuc_va_list arg)
{
    __taint_vararg_from_wstring(s, arg);
}

wint_t $tainted fgetwc (__FILE *stream) {}
wint_t $tainted getwc (__FILE *stream) {}

wint_t $tainted getwchar (void) {}

wint_t $_1_2 fputwc (wchar_t $_1 wc, __FILE *stream) {}
wint_t $_1_2 putwc (wchar_t $_1 wc, __FILE *stream) {}

wint_t $_1_2 putwchar (wchar_t $_1 wc) {}

wchar_t $tainted *fgetws (wchar_t $tainted * ws, int n, __FILE * stream) {}

int fputws (const wchar_t * ws, __FILE * stream) {}

wint_t $_1_2 ungetwc (wint_t $_1 wc, __FILE *stream) {}

wint_t $tainted getwc_unlocked (__FILE *stream) {}
wint_t $tainted getwchar_unlocked (void) {}

wint_t $tainted fgetwc_unlocked (__FILE *stream) {}

wint_t $_1_2 fputwc_unlocked (wchar_t $_1 wc, __FILE *stream) {}

wint_t $_1_2 putwc_unlocked (wchar_t $_1 wc, __FILE *stream) {}
wint_t $_1_2 putwchar_unlocked (wchar_t $_1 wc) {}

wchar_t $tainted *fgetws_unlocked (wchar_t $tainted * ws, int n, __FILE * stream) {}

int fputws_unlocked (const wchar_t * ws, __FILE * stream) {}

// conservatively taint for now
size_t wcsftime (wchar_t $tainted * s, size_t maxsize, const wchar_t * format, const struct tm * tp) {}

size_t wcsftime_l (wchar_t $tainted * s, size_t maxsize, const wchar_t * format, const struct tm * tp, __locale_t loc) {}
