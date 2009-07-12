// $Id$

// UNSOUND - callrpc has the 'fread' bug

#include <glibc-fs/prelude.h>
#include <rpc/clnt.h>

// struct rpc_err {
//     enum clnt_stat re_status;
//     union {
//         int RE_errno;
//         enum auth_stat RE_why;
//         struct {
//             u_long low;
//             u_long high;
//         } RE_vers;
//         struct {
//             long s1;
//             long s2;
//         } RE_lb;
//     } ru;
// #define	re_errno	ru.RE_errno
// #define	re_why		ru.RE_why
// #define	re_vers		ru.RE_vers
// #define	re_lb		ru.RE_lb
// };

// typedef struct CLIENT CLIENT;
// struct CLIENT {
//     AUTH	*cl_auth;
//     struct clnt_ops {
//         enum clnt_stat (*cl_call) (CLIENT *, u_long, xdrproc_t, caddr_t, xdrproc_t,
//                                    caddr_t, struct timeval);

//         void (*cl_abort) (void);
//         void (*cl_geterr) (CLIENT *, struct rpc_err *);

//         bool_t (*cl_freeres) (CLIENT *, xdrproc_t, caddr_t);

//         void (*cl_destroy) (CLIENT *);
//         bool_t (*cl_control) (CLIENT *, int, char *);

//     } *cl_ops;
//     caddr_t cl_private;
// };

CLIENT *clntraw_create (const u_long prog, const u_long vers) {}

CLIENT *clnt_create (const char *host, const u_long prog, const u_long vers, const char *prot) {}

CLIENT *clnttcp_create (struct sockaddr_in *raddr, u_long prog, u_long version, int *sockp, u_int sendsz, u_int recvsz) {}

CLIENT *clntudp_create (struct sockaddr_in *raddr, u_long program, u_long version, struct timeval wait_resend, int *sockp) {}
CLIENT *clntudp_bufcreate (struct sockaddr_in *raddr, u_long program, u_long version, struct timeval wait_resend, int *sockp, u_int sendsz, u_int recvsz) {}

CLIENT *clntunix_create  (struct sockaddr_un *raddr, u_long program, u_long version, int *sockp, u_int sendsz, u_int recvsz) {}

// UNSOUNDNESS BUG - the "fread" bug - 'out' needs to be tainted regardless of
// incoming type (e.g. a struct)
int callrpc (const char *host, const u_long prognum,
             const u_long versnum, const u_long procnum,
             const xdrproc_t inproc, const char *in,
             const xdrproc_t outproc, char $tainted *out) {}

int _rpc_dtablesize (void) {}

// writes to stderr
void clnt_pcreateerror (const char *msg) {}

char $tainted *clnt_spcreateerror(const char *msg) {}

void clnt_perrno (enum clnt_stat num) {}

void clnt_perror (CLIENT *clnt, const char *msg) {}

char $tainted *clnt_sperror (CLIENT *clnt, const char *msg) {}

// struct rpc_createerr {
//     enum clnt_stat cf_stat;
//     struct rpc_err cf_error;
// };

// struct rpc_createerr rpc_createerr;

// return error string is based only on error number, so tainting it leads to
// false positives.
char *clnt_sperrno (enum clnt_stat num) {}

int getrpcport (const char * host, u_long prognum, u_long versnum, u_int proto) {}

void get_myaddress (struct sockaddr_in *s) {}
