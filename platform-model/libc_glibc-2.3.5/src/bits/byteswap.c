#ifndef GLIBCFS_IGNORE_INCOMPLETE

#error "This file can't be annotated correctly yet due to inline assembly."

#include <glibc-fs/prelude.h>

// XXX TODO: what to do about inline assembly?  so far just warn about
// assembly and ignore it; a grody hack could be to match for the exact
// strings we know about if there are only a few of them.

#  define __bswap_16(x)                                           \
    (__extension__                                                \
     ({ register unsigned short int v, x = (x);                   \
	 if (__builtin_constant_p (__x))                          \
             __v = __bswap_constant_16 (__x);                     \
	 else                                                     \
             __asm__ ("rorw $8, %w0"                              \
                      : "=r" (__v)                                \
                      : "0" (__x)                                 \
                      : "cc");                                    \
	 __v; }))

#   define __bswap_32(x)                                          \
    (__extension__                                                \
     ({ register unsigned int v, x = (x);                         \
	 if (__builtin_constant_p (__x))                          \
             __v = __bswap_constant_32 (__x);                     \
	 else                                                     \
             __asm__ ("rorw $8, %w0;"                             \
                      "rorl $16, %0;"                             \
                      "rorw $8, %w0"                              \
                      : "=r" (__v)                                \
                      : "0" (__x)                                 \
                      : "cc");                                    \
	 __v; }))

#   define __bswap_32(x)                                          \
    (__extension__                                                \
     ({ register unsigned int v, x = (x);                         \
	 if (__builtin_constant_p (__x))                          \
             __v = __bswap_constant_32 (__x);                     \
	 else                                                     \
             __asm__ ("bswap %0" : "=r" (__v) : "0" (__x));       \
	 __v; }))

// there are non-x86 versions but we don't care about the non-assembly
// inlined functions/macros because they work as-is.

#endif
