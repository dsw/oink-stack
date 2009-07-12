#include <glibc-fs/prelude.h>
#include <resolv.h>

// typedef res_sendhookact (*res_send_qhook) (struct sockaddr_in * const *ns,
// 					   const u_char **query,
// 					   int *querylen,
// 					   u_char *ans,
// 					   int anssiz,
// 					   int *resplen);

// typedef res_sendhookact (*res_send_rhook) (const struct sockaddr_in *ns,
// 					   const u_char *query,
// 					   int querylen,
// 					   u_char *ans,
// 					   int anssiz,
// 					   int *resplen);

// struct __res_state {
//     int	retrans;
//     int	retry;
//     u_long	options;
//     int	nscount;
//     struct sockaddr_in
//     nsaddr_list[MAXNS];
//     u_short	id;
//     char	*dnsrch[MAXDNSRCH+1];
//     char	defdname[256];
//     u_long	pfcode;
//     unsigned ndots:4;
//     unsigned nsort:4;
//     char	unused[3];
//     struct {
//         struct in_addr	addr;
//         u_int32_t	mask;
//     } sort_list[MAXRESOLVSORT];
//     res_send_qhook qhook;
//     res_send_rhook rhook;
//     int	res_h_errno;
//     int	_vcsock;
//     u_int	_flags;
//     union {
//         char	pad[52];
//         struct {
//             u_int16_t		nscount;
//             u_int16_t		nsmap[MAXNS];
//             int			nssocks[MAXNS];
//             u_int16_t		nscount6;
//             u_int16_t		nsinit;
//             struct sockaddr_in6	*nsaddrs[MAXNS];
//         } _ext;
//     } _u;
// };

static inline void __taint_res_state(struct __res_state * p)
{/*T:H*/
    __DO_TAINT(p->dnsrch[0]);
    __DO_TAINT(p->defdname);
}

static inline struct __res_state * __get_tainted_res_state(void)
{/*T:H*/
    struct __res_state * p;
    __taint_res_state(p);
    return p;
}

// struct res_sym {
//     int	number;
//     char *	name;
//     char *	humanname;
// };

static inline void __taint_res_sym(struct res_sym * p)
{/*T:H*/
    __DO_TAINT(p->name);
    __DO_TAINT(p->humanname);
}

struct __res_state *__res_state(void) { return __get_tainted_res_state(); }

void fp_nquery (const u_char *s1, int i1, FILE * fp) {}
void fp_query (const u_char *s1, FILE *fp) {}
const char $tainted * hostalias (const char *s1) {}
void p_query (const u_char *s1) {}
void res_close (void) {}
int res_init (void) {}
int res_isourserver (const struct sockaddr_in *a1) {}
int res_mkquery (int i0, const char * s1, int i1, int i2, const u_char * s3, int i3, const u_char * s4, u_char $tainted * s5, int i6) {}
int res_query (const char * s1, int i2, int i3, u_char $tainted * s4, int i5) {}
int res_querydomain (const char * s1, const char * s2, int i3, int i4, u_char $tainted * s5, int i6) {}
int res_search (const char * s1, int i2, int i3, u_char $tainted * s4, int i5) {}
int res_send (const u_char * s1, int i2, u_char $tainted * s3, int i4) {}

int res_hnok (const char * s1) {}
int res_ownok (const char * s1) {}
int res_mailok (const char * s1) {}
int res_dnok (const char * s1) {}
int sym_ston (const struct res_sym * r1, const char * s2, int * p3) {}
const char $tainted * sym_ntos (const struct res_sym * r1, int i2, int * p3) {}
const char $tainted * sym_ntop (const struct res_sym * r1, int i2, int * p3) {}
int b64_ntop (u_char const * s1, size_t n2, char $tainted * s3, size_t n4) {}
int b64_pton (char const * s1, u_char $tainted * s2, size_t n3) {}
int loc_aton (const char *ascii, u_char $tainted *binary) {}
const char * loc_ntoa (const u_char *binary, char $tainted *ascii) {}
int dn_skipname (const u_char * s1, const u_char * s2) {}
void putlong (u_int32_t i1, u_char $tainted * s2) {}
void putshort (u_int16_t i1, u_char $tainted * s2) {}
const char $tainted * p_class (int i1) {}
const char $tainted * p_time (u_int32_t i1) {}
const char $tainted * p_type (int i1) {}
const char $tainted * p_rcode (int i1) {}
const u_char $tainted * p_cdnname (const u_char * s1, const u_char * s2, int i3, FILE * fp) {}
const u_char $tainted * p_cdname (const u_char * s1, const u_char * s2, FILE * fp) {}
const u_char $tainted * p_fqnname (const u_char *cp, const u_char *msg, int i1, char $tainted * s1, int i2) {}
const u_char $tainted * p_fqname (const u_char * s1, const u_char * s2, FILE * fp) {}
const char $tainted * p_option (u_long option) {}
char $tainted * p_secstodate (u_long i2) {}
int dn_count_labels (const char * s1) {}
int dn_comp (const char * s1, u_char $tainted * s2, int i3, u_char $tainted ** ps4, u_char $tainted ** ps5) {}
int dn_expand (const u_char * s1, const u_char * s2, const u_char * s3, char $tainted * s4, int i5) {}
u_int res_randomid (void) {}
int res_nameinquery (const char * s1, int i2, int i3, const u_char * s4, const u_char * s5) {}
int res_queriesmatch (const u_char * s1, const u_char * s2, const u_char * s3, const u_char * s4) {}
const char $tainted * p_section (int section, int opcode) {}

int res_ninit (res_state r1) {}
int res_nisourserver (const res_state r2, const struct sockaddr_in * s3) {}
void fp_resstat (const res_state r1, FILE * fp) {}
void res_npquery (const res_state r1, const u_char * s2, int i3, FILE * fp) {}
const char $tainted * res_hostalias (const res_state r1, const char * s2, char $tainted * s3, size_t n4) {}
int res_nquery (res_state r1, const char * s2, int i3, int i4, u_char $tainted * s5, int i6) {}
int res_nsearch (res_state r1, const char * s2, int i3, int i4, u_char $tainted * s5, int i7) {}
int res_nquerydomain (res_state r1, const char * s2, const char * s3, int i4, int i5, u_char $tainted * s6, int i7) {}
int res_nmkquery (res_state r1, int i2, const char * s3, int i4, int i5, const u_char * s6, int i7, const u_char * s8, u_char $tainted * s9, int i10) {}
int res_nsend (res_state r1, const u_char * s2, int i3, u_char $tainted * s4, int i5) {}
void res_nclose (res_state r1) {}
