#include <glibc-fs/prelude.h>
#include <alloca.h>

#undef alloca
FAKEBUILTIN
void *alloca (size_t size) {}

// # define alloca(size)	__builtin_alloca (size)
