#include <glibc-fs/prelude.h>
#include <netinet/in.h>

uint32_t ntohl (uint32_t netlong) {}
uint16_t ntohs (uint16_t netshort) {}
uint32_t htonl (uint32_t hostlong) {}
uint16_t htons (uint16_t hostshort) {}

int bindresvport (int sockfd, struct sockaddr_in *sock_in) {}

int bindresvport6 (int sockfd, struct sockaddr_in6 *sock_in) {}

int inet6_option_space (int nbytes) {}
int inet6_option_init (void *bp, struct cmsghdr **cmsgp, int type) {}
int inet6_option_append (struct cmsghdr *cmsg, const uint8_t *typep, int multx, int plusy) {}
uint8_t *inet6_option_alloc (struct cmsghdr *cmsg, int datalen, int multx, int plusy) {}
int inet6_option_next (const struct cmsghdr *cmsg, uint8_t **tptrp) {}
int inet6_option_find (const struct cmsghdr *cmsg, uint8_t **tptrp, int type) {}

int getipv4sourcefilter (int s, struct in_addr interface_addr,
                         struct in_addr group, uint32_t *fmode,
                         uint32_t *numsrc, struct in_addr *slist)
{}

int setipv4sourcefilter (int s, struct in_addr interface_addr,
                         struct in_addr group, uint32_t fmode,
                         uint32_t numsrc,
                         const struct in_addr *slist)
{}

int getsourcefilter (int s, uint32_t interface_addr,
                     const struct sockaddr *group,
                     socklen_t grouplen, uint32_t *fmode,
                     uint32_t *numsrc,
                     struct sockaddr_storage *slist)
{}

int setsourcefilter (int s, uint32_t interface_addr,
                     const struct sockaddr *group,
                     socklen_t grouplen, uint32_t fmode,
                     uint32_t numsrc,
                     const struct sockaddr_storage *slist)
{}
