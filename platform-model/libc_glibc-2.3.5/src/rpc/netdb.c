#include <glibc-fs/prelude.h>
#include <rpc/netdb.h>

// struct rpcent
// {
//     char *r_name;
//     char **r_aliases;
//     int r_number;
// };

static inline void __taint_rpcent(struct rpcent * p)
{/*T:H*/
    __DO_TAINT(p->r_name);
    __DO_TAINT(p->r_aliases[0]);
}

static inline struct rpcent* __get_tainted_rpcent(void)
{/*T:H*/
    struct rpcent* p;
    __taint_rpcent(p);
    return p;
}

void setrpcent (int stayopen) {}
void endrpcent (void) {}
struct rpcent *getrpcbyname (const char *name) { return __get_tainted_rpcent(); }
struct rpcent *getrpcbynumber (int number) { return __get_tainted_rpcent(); }
struct rpcent *getrpcent (void) { return __get_tainted_rpcent(); }

int getrpcbyname_r (const char *name, struct rpcent *result_buf,
                    char $tainted *buffer, size_t buflen,
                    struct rpcent **result)
{
    __taint_rpcent(result_buf);
    *result = result_buf;
}

int getrpcbynumber_r (int number, struct rpcent *result_buf,
                      char $tainted *buffer, size_t buflen,
                      struct rpcent **result)
{
    __taint_rpcent(result_buf);
    *result = result_buf;
}

int getrpcent_r (struct rpcent *result_buf, char $tainted *buffer,
                 size_t buflen, struct rpcent **result)
{
    __taint_rpcent(result_buf);
    *result = result_buf;
}
