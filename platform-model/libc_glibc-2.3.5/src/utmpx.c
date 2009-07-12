#include <glibc-fs/prelude.h>
#include <utmpx.h>
#include <utmp.h>

//// from bits/utmpx.h
// struct utmpx
// {
//     short int ut_type;
//     __pid_t ut_pid;
//     char ut_line[__UT_LINESIZE];
//     char ut_id[4];
//     char ut_user[__UT_NAMESIZE];
//     char ut_host[__UT_HOSTSIZE];
//     struct __exit_status ut_exit;

//     long int ut_session;
//     struct timeval ut_tv;
//     __int32_t ut_addr_v6[4];
//     char unused[20];
// };

static inline void __taint_utmp(struct utmp* p)
{/*T:H*/
    __DO_TAINT(p->ut_line);
    __DO_TAINT(p->ut_id);
    __DO_TAINT(p->ut_user);
    __DO_TAINT(p->ut_host);
}

static inline void __taint_utmpx(struct utmpx* p)
{/*T:H*/
    __DO_TAINT(p->ut_line);
    __DO_TAINT(p->ut_id);
    __DO_TAINT(p->ut_user);
    __DO_TAINT(p->ut_host);
}

static inline struct utmpx* __get_tainted_utmpx(void)
{/*T:H*/
    struct utmpx* p;
    __taint_utmpx(p);
    return p;
}

void setutxent (void) {}

void endutxent (void) {}

struct utmpx *getutxent (void) { return __get_tainted_utmpx(); }

struct utmpx *getutxid (const struct utmpx *id) { return __get_tainted_utmpx(); }

struct utmpx *getutxline (const struct utmpx *line) { return __get_tainted_utmpx(); }

struct utmpx *pututxline (const struct utmpx *utmpx) { return __get_tainted_utmpx(); }

int utmpxname (const char *file) {}

void updwtmpx (const char *wtmpx_file, const struct utmpx *utmpx) {}

// we could do something more intelligent by copying utmpx to utmp, when we're
// less lazy
void getutmp (const struct utmpx *utmpx, struct utmp *utmp) { __taint_utmp(utmp); }

void getutmpx (const struct utmp *utmp, struct utmpx *utmpx) { __taint_utmpx(utmpx); }
