#include <glibc-fs/prelude.h>
#include <utmp.h>

// struct utmp
// {
//     short int ut_type;
//     pid_t ut_pid;
//     char ut_line[UT_LINESIZE];
//     char ut_id[4];
//     char ut_user[UT_NAMESIZE];
//     char ut_host[UT_HOSTSIZE];
//     struct exit_status ut_exit;

//     long int ut_session;
//     struct timeval ut_tv;

//     int32_t ut_addr_v6[4];
//     char unused[20];
// };

static inline void __taint_utmp(struct utmp* p)
{/*T:H*/
    __DO_TAINT(p->ut_line);
    __DO_TAINT(p->ut_id);
    __DO_TAINT(p->ut_user);
    __DO_TAINT(p->ut_host);
}

static inline struct utmp* __get_tainted_utmp(void)
{/*T:H*/
    struct utmp* p;
    __taint_utmp(p);
    return p;
}

int login_tty (int fd) {}

void login (const struct utmp *entry) {}

int logout (const char *ut_line) {}

void logwtmp (const char *ut_line, const char *ut_name,
              const char *ut_host) {}

void updwtmp (const char *wtmp_file, const struct utmp *utmp) {}

int utmpname (const char *file) {}

struct utmp *getutent (void) { return __get_tainted_utmp(); }

void setutent (void) {}

void endutent (void) {}

struct utmp *getutid (const struct utmp *id)  { return __get_tainted_utmp(); }

struct utmp *getutline (const struct utmp *line) { return __get_tainted_utmp(); }

struct utmp *pututline (const struct utmp *utmp_ptr) { return __get_tainted_utmp(); }

int getutent_r (struct utmp *buffer, struct utmp **result)
{
    __taint_utmp(buffer);
    *result = buffer;
}

int getutid_r (const struct utmp *id, struct utmp *buffer, struct utmp **result)
{
    __taint_utmp(buffer);
    *result = buffer;
}

int getutline_r (const struct utmp *line, struct utmp *buffer, struct utmp **result)
{
    __taint_utmp(buffer);
    *result = buffer;
}

