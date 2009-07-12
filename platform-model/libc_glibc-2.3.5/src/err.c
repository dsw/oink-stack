#include <glibc-fs/prelude.h>
#include <err.h>

void warn (const char $untainted *format, ...) {}

void vwarn (const char $untainted *format, va_list arg) {}

void warnx (const char $untainted *format, ...) {}
void vwarnx (const char $untainted *format, va_list arg) {}

void err (int status, const char $untainted *format, ...) {__void_noreturn();}
void verr (int status, const char $untainted *format, va_list arg) {__void_noreturn();}
void errx (int status, const char $untainted *format, ...) {__void_noreturn();}
void verrx (int status, const char $untainted *format, va_list arg) {__void_noreturn();}
