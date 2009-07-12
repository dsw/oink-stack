// $Id$

// see ../string.c for definitions for functions.

// Implementations for non-inline portions of inlined functions.

// SOUNDNESS HOLE: If the implementation ends up using the pure-assembly
// version (not the select-optimized-or-not expressions as in bits/string2.c),
// we're screwed.

#include <glibc-fs/prelude.h>

void $_1_2 * __memcpy_c(void $_1_2 * dest, void const $_1 * src, size_t n) {}

void $_1_2 * __memset_cc (void $_1_2 * s, int $_1 c, size_t n) {}

void $_1_2 * __memset_cg (void $_1_2 * s, int $_1 c, size_t n) {}

void $_1_2 * __memset_gg (void $_1_2 * s, int $_1 c, size_t n) {}

void $_1 * __memrchr (const void $_1 * s, int c, size_t n) {}

char $_1 * __strchr_g (const char $_1 * s, int c) {}

char $_1 * __strchr_c (const char $_1 * s, int c) {}

char $_1 * __strchrnul_g (const char $_1 * s, int c) {}

char $_1 * __strchrnul_c (const char $_1 * s, int c) {}
