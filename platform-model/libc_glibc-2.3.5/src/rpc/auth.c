// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/auth.h>

// union des_block {
//     struct {
//         u_int32_t high;
//         u_int32_t low;
//     } key;
//     char c[8];
// };
// typedef union des_block des_block;
// bool_t xdr_des_block (XDR *xdrs, des_block *blkp);

// struct opaque_auth {
//     enum_t	oa_flavor;
//     caddr_t	oa_base;
//     u_int	oa_length;
// };

// typedef struct AUTH AUTH;
// struct AUTH {
//     struct opaque_auth ah_cred;
//     struct opaque_auth ah_verf;
//     union des_block ah_key;
//     struct auth_ops {
//         void (*ah_nextverf) (AUTH *);
//         int  (*ah_marshal) (AUTH *, XDR *);
//         int  (*ah_validate) (AUTH *, struct opaque_auth *);

//         int  (*ah_refresh) (AUTH *);
//         void (*ah_destroy) (AUTH *);
//     } *ah_ops;
//     caddr_t ah_private;
// };

//// xdr.h
// struct netobj
// {
//     u_int n_len;
//     char *n_bytes;
// };

static inline void __taint_netobj(struct netobj* p)
{/*T:H*/
    __DO_TAINT(p->n_bytes);
}

// struct opaque_auth _null_auth;

AUTH *authunix_create (char /*const*/ *machname, __uid_t uid, __gid_t gid, int len, __gid_t *aup_gids) {}
AUTH *authunix_create_default (void) {}
AUTH *authnone_create (void) {}
AUTH *authdes_create (const char *servername, u_int window, struct sockaddr *syncaddr, des_block *ckey) {}
AUTH *authdes_pk_create (const char *s1, netobj *n1, u_int i1, struct sockaddr *a1, des_block *c1) {
    __taint_netobj(n1);
}

int getnetname (char $tainted *s1) {}
int host2netname (char $tainted *s1, const char *s2, const char *s3) {}
int user2netname (char $tainted *s1, const uid_t u1, const char * s2) {}
int netname2user (const char *s1, uid_t * u1, gid_t * u2, int * i3, gid_t * u4) {}
int netname2host (const char *s1, char $tainted *s2, const int i3) {}

int key_decryptsession (char $tainted * s1, des_block * c1) {}
int key_decryptsession_pk (char $tainted * s1, netobj * n2, des_block * c1) {
    __taint_netobj(n2);
}
int key_encryptsession (char $tainted *s1, des_block * c1) {}
int key_encryptsession_pk (char $tainted *s1, netobj *n2, des_block *c1) {
    __taint_netobj(n2);
}
int key_gendes (des_block * c1) {}
int key_setsecret (char $tainted *s1) {}
int key_secretkey_is_set (void) {}
int key_get_conv (char $tainted *s1, des_block * c2) {}

bool_t xdr_opaque_auth (XDR *x1, struct opaque_auth *p2) {}
