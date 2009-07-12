#include <glibc-fs/prelude.h>
#include <sys/syslog.h>

void closelog (void) {}
void openlog (const char /*$untainted*/ *ident, int option, int facility) {}
int setlogmask (int mask) {}
void syslog (int pri, const char $untainted *fmt, ...) {}
void vsyslog (int pri, const char $untainted *fmt, __gnuc_va_list ap) {}
