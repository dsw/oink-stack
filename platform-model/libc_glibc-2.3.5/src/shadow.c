#include <glibc-fs/prelude.h>
#include <shadow.h>

// struct spwd
// {
//     char *sp_namp;
//     char *sp_pwdp;
//     long int sp_lstchg;
//     long int sp_min;
//     long int sp_max;
//     long int sp_warn;
//     long int sp_inact;
//     long int sp_expire;
//     unsigned long int sp_flag;
// };

static inline void __taint_spwd(struct spwd* p)
{/*T:H*/
    __DO_TAINT(p->sp_namp);
    __DO_TAINT(p->sp_pwdp);
}

static inline struct spwd* __get_tainted_spwd(void)
{/*T:H*/
    struct spwd* p;
    __taint_spwd(p);
    return p;
}

void setspent (void) {}

void endspent (void) {}

struct spwd *getspent (void) { return __get_tainted_spwd(); }

struct spwd *getspnam (const char *name) { return __get_tainted_spwd(); }

struct spwd *sgetspent (const char *string) { return __get_tainted_spwd(); }

struct spwd *fgetspent (FILE *stream) { return __get_tainted_spwd(); }

int putspent (const struct spwd *p, FILE *stream) {}

int getspent_r (struct spwd *result_buf, char $tainted *buffer,
                size_t buflen, struct spwd **result)
{
    __taint_spwd(result_buf);
    result[0] = result_buf;
}

int getspnam_r (const char *name, struct spwd *result_buf,
                char $tainted *buffer, size_t buflen,
                struct spwd **result)
{
    __taint_spwd(result_buf);
    result[0] = result_buf;
}

int sgetspent_r (const char *string, struct spwd *result_buf,
                 char $tainted *buffer, size_t buflen,
                 struct spwd **result)
{
    __taint_spwd(result_buf);
    result[0] = result_buf;
}

int fgetspent_r (FILE *stream, struct spwd *result_buf,
                 char $tainted *buffer, size_t buflen,
                 struct spwd **result)
{
    __taint_spwd(result_buf);
    result[0] = result_buf;
}

int lckpwdf (void) {}

int ulckpwdf (void) {}
