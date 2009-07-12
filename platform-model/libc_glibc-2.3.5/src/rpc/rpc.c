// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/rpc.h>

fd_set *__rpc_thread_svc_fdset (void) {}

struct rpc_createerr *__rpc_thread_createerr (void) {}

struct pollfd **__rpc_thread_svc_pollfd (void) {}

int *__rpc_thread_svc_max_pollfd (void) {}
