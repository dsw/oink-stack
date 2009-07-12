#include <glibc-fs/prelude.h>
#include <pwd.h>

// struct passwd
// {
//     char *pw_name;
//     char *pw_passwd;
//     __uid_t pw_uid;
//     __gid_t pw_gid;
//     char *pw_gecos;
//     char *pw_dir;
//     char *pw_shell;
// };

static inline void __taint_passwd(struct passwd *p)
{/*T:H*/
    __DO_TAINT(p->pw_name);
    __DO_TAINT(p->pw_passwd);
    __DO_TAINT(p->pw_gecos);
    __DO_TAINT(p->pw_dir);
    __DO_TAINT(p->pw_shell);
}

static inline struct passwd* __get_tainted_passwd(void)
{/*T:H*/
    struct passwd* p;
    __taint_passwd(p);
    return p;
}

void setpwent (void) {}

void endpwent (void) {}

struct passwd *getpwent (void) { return __get_tainted_passwd(); }

struct passwd *fgetpwent (FILE *stream) { return __get_tainted_passwd(); }

int putpwent (const struct passwd * p, FILE * f) {}

struct passwd *getpwuid (__uid_t uid) { return __get_tainted_passwd(); }

struct passwd *getpwnam (const char *name) { return __get_tainted_passwd(); }

int getpwent_r (struct passwd * resultbuf, char $tainted * buffer, size_t buflen, struct passwd ** result)
{
    __taint_passwd(resultbuf);
    *result = resultbuf;
}

int getpwuid_r (__uid_t uid, struct passwd * resultbuf, char $tainted * buffer, size_t buflen, struct passwd ** result)
{
    __taint_passwd(resultbuf);
    *result = resultbuf;
}

int getpwnam_r (const char * name, struct passwd * resultbuf, char $tainted * buffer, size_t buflen, struct passwd ** result)
{
    __taint_passwd(resultbuf);
    *result = resultbuf;
}

int fgetpwent_r (FILE * stream, struct passwd * resultbuf, char $tainted * buffer, size_t buflen, struct passwd ** result)
{
    __taint_passwd(resultbuf);
    *result = resultbuf;
}

int getpw (__uid_t uid, char $tainted *buffer) {}
