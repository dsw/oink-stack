#include <glibc-fs/prelude.h>
#include <sys/socket.h>

// // from bits/uio.h
// struct iovec
// {
//     void *iov_base;
//     size_t iov_len;
// };

// // from bits/socket.h
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

static inline void __taint_msghdr(struct msghdr* p)
{/*T:H*/
    __DO_TAINT(p->msg_name);
    __DO_TAINT(p->msg_iov->iov_base);
    __DO_TAINT(p->msg_control);
}

int socket (int domain, int type, int protocol) {}

int socketpair (int domain, int type, int protocol, int fds[2]) {}

int bind (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len) {}

int getsockname (int fd, __SOCKADDR_ARG addr, socklen_t * len) {}

int connect (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len) {}

int getpeername (int fd, __SOCKADDR_ARG addr, socklen_t * len) {}

ssize_t send (int fd, const void $tainted *buf, size_t n, int flags) {}

ssize_t recv (int fd, void $tainted *buf, size_t n, int flags) {}

ssize_t sendto (int fd, const void *buf, size_t n,
                int flags, __CONST_SOCKADDR_ARG addr,
                socklen_t addr_len) {}

ssize_t recvfrom (int fd, void $tainted * buf, size_t n, int flags,
                  __SOCKADDR_ARG addr, socklen_t * addr_len) {}

ssize_t sendmsg (int fd, const struct msghdr *message, int flags) {}

ssize_t recvmsg (int fd, struct msghdr *message, int flags)
{
    __taint_msghdr(message);
}

int getsockopt (int fd, int level, int optname, void * optval, socklen_t * optlen) {}

int setsockopt (int fd, int level, int optname, const void *optval, socklen_t optlen) {}

int listen (int fd, int n) {}

int accept (int fd, __SOCKADDR_ARG addr, socklen_t * addr_len) {}

int shutdown (int fd, int how) {}

int sockatmark (int fd) {}

int isfdtype (int fd, int fdtype) {}
