#include <glibc-fs/prelude.h>
#include <time.h>

clock_t clock (void) {}

time_t time (time_t *timer) {}

double difftime (time_t time1, time_t time0) {}

time_t mktime (struct tm *tp) {}

// quarl 2006-06-15
//    Tried conservatively tainting output of 'strftime', but that resulted in
//    too many false positives (unexploitable format string bugs).  So now we
//    assume the time strings don't add any taint (i.e. assume that there are
//    no locales with '%' in any strings).
FAKEBUILTIN
size_t strftime (char /*$tainted*/ $_1_2 * s, size_t maxsize, const char $_1 * format, const struct tm * tp) {}

size_t strftime_l (char /*$tainted*/ $_1_2 * s, size_t maxsize, const char $_1 * format,
                   const struct tm * tp, __locale_t loc) {}

// size_t strftime (char $_1_2 * s, size_t maxsize, const char $_1 * format, const struct tm * tp) {}

// size_t strftime_l (char $_1_2 * s, size_t maxsize, const char $_1 * format,
//                    const struct tm * tp, __locale_t loc) {}

char $_1_2 *strptime (const char $_1 * s, const char * fmt, struct tm *tp) {}

char $_1_2 *strptime_l (const char $_1 * s, const char * fmt, struct tm *tp, __locale_t loc) {}

struct tm *gmtime (const time_t *timer) {}

struct tm *localtime (const time_t *timer) {}

struct tm *gmtime_r (const time_t * timer, struct tm * tp) {}

struct tm *localtime_r (const time_t * timer, struct tm * tp) {}

// conservatively taint for now
char $tainted *asctime (const struct tm *tp) {}

char $tainted *ctime (const time_t *timer) {}

char $tainted *asctime_r (const struct tm * tp, char $tainted * buf) {}

char $tainted *ctime_r (const time_t * timer, char $tainted * buf) {}

// char *tzname[2];
// int daylight;
// long int timezone;

void tzset (void) {}

int stime (const time_t *when) {}

time_t timegm (struct tm *tp) {}

time_t timelocal (struct tm *tp) {}

int dysize (int year) {}

int nanosleep (const struct timespec *requested_time, struct timespec *remaining) {}

int clock_getres (clockid_t clock_id, struct timespec *res) {}

int clock_gettime (clockid_t clock_id, struct timespec *tp) {}

int clock_settime (clockid_t clock_id, const struct timespec *tp) {}

int clock_nanosleep (clockid_t clock_id, int flags,
                     const struct timespec *req,
                     struct timespec *rem) {}

int clock_getcpuclockid (pid_t pid, clockid_t *clock_id) {}

int timer_create (clockid_t clock_id, struct sigevent * evp, timer_t * timerid) {}

int timer_delete (timer_t timerid) {}

int timer_settime (timer_t timerid, int flags,
                   const struct itimerspec * value,
                   struct itimerspec * ovalue) {}

int timer_gettime (timer_t timerid, struct itimerspec *value) {}

int timer_getoverrun (timer_t timerid) {}

// int getdate_err;

struct tm *getdate (const char *string) {}

int getdate_r (const char * string, struct tm * resbufp) {}
