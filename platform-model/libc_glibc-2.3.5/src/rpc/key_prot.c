#ifndef GLIBCFS_IGNORE_INCOMPLETE

#include <glibc-fs/prelude.h>
#include <rpc/key_prot.h>

// typedef enum keystatus keystatus;
bool_t xdr_keystatus(XDR *, keystatus*);

// typedef char keybuf[HEXKEYBYTES];
bool_t xdr_keybuf(XDR *, keybuf);

// typedef char *netnamestr;
bool_t xdr_netnamestr(XDR *, netnamestr*);

// struct cryptkeyarg {
//     netnamestr remotename;
//     des_block deskey;
// };
// typedef struct cryptkeyarg cryptkeyarg;
bool_t xdr_cryptkeyarg(XDR *, cryptkeyarg*);

// struct cryptkeyarg2 {
//     netnamestr remotename;
//     netobj remotekey;
//     des_block deskey;
// };
// typedef struct cryptkeyarg2 cryptkeyarg2;
bool_t xdr_cryptkeyarg2(XDR *, cryptkeyarg2*);

// struct cryptkeyres {
//     keystatus status;
//     union {
//         des_block deskey;
//     } cryptkeyres_u;
// };
// typedef struct cryptkeyres cryptkeyres;
bool_t xdr_cryptkeyres(XDR *, cryptkeyres*);

// struct unixcred {
//     u_int uid;
//     u_int gid;
//     struct {
//         u_int gids_len;
//         u_int *gids_val;
//     } gids;
// };
// typedef struct unixcred unixcred;
bool_t xdr_unixcred(XDR *, unixcred*);

// struct getcredres {
//     keystatus status;
//     union {
//         unixcred cred;
//     } getcredres_u;
// };
// typedef struct getcredres getcredres;
bool_t xdr_getcredres(XDR *, getcredres*);

// struct key_netstarg {
//     keybuf st_priv_key;
//     keybuf st_pub_key;
//     netnamestr st_netname;
// };
// typedef struct key_netstarg key_netstarg;
bool_t xdr_key_netstarg(XDR *, key_netstarg*);

// struct key_netstres {
//     keystatus status;
//     union {
//         key_netstarg knet;
//     } key_netstres_u;
// };
// typedef struct key_netstres key_netstres;
bool_t xdr_key_netstres(XDR *, key_netstres*);

keystatus * key_set_1(opaque *, CLIENT *);
keystatus * key_set_1_svc(opaque *, struct svc_req *);

cryptkeyres * key_encrypt_1(cryptkeyarg *, CLIENT *);
cryptkeyres * key_encrypt_1_svc(cryptkeyarg *, struct svc_req *);

cryptkeyres * key_decrypt_1(cryptkeyarg *, CLIENT *);
cryptkeyres * key_decrypt_1_svc(cryptkeyarg *, struct svc_req *);

des_block * key_gen_1(void *, CLIENT *);
des_block * key_gen_1_svc(void *, struct svc_req *);

getcredres * key_getcred_1(netnamestr *, CLIENT *);
getcredres * key_getcred_1_svc(netnamestr *, struct svc_req *);

keystatus * key_set_2(opaque *, CLIENT *);
keystatus * key_set_2_svc(opaque *, struct svc_req *);
cryptkeyres * key_encrypt_2(cryptkeyarg *, CLIENT *);
cryptkeyres * key_encrypt_2_svc(cryptkeyarg *, struct svc_req *);
cryptkeyres * key_decrypt_2(cryptkeyarg *, CLIENT *);
cryptkeyres * key_decrypt_2_svc(cryptkeyarg *, struct svc_req *);
des_block * key_gen_2(void *, CLIENT *);
des_block * key_gen_2_svc(void *, struct svc_req *);
getcredres * key_getcred_2(netnamestr *, CLIENT *);
getcredres * key_getcred_2_svc(netnamestr *, struct svc_req *);

cryptkeyres * key_encrypt_pk_2(cryptkeyarg2 *, CLIENT *);
cryptkeyres * key_encrypt_pk_2_svc(cryptkeyarg2 *, struct svc_req *);

cryptkeyres * key_decrypt_pk_2(cryptkeyarg2 *, CLIENT *);
cryptkeyres * key_decrypt_pk_2_svc(cryptkeyarg2 *, struct svc_req *);

keystatus * key_net_put_2(key_netstarg *, CLIENT *);
keystatus * key_net_put_2_svc(key_netstarg *, struct svc_req *);

key_netstres * key_net_get_2(void *, CLIENT *);
key_netstres * key_net_get_2_svc(void *, struct svc_req *);

cryptkeyres * key_get_conv_2(opaque *, CLIENT *);
cryptkeyres * key_get_conv_2_svc(opaque *, struct svc_req *);

#endif
