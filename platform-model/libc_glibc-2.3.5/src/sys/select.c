#include <glibc-fs/prelude.h>
#include <sys/select.h>

int select (int nfds, fd_set * readfds, fd_set * writefds,
            fd_set * exceptfds, struct timeval * timeout) {}

int __select (int nfds, fd_set * readfds, fd_set * writefds,
              fd_set * exceptfds, struct timeval * timeout) {}

int pselect (int nfds, fd_set * readfds, fd_set * writefds,
             fd_set * exceptfds, const struct timespec * timeout, const __sigset_t * sigmask) {}
