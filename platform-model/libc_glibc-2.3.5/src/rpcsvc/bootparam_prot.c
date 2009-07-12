#ifndef GLIBCFS_IGNORE_INCOMPLETE
// this file is actually complete, but we don't want to include <rpc/*.h>
// which requires all the other stuff to be complete

#include <glibc-fs/prelude.h>
#include <rpcsvc/bootparam_prot.h>

// typedef char *bp_machine_name_t;

// typedef char *bp_path_t;

// typedef char *bp_fileid_t;

// struct ip_addr_t {
//     char net;
//     char host;
//     char lh;
//     char impno;
// };

// struct bp_whoami_res {
//     bp_machine_name_t client_name;
//     bp_machine_name_t domain_name;
//     bp_address router_address;
// };

static inline void __taint_bp_whoami_res(struct bp_whoami_res* p)
{/*T:H*/
    __DO_TAINT(p->client_name);
    __DO_TAINT(p->domain_name);
}

static inline struct bp_whoami_res* __get_tainted_bp_whoami_res()
{/*T:H*/
    struct bp_whoami_res* p;
    __taint_bp_whoami_res(p);
    return p;
}

// struct bp_getfile_arg {
//     bp_machine_name_t client_name;
//     bp_fileid_t file_id;
// };

static inline void __taint_bp_getfile_arg(struct bp_getfile_arg* p)
{/*T:H*/
    __DO_TAINT(p->client_name);
}

static inline struct bp_getfile_arg* __get_tainted_bp_getfile_arg()
{/*T:H*/
    struct bp_getfile_arg* p;
    __taint_bp_getfile_arg(p);
    return p;
}

// struct bp_getfile_res {
//     bp_machine_name_t server_name;
//     bp_address server_address;
//     bp_path_t server_path;
// };

static inline void __taint_bp_getfile_res(struct bp_getfile_res* p)
{/*T:H*/
    __DO_TAINT(p->server_name);
    __DO_TAINT(p->server_path);
}

static inline struct bp_getfile_res* __get_tainted_bp_getfile_res()
{/*T:H*/
    struct bp_getfile_res* p;
    __taint_bp_getfile_res(p);
    return p;
}

bp_whoami_res * bootparamproc_whoami_1(bp_whoami_arg *a1, CLIENT *c1) { return __get_tainted_bp_whoami_res(); }
bp_whoami_res * bootparamproc_whoami_1_svc(bp_whoami_arg *a1, struct svc_req *r1) { return __get_tainted_bp_whoami_res(); }

bp_getfile_res * bootparamproc_getfile_1(bp_getfile_arg *a1, CLIENT *c1) { return __get_tainted_bp_getfile_res(); }
bp_getfile_res * bootparamproc_getfile_1_svc(bp_getfile_arg *a1, struct svc_req *r1) { return __get_tainted_bp_getfile_res(); }
int bootparamprog_1_freeresult (SVCXPRT *r1, xdrproc_t x1, caddr_t a1) {}

bool_t xdr_bp_machine_name_t (XDR *x1, char $tainted **s1) {}
bool_t xdr_bp_path_t (XDR *x1, char $tainted **s1);
bool_t xdr_bp_fileid_t (XDR *x1, char $tainted **s1);
bool_t xdr_ip_addr_t (XDR *x1, ip_addr_t *a1);
bool_t xdr_bp_address (XDR *x1, bp_address* a1) {}
bool_t xdr_bp_whoami_arg (XDR *x1, bp_whoami_arg* a1) {}
bool_t xdr_bp_whoami_res (XDR *x1, bp_whoami_res* p) { __taint_bp_whoami_res(p); }
bool_t xdr_bp_getfile_arg (XDR *x1, bp_getfile_arg* p) { __taint_bp_getfile_arg(p); }
bool_t xdr_bp_getfile_res (XDR *x1, bp_getfile_res* p) { __taint_bp_getfile_res(p); }

#endif
