#include <glibc-fs/prelude.h>
#include <execinfo.h>

int backtrace (void $tainted * $tainted *array, int size) {}

char $tainted **backtrace_symbols (void $tainted *const *array, int size) {}

void backtrace_symbols_fd (void $tainted *const *array, int size, int fd) {}
