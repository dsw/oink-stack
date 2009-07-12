#include <glibc-fs/prelude.h>

#define __NO_CTYPE
#include <ctype.h>

int tolower (int c) { return c; }
int toupper (int c) { return c; }
int isctype (int c, int mask) {}
int isascii (int c) {}
int toascii (int c) { return c; }

int isalnum (int c) {}
int isalpha (int c) {}
int iscntrl (int c) {}
int isdigit (int c) {}
int islower (int c) {}
int isgraph (int c) {}
int isprint (int c) {}
int ispunct (int c) {}
int isspace (int c) {}
int isupper (int c) {}
int isxdigit (int c) {}
int isblank (int c) {}
int _toupper (int c) { return c; }
int _tolower (int c) { return c; }

int isalnum_l (int c, __locale_t l) {}
int isalpha_l (int c, __locale_t l) {}
int iscntrl_l (int c, __locale_t l) {}
int isdigit_l (int c, __locale_t l) {}
int islower_l (int c, __locale_t l) {}
int isgraph_l (int c, __locale_t l) {}
int isprint_l (int c, __locale_t l) {}
int ispunct_l (int c, __locale_t l) {}
int isspace_l (int c, __locale_t l) {}
int isupper_l (int c, __locale_t l) {}
int isxdigit_l (int c, __locale_t l) {}
int isblank_l (int c, __locale_t l) {}

int __tolower_l (int c, __locale_t l) {}
int tolower_l (int c, __locale_t l) {}

int __toupper_l (int c, __locale_t l) {}
int toupper_l (int c, __locale_t l) {}

const unsigned short int **__ctype_b_loc (void) {}
const __int32_t **__ctype_tolower_loc (void) {}
const __int32_t **__ctype_toupper_loc (void) {}
