#include <glibc-fs/prelude.h>
#include <grp.h>

// struct group
// {
//     char *gr_name;
//     char *gr_passwd;
//     __gid_t gr_gid;
//     char **gr_mem;
// };

void setgrent (void) {}

void endgrent (void) {}

static inline void __taint_group(struct group* p)
{/*T:H*/
    __DO_TAINT(p->gr_name);
    __DO_TAINT(p->gr_passwd);
    __DO_TAINT(p->gr_mem[0]);
}

static inline struct group* __get_tainted_group()
{/*T:H*/
    struct group* p;
    __taint_group(p);
    return p;
}

struct group *getgrent (void) { return __get_tainted_group(); }

struct group *fgetgrent (FILE *stream) { return __get_tainted_group(); }

int putgrent (const struct group * p, FILE * f) {}

struct group *getgrgid (__gid_t gid) { return __get_tainted_group(); }

struct group *getgrnam (const char *name) { return __get_tainted_group(); }

int getgrent_r (struct group * resultbuf, char $tainted * buffer, size_t buflen, struct group ** result)
{
    __taint_group(resultbuf);
    __taint_group(*result);
}

int getgrgid_r (__gid_t gid, struct group * resultbuf, char $tainted * buffer, size_t buflen, struct group ** result)
{
    __taint_group(resultbuf);
    __taint_group(*result);
}

int getgrnam_r (const char * name, struct group * resultbuf, char $tainted * buffer, size_t buflen, struct group ** result)
{
    __taint_group(resultbuf);
    __taint_group(*result);
}

int fgetgrent_r (FILE * stream, struct group * resultbuf, char $tainted * buffer, size_t buflen, struct group ** result)
{
    __taint_group(resultbuf);
    __taint_group(*result);
}

int setgroups (size_t n, const __gid_t *groups) {}

int getgrouplist (const char *user, __gid_t group, __gid_t *groups, int *ngroups) {}

int initgroups (const char *user, __gid_t group) {}
