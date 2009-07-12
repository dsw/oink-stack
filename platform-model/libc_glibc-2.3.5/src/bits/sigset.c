#include <glibc-fs/prelude.h>
#include <bits/sigset.h>

int __sigismember (const __sigset_t *s, int i) {}
int __sigaddset (__sigset_t *s, int i) {}
int __sigdelset (__sigset_t *s, int i) {}
