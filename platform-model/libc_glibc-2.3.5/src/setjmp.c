#include <glibc-fs/prelude.h>
#include <setjmp.h>

#undef setjmp

// typedef struct __jmp_buf_tag
// {
//     __jmp_buf jmpbuf;
//     int mask_was_saved;
//     __sigset_t saved_mask;
// } jmp_buf[1];

// quarl 2006-07-06
//    Avoid these being FAKEBUILTIN for now if only because we'd have to
//    declare the type of their parameters.

// FAKEBUILTIN
int setjmp (jmp_buf env) {}

int __sigsetjmp (struct __jmp_buf_tag env[1], int savemask) {}

int _setjmp (struct __jmp_buf_tag env[1]) {}

// FAKEBUILTIN
void longjmp (struct __jmp_buf_tag env[1], int val) { __void_noreturn(); }

void _longjmp (struct __jmp_buf_tag env[1], int val) { __void_noreturn(); }

typedef struct __jmp_buf_tag sigjmp_buf[1];

void siglongjmp (sigjmp_buf env, int val) { __void_noreturn(); }
