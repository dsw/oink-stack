#include <glibc-fs/prelude.h>
#include <sys/gmon.h>

void __monstartup (u_long lowpc, u_long highpc) {}
void monstartup (u_long lowpc, u_long highpc) {}

void _mcleanup (void) {}
