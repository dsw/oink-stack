#ifndef GLIBCFS_IGNORE_INCOMPLETE

#include <glibc-fs/prelude.h>
#include <rpc/rpc_msg.h>

// struct accepted_reply {
//     struct opaque_auth	ar_verf;
//     enum accept_stat	ar_stat;
//     union {
//         struct {
//             u_long	low;
//             u_long	high;
//         } AR_versions;
//         struct {
//             caddr_t	where;
//             xdrproc_t proc;
//         } AR_results;

//     } ru;
// };

// struct rejected_reply {
//     enum reject_stat rj_stat;
//     union {
//         struct {
//             u_long low;
//             u_long high;
//         } RJ_versions;
//         enum auth_stat RJ_why;
//     } ru;
// };

// struct reply_body {
//     enum reply_stat rp_stat;
//     union {
//         struct accepted_reply RP_ar;
//         struct rejected_reply RP_dr;
//     } ru;
// };

// struct call_body {
//     u_long cb_rpcvers;
//     u_long cb_prog;
//     u_long cb_vers;
//     u_long cb_proc;
//     struct opaque_auth cb_cred;
//     struct opaque_auth cb_verf;
// };

// struct rpc_msg {
//     u_long			rm_xid;
//     enum msg_type		rm_direction;
//     union {
//         struct call_body RM_cmb;
//         struct reply_body RM_rmb;
//     } ru;
// };

bool_t	xdr_callmsg (XDR *xdrs, struct rpc_msg *cmsg);

bool_t	xdr_callhdr (XDR *xdrs, struct rpc_msg *cmsg);

bool_t	xdr_replymsg (XDR *xdrs, struct rpc_msg *rmsg);

void	_seterr_reply (struct rpc_msg *msg, struct rpc_err *error);

#endif
