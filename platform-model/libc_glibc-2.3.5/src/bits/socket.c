#include <glibc-fs/prelude.h>
#include <sys/socket.h>

// struct sockaddr
// {
//     __SOCKADDR_COMMON (sa_);
//     char sa_data[14];
// };

// struct sockaddr_storage
// {
//     __SOCKADDR_COMMON (ss_);
//     __ss_aligntype ss_align;
//     char ss_padding[_SS_PADSIZE];
// };

// struct msghdr
// {
//     void *msg_name;
//     socklen_t msg_namelen;

//     struct iovec *msg_iov;
//     size_t msg_iovlen;

//     void *msg_control;
//     size_t msg_controllen;

//     int msg_flags;
// };

// struct cmsghdr
// {
//     size_t cmsg_len;
//     int cmsg_level;
//     int cmsg_type;
//     __extension__ unsigned char cmsg_data __flexarr;
// };

struct cmsghdr * __cmsg_nxthdr (struct msghdr *mhdr, struct cmsghdr *cmsg)
{
    return (struct cmsghdr *) ((unsigned char *) cmsg + CMSG_ALIGN (cmsg->cmsg_len));
}
