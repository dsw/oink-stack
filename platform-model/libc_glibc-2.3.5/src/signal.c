#include <glibc-fs/prelude.h>
#include <signal.h>

__sighandler_t __sysv_signal (int sig, __sighandler_t handler) {}

__sighandler_t sysv_signal (int sig, __sighandler_t handler) {}

__sighandler_t signal (int sig, __sighandler_t handler) {}

__sighandler_t bsd_signal (int sig, __sighandler_t handler) {}

int kill (__pid_t pid, int sig) {}

int killpg (__pid_t pgrp, int sig) {}

int raise (int sig) {}

__sighandler_t ssignal (int sig, __sighandler_t handler) {}
int gsignal (int sig) {}

void psignal (int sig, const char *s) {}

int __sigpause (int sig_or_mask, int is_sig) {}

#undef sigpause
int sigpause (int mask) {}
// int __xpg_sigpause (int sig) {}

int sigblock (int mask) {}

int sigsetmask (int mask) {}

int siggetmask (void) {}

int sigemptyset (sigset_t *set) {}

int sigfillset (sigset_t *set) {}

int sigaddset (sigset_t *set, int signo) {}

int sigdelset (sigset_t *set, int signo) {}

int sigismember (const sigset_t *set, int signo) {}

int sigisemptyset (const sigset_t *set) {}

int sigandset (sigset_t *set, const sigset_t *left, const sigset_t *right) {}

int sigorset (sigset_t *set, const sigset_t *left, const sigset_t *right) {}

int sigprocmask (int how, const sigset_t * set, sigset_t * oset) {}

int sigsuspend (const sigset_t *set) {}

int sigaction (int sig, const struct sigaction * act, struct sigaction * oact) {}

int sigpending (sigset_t *set) {}

int sigwait (const sigset_t * set, int * sig) {}

int sigwaitinfo (const sigset_t * set, siginfo_t * info) {}

int sigtimedwait (const sigset_t * set, siginfo_t * info, const struct timespec * timeout) {}

int sigqueue (__pid_t pid, int sig, const union sigval val) {}

int sigvec (int sig, const struct sigvec *vec, struct sigvec *ovec) {}

int sigreturn (struct sigcontext *scp) {}

int siginterrupt (int sig, int interrupt) {}

int sigstack (struct sigstack *ss, struct sigstack *oss) {}

int sigaltstack (const struct sigaltstack * ss, struct sigaltstack * oss) {}

int sighold (int sig) {}

int sigrelse (int sig) {}

int sigignore (int sig) {}

__sighandler_t sigset (int sig, __sighandler_t disp) {}

int __libc_current_sigrtmin (void) {}

int __libc_current_sigrtmax (void) {}
