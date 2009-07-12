// These could be merged to pthread.c, but that file is not complete yet, and
// <bits/sigthread.h> is included in a bunch of other headers so we need these
// definitions.

#include <glibc-fs/prelude.h>
// #include <bits/sigthread.h>
#include <signal.h>

int pthread_sigmask (int how, const __sigset_t * newmask, __sigset_t * oldmask) {}

int pthread_kill (pthread_t threadid, int signo) {}

