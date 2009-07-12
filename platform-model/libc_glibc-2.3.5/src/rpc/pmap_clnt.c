// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/pmap_clnt.h>

// typedef bool_t (*resultproc_t) (caddr_t resp, struct sockaddr_in *raddr);

bool_t pmap_set (const u_long program, const u_long vers, int protocol, u_short port) {}
bool_t pmap_unset (const u_long program, const u_long vers) {}

struct pmaplist *pmap_getmaps (struct sockaddr_in *address) {}

enum clnt_stat pmap_rmtcall (struct sockaddr_in *addr, const u_long prog, const u_long vers, const u_long proc,
                             xdrproc_t xdrargs, caddr_t argsp, xdrproc_t xdrres, caddr_t resp, struct timeval tout,
                             u_long *port_ptr) {}

enum clnt_stat clnt_broadcast (const u_long prog, const u_long vers, const u_long proc, xdrproc_t xargs,
                               caddr_t argsp, xdrproc_t xresults, caddr_t resultsp, resultproc_t eachresult) {}

u_short pmap_getport (struct sockaddr_in *address,
                      const u_long program,
                      const u_long version, u_int protocol) {}
