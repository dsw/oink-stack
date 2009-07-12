#include <glibc-fs/prelude.h>
#include <mcheck.h>

int mcheck (void (*__abortfunc) (enum mcheck_status)) {}

int mcheck_pedantic (void (*__abortfunc) (enum mcheck_status)) {}

void mcheck_check_all (void) {}

enum mcheck_status mprobe (void *ptr) {}

void mtrace (void) {}
void muntrace (void) {}
