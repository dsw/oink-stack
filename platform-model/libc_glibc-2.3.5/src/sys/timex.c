#include <glibc-fs/prelude.h>
#include <sys/timex.h>

int __adjtimex (struct timex *ntx) {}
int adjtimex (struct timex *ntx) {}

int ntp_gettime (struct ntptimeval *ntv) {}
int ntp_adjtime (struct timex *tntx) {}
