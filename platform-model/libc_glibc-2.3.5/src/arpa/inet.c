#include <glibc-fs/prelude.h>

// We avoid including arpa/inet.h so that incompatible types (such as return
// value of inet_ntoa being either int or in_addr_t) don't hurt us.  Since we
// don't taint those, it's okay.

#include <netinet/in.h>
// #include <arpa/inet.h>

in_addr_t inet_addr (const char *cp) {}

in_addr_t inet_lnaof (struct in_addr in) {}

struct in_addr inet_makeaddr (in_addr_t net, in_addr_t host) {}

in_addr_t inet_netof (struct in_addr in) {}

in_addr_t inet_network (const char *cp) {}

char $tainted *inet_ntoa (struct in_addr in) {}

int inet_pton (int af, const char * cp, void $tainted * buf) {}

const char $tainted *inet_ntop (int af, const void * cp, char $tainted * buf, socklen_t len) {}

int inet_aton (const char *cp, struct in_addr *inp) {}

char $tainted *inet_neta (in_addr_t net, char $tainted *buf, size_t len) {}

char $tainted *inet_net_ntop (int af, const void *cp, int bits, char $tainted *buf, size_t len) {}

int inet_net_pton (int af, const char *cp, void $tainted *buf, size_t len) {}

unsigned int inet_nsap_addr (const char *cp, unsigned char $tainted *buf, int len) {}

char $tainted *inet_nsap_ntoa (int len, const unsigned char *cp, char $tainted *buf) {}
