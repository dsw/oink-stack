#ifndef GLIBCFS_IGNORE_INCOMPLETE

#include <glibc-fs/prelude.h>
#include <rpc/svc_auth.h>

enum auth_stat _authenticate (struct svc_req *rqst, struct rpc_msg *msg);

#endif
