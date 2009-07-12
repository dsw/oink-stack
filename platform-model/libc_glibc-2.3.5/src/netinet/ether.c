#include <glibc-fs/prelude.h>
#include <netinet/ether.h>

char $tainted *ether_ntoa (const struct ether_addr *addr) {}
char $tainted *ether_ntoa_r (const struct ether_addr *addr, char $tainted *buf) {}

struct ether_addr *ether_aton (const char *asc) {}
struct ether_addr *ether_aton_r (const char *asc, struct ether_addr *addr) {}

int ether_ntohost (char $tainted *hostname, const struct ether_addr *addr) {}

int ether_hostton (const char *hostname, struct ether_addr *addr) {}

int ether_line (const char *line, struct ether_addr *addr, char $tainted *hostname) {}
