// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/auth_unix.h>

// struct authunix_parms
// {
//     u_long aup_time;
//     char *aup_machname;
//     __uid_t aup_uid;
//     __gid_t aup_gid;
//     u_int aup_len;
//     __gid_t *aup_gids;
// };

static inline void __taint_authunix_parms(struct authunix_parms * p)
{/*T:H*/
    __DO_TAINT(p->aup_machname);
}

bool_t xdr_authunix_parms (XDR *xdrs, struct authunix_parms *p)
{
    __taint_authunix_parms(p);
}
