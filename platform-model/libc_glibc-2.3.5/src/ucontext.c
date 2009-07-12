#include <glibc-fs/prelude.h>
#include <ucontext.h>

// typedef struct
// {
//     gregset_t gregs;
//     fpregset_t fpregs;
//     unsigned long int oldmask;
//     unsigned long int cr2;
// } mcontext_t;

// typedef struct ucontext
// {
//     unsigned long int uc_flags;
//     struct ucontext *uc_link;
//     stack_t uc_stack;
//     mcontext_t uc_mcontext;
//     __sigset_t uc_sigmask;
//     struct _libc_fpstate fpregs_mem;
// } ucontext_t;

// TODO: I guess we could taint all the gregs & fpregs to be conservative

int getcontext (ucontext_t *ucp) {}

int setcontext (const ucontext_t *ucp) {}

int swapcontext (ucontext_t * oucp, const ucontext_t * ucp) {}

void makecontext (ucontext_t *ucp, void (*__func) (void), int argc, ...) {
    va_list arg;
    va_start(arg, argc);
    __taint_vararg_environment(arg);
    va_end(arg);
}
