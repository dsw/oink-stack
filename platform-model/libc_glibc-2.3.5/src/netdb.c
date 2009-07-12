#include <glibc-fs/prelude.h>
#include <netdb.h>

int *__h_errno_location () {}

void herror (const char *str) {}

// 2006-05-08 - this is no longer tainted.  Result is from an array of static
// strings indexed by err_num, so usually just false positives.
const char *hstrerror (int err_num) {}
// const char $tainted *hstrerror (int err_num) {}

// struct hostent
// {
//     char *h_name;
//     char **h_aliases;
//     int h_addrtype;
//     int h_length;
//     char **h_addr_list;
// };

static inline void __taint_hostent(struct hostent* p)
{/*T:H*/
    __DO_TAINT(p->h_name);
    __DO_TAINT(p->h_aliases[0]);
    __DO_TAINT(p->h_addr_list[0]);
}

static inline struct hostent* __get_tainted_hostent(void)
{/*T:H*/
    struct hostent* p;
    __taint_hostent(p);
    return p;
}

// // from bits/netent.h
// struct netent
// {
//     char *n_name;
//     char **n_aliases;
//     int n_addrtype;
//     uint32_t n_net;
// };

static inline void __taint_netent(struct netent* p)
{/*T:H*/
    __DO_TAINT(p->n_name);
    __DO_TAINT(p->n_aliases[0]);
}

static inline struct netent* __get_tainted_netent(void)
{/*T:H*/
    struct netent* p;
    __taint_netent(p);
    return p;
}

void sethostent (int stay_open) {}

void endhostent (void) {}

struct hostent *gethostent (void) { return __get_tainted_hostent(); }

struct hostent *gethostbyaddr (const void *addr, __socklen_t len, int type)
{ return __get_tainted_hostent(); }

struct hostent *gethostbyname (const char *name)
{ return __get_tainted_hostent(); }

struct hostent *gethostbyname2 (const char *name, int af)
{ return __get_tainted_hostent(); }

