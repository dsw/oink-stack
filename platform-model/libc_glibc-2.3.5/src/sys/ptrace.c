#include <glibc-fs/prelude.h>
#include <sys/ptrace.h>

long int ptrace (enum __ptrace_request request, ...)
{
    va_list arg;
    va_start(arg, request);
    __taint_vararg_environment(arg);
    va_end(arg);
}
// long int ptrace (enum __ptrace_request request, pid_t pid, void $tainted* addr, void $tainted* data)
// {}

