#include <glibc-fs/prelude.h>
#include <ifaddrs.h>

// struct ifaddrs
// {
//     struct ifaddrs *ifa_next;

//     char *ifa_name;
//     unsigned int ifa_flags;

//     struct sockaddr *ifa_addr;
//     struct sockaddr *ifa_netmask;
//     union
//     {

//         struct sockaddr *ifu_broadaddr;
//         struct sockaddr *ifu_dstaddr;
//     } ifa_ifu;

//     void *ifa_data;
// };

static inline void __taint_ifaddrs(struct ifaddrs *p)
{/*T:H*/
    __DO_TAINT(p->ifa_name);
    __DO_TAINT(p->ifa_data);
}

int getifaddrs (struct ifaddrs **ifap) {
    __taint_ifaddrs(ifap[0]);
}

void freeifaddrs (struct ifaddrs *ifa) {}