int gethostent_r (struct hostent * result_buf,
                  char * buf, size_t buflen,
                  struct hostent ** result,
                  int * h_errnop)
{
    __taint_hostent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int gethostbyaddr_r (const void * addr, __socklen_t len, int type,
                     struct hostent * result_buf,
                     char * buf, size_t buflen,
                     struct hostent ** result,
                     int * h_errnop)
{
    __taint_hostent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int gethostbyname_r (const char * name,
                     struct hostent * result_buf,
                     char * buf, size_t buflen,
                     struct hostent ** result,
                     int * h_errnop)
{
    __taint_hostent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int gethostbyname2_r (const char * name, int af,
                      struct hostent * result_buf,
                      char * buf, size_t buflen,
                      struct hostent ** result,
                      int * h_errnop)
{
    __taint_hostent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

void setnetent (int stay_open) {}

void endnetent (void) {}

struct netent *getnetent (void) { return __get_tainted_netent(); }

struct netent *getnetbyaddr (uint32_t net, int type) { return __get_tainted_netent(); }

struct netent *getnetbyname (const char *name) { return __get_tainted_netent(); }

int getnetent_r (struct netent * result_buf,
                 char * buf, size_t buflen,
                 struct netent ** result,
                 int * h_errnop)
{
    __taint_netent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int getnetbyaddr_r (uint32_t net, int type,
                    struct netent * result_buf,
                    char * buf, size_t buflen,
                    struct netent ** result,
                    int * h_errnop)
{
    __taint_netent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int getnetbyname_r (const char * name,
                    struct netent * result_buf,
                    char * buf, size_t buflen,
                    struct netent ** result,
                    int * h_errnop)
{
    __taint_netent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

// struct servent
// {
//     char *s_name;
//     char **s_aliases;
//     int s_port;
//     char *s_proto;
// };

static inline void __taint_servent(struct servent* p)
{/*T:H*/
    __DO_TAINT(p->s_name);
    __DO_TAINT(p->s_aliases[0]);
    __DO_TAINT(p->s_proto);
}

static inline struct servent* __get_tainted_servent(void)
{/*T:H*/
    struct servent* p;
    __taint_servent(p);
    return p;
}

void setservent (int stay_open) {}

void endservent (void) {}

struct servent *getservent (void) { return __get_tainted_servent(); }

struct servent *getservbyname (const char *name, const char *proto) { return __get_tainted_servent(); }

struct servent *getservbyport (int port, const char *proto) { return __get_tainted_servent(); }

int getservent_r (struct servent * result_buf,
                  char * buf, size_t buflen,
                  struct servent ** result)
{
    __taint_servent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int getservbyname_r (const char * name,
                     const char * proto,
                     struct servent * result_buf,
                     char * buf, size_t buflen,
                     struct servent ** result)
{
    __taint_servent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int getservbyport_r (int port, const char * proto,
                     struct servent * result_buf,
                     char * buf, size_t buflen,
                     struct servent ** result)
{
    __taint_servent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

// struct protoent
// {
//     char *p_name;
//     char **p_aliases;
//     int p_proto;
// };

static inline void __taint_protoent(struct protoent * p)
{/*T:H*/
    __DO_TAINT(p->p_name);
    __DO_TAINT(p->p_aliases[0]);
}

static inline struct protoent* __get_tainted_protoent(void)
{/*T:H*/
    struct protoent* p;
    __taint_protoent(p);
    return p;
}

void setprotoent (int stay_open) {}

void endprotoent (void) {}

struct protoent *getprotoent (void) { return __get_tainted_protoent(); }

struct protoent *getprotobyname (const char *name) { return __get_tainted_protoent(); }

struct protoent *getprotobynumber (int proto) { return __get_tainted_protoent(); }

int getprotoent_r (struct protoent * result_buf,
                   char * buf, size_t buflen,
                   struct protoent ** result)
{
    __taint_protoent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int getprotobyname_r (const char * name,
                      struct protoent * result_buf,
                      char * buf, size_t buflen,
                      struct protoent ** result)
{
    __taint_protoent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int getprotobynumber_r (int proto,
                        struct protoent * result_buf,
                        char * buf, size_t buflen,
                        struct protoent ** result)
{
    __taint_protoent(result_buf);
    __DO_TAINT(buf);
    *result = result_buf;
}

int setnetgrent (const char *netgroup) {}

void endnetgrent (void) {}

int getnetgrent (char $tainted ** hostp, char $tainted ** userp, char $tainted ** domainp) {}

int innetgr (const char *netgroup, const char *host, const char *user, const char *domain) {}

int getnetgrent_r (char $tainted ** hostp, char $tainted ** userp,
                   char $tainted ** domainp, char $tainted * buffer, size_t buflen) {}

int rcmd (char $tainted ** ahost, unsigned short int rport,
          const char * locuser, const char * remuser,
          const char * cmd, int * fd2p) {}

int rcmd_af (char $tainted ** ahost, unsigned short int rport,
             const char * locuser, const char * remuser,
             const char * cmd, int * fd2p, sa_family_t af) {}

int rexec (char $tainted ** ahost, int rport,
           const char * name, const char * pass, const char * cmd, int * fd2p) {}

int rexec_af (char $tainted ** ahost, int rport,
              const char * name, const char * pass,
              const char * cmd, int * fd2p, sa_family_t af) {}

int ruserok (const char *rhost, int suser,
             const char *remuser, const char *locuser) {}

int ruserok_af (const char *rhost, int suser,
                const char *remuser, const char *locuser,
                sa_family_t af) {}

int rresvport (int *alport) {}

int rresvport_af (int *alport, sa_family_t af) {}

// struct addrinfo
// {
//     int ai_flags;
//     int ai_family;
//     int ai_socktype;
//     int ai_protocol;
//     socklen_t ai_addrlen;
//     struct sockaddr *ai_addr;
//     char *ai_canonname;
//     struct addrinfo *ai_next;
// };

static inline void __taint_addrinfo(struct addrinfo *p)
{/*T:H*/
    __DO_TAINT(p->ai_canonname);
    p->ai_next = p;
}

// struct gaicb
// {
//     const char *ar_name;
//     const char *ar_service;
//     const struct addrinfo *ar_request;
//     struct addrinfo *ar_result;

//     int __return;
//     int unused[5];
// };

static inline void __taint_gaicb(struct gaicb * p)
{/*T:H*/
    __taint_addrinfo(p->ar_result);
}

int getaddrinfo (const char * name,
                 const char * service,
                 const struct addrinfo * req,
                 struct addrinfo ** pai)
{
    __taint_addrinfo(*pai);
}

void freeaddrinfo (struct addrinfo *ai) {}

// 2006-05-08 - this is no longer tainted.  Result is from an array of static
// strings indexed by ecode, so usually just false positives.
const char *gai_strerror (int ecode) {}
// const char $tainted *gai_strerror (int ecode) {}

int getnameinfo (const struct sockaddr * sa,
                 socklen_t salen, char $tainted * host,
                 socklen_t hostlen, char $tainted * serv,
                 socklen_t servlen, unsigned int flags) {}

int getaddrinfo_a (int mode, struct gaicb *list[], int ent, struct sigevent * sig)
{ __taint_gaicb(list[0]); }

int gai_suspend (const struct gaicb *const list[], int ent, const struct timespec *timeout)
{}

int gai_error (struct gaicb *req) { __taint_gaicb(req); }

int gai_cancel (struct gaicb *gaicbp) { __taint_gaicb(gaicbp); }
