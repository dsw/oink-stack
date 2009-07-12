#include <glibc-fs/prelude.h>
#include <pty.h>

int openpty (int *amaster, int *aslave, char $tainted *name, struct termios *termp, struct winsize *winp) {}

int forkpty (int *amaster, char $tainted *name, struct termios *termp, struct winsize *winp) {}
