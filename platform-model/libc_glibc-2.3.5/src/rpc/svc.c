// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/svc.h>

// typedef struct SVCXPRT SVCXPRT;
// struct SVCXPRT {
//     int xp_sock;
//     u_short xp_port;
//     const struct xp_ops {
//         bool_t	(*xp_recv) (SVCXPRT *xprt, struct rpc_msg *msg);

//         enum xprt_stat (*xp_stat) (SVCXPRT *xprt);

//         bool_t	(*xp_getargs) (SVCXPRT *xprt, xdrproc_t xdr_args,
// 			       caddr_t args_ptr);
//         bool_t	(*xp_reply) (SVCXPRT *xprt, struct rpc_msg *msg);

//         bool_t	(*xp_freeargs) (SVCXPRT *xprt, xdrproc_t xdr_args,
// 				caddr_t args_ptr);

//         void	(*xp_destroy) (SVCXPRT *xprt);

//     } *xp_ops;
//     int		xp_addrlen;
//     struct sockaddr_in xp_raddr;
//     struct opaque_auth xp_verf;
//     caddr_t		xp_p1;
//     caddr_t		xp_p2;
//     char		xp_pad [256];
// };

// struct svc_req {
//     rpcprog_t rq_prog;
//     rpcvers_t rq_vers;
//     rpcproc_t rq_proc;
//     struct opaque_auth rq_cred;
//     caddr_t rq_clntcred;
//     SVCXPRT *rq_xprt;
// };

void __taint_SVCXPRT(SVCXPRT *xprt) {
    char $tainted *t1;
    char $tainted *t2;
    char $tainted *t3;
    char $tainted *t4;
    char $tainted *t5;
    xdrproc_t args;
    struct rpc_msg *msg;                            // ...?
    xprt->xp_p1 = t1;
    xprt->xp_p2 = t2;
    xprt->xp_pad[0] = *t3;
    xprt->xp_ops->xp_recv(xprt, msg);
    xprt->xp_ops->xp_stat(xprt);
    xprt->xp_ops->xp_getargs(xprt, args, t4);
    xprt->xp_ops->xp_reply(xprt, msg);
    xprt->xp_ops->xp_freeargs(xprt, args, t5);
    xprt->xp_ops->xp_destroy(xprt);
}

// typedef void (*__dispatch_fn_t) (struct svc_req*, SVCXPRT*);

bool_t svc_register (SVCXPRT *xprt, rpcprog_t prog, rpcvers_t vers, __dispatch_fn_t dispatch, rpcprot_t protocol)
{
    __taint_SVCXPRT(xprt);
    struct svc_req *req;                            // ...?
    dispatch(req, xprt);                      // should we pass any arguments?
}

void svc_unregister (rpcprog_t prog, rpcvers_t vers) {}

void xprt_register (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

void xprt_unregister (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

bool_t	svc_sendreply (SVCXPRT *xprt, xdrproc_t xdr_results, caddr_t xdr_location) {
    __taint_SVCXPRT(xprt);
}

void	svcerr_decode (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

void	svcerr_weakauth (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

void	svcerr_noproc (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

void	svcerr_progvers (SVCXPRT *xprt, rpcvers_t low_vers, rpcvers_t high_vers) { __taint_SVCXPRT(xprt); }

void	svcerr_auth (SVCXPRT *xprt, enum auth_stat why) { __taint_SVCXPRT(xprt); }

void	svcerr_noprog (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

void	svcerr_systemerr (SVCXPRT *xprt) { __taint_SVCXPRT(xprt); }

struct pollfd *svc_pollfd;
int svc_max_pollfd;
fd_set svc_fdset;

void svc_getreq (int rdfds) {}
void svc_getreq_common (const int fd) {}
void svc_getreqset (fd_set *readfds) {}
void svc_getreq_poll (struct pollfd *fd, const int i) {}
void svc_exit (void) {}
void svc_run (void) {}

SVCXPRT *svcraw_create (void);

SVCXPRT *svcudp_create (int sock);
SVCXPRT *svcudp_bufcreate (int sock, u_int sendsz, u_int recvsz);

SVCXPRT *svctcp_create (int sock, u_int sendsize, u_int recvsize);

SVCXPRT *svcunix_create (int sock, u_int sendsize, u_int recvsize, char *path);
