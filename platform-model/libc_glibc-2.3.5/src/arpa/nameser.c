#include <glibc-fs/prelude.h>
#include <arpa/nameser.h>

// This file nor its API are documented publicly so for now just do the
// conservative thing and taint every char* that's not const.

// typedef struct __ns_msg {
//     const u_char     *_msg, *_eom;
//     u_int16_t        _id, _flags, _counts[ns_s_max];
//     const u_char     *_sections[ns_s_max];
//     ns_sect          _sect;
//     int              _rrnum;
//     const u_char     *_ptr;
// } ns_msg;

// typedef      struct __ns_rr {
//     char             name[NS_MAXDNAME];
//     u_int16_t        type;
//     u_int16_t        rr_class;
//     u_int32_t        ttl;
//     u_int16_t        rdlength;
//     const u_char *   rdata;
// } ns_rr;

static inline void __taint_ns_rr(struct __ns_rr* p)
{/*T:H*/
    __DO_TAINT(p->name);
}

// struct ns_tsig_key {
//     char name[NS_MAXDNAME], alg[NS_MAXDNAME];
//     unsigned char *data;
//     int len;
// };

static inline void __taint_ns_tsig_key(struct ns_tsig_key *p)
{/*T:H*/
    __DO_TAINT(p->name);
    __DO_TAINT(p->alg);
}

// struct ns_tcp_tsig_state {
//     int counter;
//     struct dst_key *key;
//     void *ctx;
//     unsigned char sig[NS_PACKETSZ];
//     int siglen;
// };

static inline void __taint_ns_tcp_tsig_state(struct ns_tcp_tsig_state *p)
{/*T:H*/
    __DO_TAINT(p->sig);
    __DO_TAINT(p->ctx);
}

u_int ns_get16 (const u_char *s) {}

u_long ns_get32 (const u_char *s) {}

void ns_put16 (u_int i, u_char $tainted *s) {}

void ns_put32 (u_long i, u_char $tainted *s) {}

int ns_initparse (const u_char *s, int i1, ns_msg *m) {}

int ns_skiprr (const u_char *s, const u_char *s2, ns_sect ns1, int i1) {}

int ns_parserr (ns_msg *m, ns_sect s, int i1, ns_rr *rr) {
    __taint_ns_rr(rr);
}

// not sure what the semantics of these are so just taint for now
int ns_sprintrr (const ns_msg *m, const ns_rr *r, const char *s1,
                 const char *s2, char $tainted *s3, size_t n) {}

int ns_sprintrrf (const u_char *s1, size_t n1, const char * s2,
                  ns_class cl, ns_type t, u_long i, const u_char *s3,
                  size_t n2, const char *s4, const char *s5, char $tainted *s6, size_t n3) {}

int ns_format_ttl (u_long i, char $tainted *s, size_t n) {}

int ns_parse_ttl (const char *s, u_long *p) {}

u_int32_t ns_datetosecs (const char *cp, int *errp) {}

int ns_name_ntol (const u_char *s, u_char $tainted *t, size_t n) {}

int ns_name_ntop (const u_char *s, char $tainted *t, size_t n) {}

int ns_name_pton (const char *s, u_char $tainted *t, size_t n) {}

int ns_name_unpack (const u_char *s1, const u_char *s2,
                    const u_char *s3, u_char $tainted *t, size_t n) {}

int ns_name_pack (const u_char *s1, u_char $tainted *t, int n1,
                  const u_char **ps2, const u_char **ps3) {}

int ns_name_uncompress (const u_char *s1, const u_char *s2,
                        const u_char *s3, char $tainted *t, size_t n) {}

int ns_name_compress (const char *s1, u_char $tainted *t, size_t n,
                      const u_char ** ps2, const u_char ** ps3) {}

int ns_name_skip (const u_char **ps1, const u_char *s2) {}

void ns_name_rollback (const u_char *s1, const u_char **ps2, const u_char **ps3) {}

int ns_sign (u_char $tainted *t, int *p1, int i1, int i2, void *x1,
             const u_char *s1, int i3, u_char $tainted *t1, int *p2, time_t tm) {}

int ns_sign_tcp (u_char $tainted *t1, int *p1, int i1, int i2,
                 ns_tcp_tsig_state *t, int i3)
{
    __taint_ns_tcp_tsig_state(t);
}

int ns_sign_tcp_init (void $tainted *p, const u_char *s2,
                      int i1, ns_tcp_tsig_state *t)
{ __taint_ns_tcp_tsig_state(t); }


u_char $tainted *ns_find_tsig (u_char $tainted *t1, u_char $tainted * t2) {}

int ns_verify (u_char $tainted * s1, int * p2, void $tainted * p3,
               const u_char * p4, int p5, u_char $tainted * p6, int * p7, time_t * p8, int p9) {}

int ns_verify_tcp (u_char $tainted * t1, int * p2, ns_tcp_tsig_state * t, int i1)
{ __taint_ns_tcp_tsig_state(t); }

int ns_verify_tcp_init (void $tainted * p1, const u_char * s1, int i1,
                        ns_tcp_tsig_state * t)
{ __taint_ns_tcp_tsig_state(t); }

int ns_samedomain (const char * s1, const char * s2) {}

int ns_subdomain (const char * s1, const char * s2) {}

int ns_makecanon (const char * s1, char $tainted * t1, size_t n1) {}

int ns_samename (const char * s1, const char * s2) {}

