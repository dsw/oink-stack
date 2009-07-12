#include <glibc-fs/prelude.h>
#include <sys/time.h>

int gettimeofday (struct timeval * tv, __timezone_ptr_t tz) {}

int settimeofday (const struct timeval *tv, const struct timezone *tz) {}

int adjtime (const struct timeval *delta, struct timeval *olddelta) {}

int getitimer (__itimer_which_t which, struct itimerval *value) {}

int setitimer (__itimer_which_t which, const struct itimerval * new,
               struct itimerval * old) {}

int utimes (const char *file, const struct timeval tvp[2]) {}

int lutimes (const char *file, const struct timeval tvp[2]) {}

int futimes (int fd, const struct timeval tvp[2]) {}
