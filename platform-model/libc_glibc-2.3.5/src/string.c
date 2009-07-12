// $Id$

// string functions
//    (string.h is a superset of strings.h)

// these functions are declared in bits/string2.h:
//    __rawmemchr
//    __strsep_g
//    __strdup
//    __strndup

#include <glibc-fs/prelude.h>
#include <string.h>

FAKEBUILTIN
void $_1_2 * memcpy(void $_1_2 * dest, void const $_1 * src, size_t n) {}

FAKEBUILTIN
void $_1_2 * memmove (void $_1_2 * dest, const void $_1 * src, size_t n) {}

void $_1_2 * memccpy (void $_1_2 * dest, const void $_1 * src, int c, size_t n) {}

FAKEBUILTIN
void $_1_2 * memset (void $_1_2 * s, int $_1 c, size_t n) {}

FAKEBUILTIN
int memcmp (const void * s1, const void * s2, size_t n) {}

void $_1 * memchr (const void $_1 * s, int c, size_t n) {}

void $_1 * rawmemchr (const void $_1 * s, int c) {}
void $_1 * __rawmemchr (const void $_1 * s, int c) {}

void $_1 * memrchr (const void $_1 * s, int c, size_t n) {}

FAKEBUILTIN
char $_1_2 * strcpy (char $_1_2 * dest, const char $_1 * src) {}

FAKEBUILTIN
char $_1_2 * strncpy (char $_1_2 * dest, const char $_1 * src, size_t n) {}

FAKEBUILTIN
char $_1_2 * strcat (char $_1_2 * dest, const char $_1 * src) {}

FAKEBUILTIN
char $_1_2 * strncat (char $_1_2 * dest, const char $_1 * src, size_t n) {}

FAKEBUILTIN
int strcmp (const char * s1, const char * s2) {}

FAKEBUILTIN
int strncmp (const char * s1, const char * s2, size_t n) {}

int strcoll (const char * s1, const char * s2) {}

size_t strxfrm (char $_1_2 * dest, const char $_1 * src, size_t n) {}

int strcoll_l (const char * s1, const char * s2, __locale_t l) {}

size_t strxfrm_l (char $_1_2 * dest, const char $_1 * src, size_t n, __locale_t l) {}

FAKEBUILTIN
char $_1_2 * strdup(const char $_1 * s1) {}
char $_1_2 * __strdup(const char $_1 * s1) {}

char $_1_2 * strndup (const char $_1 * string, size_t n) {}
char $_1_2 * __strndup (const char $_1 * string, size_t n) {}

FAKEBUILTIN
char $_1 * strchr (const char $_1 * s, int c) {}

FAKEBUILTIN
char $_1 * strrchr (const char $_1 * s, int c) {}

char $_1 * strchrnul (const char $_1 * s, int c) {}

FAKEBUILTIN
size_t strcspn (const char * s, const char * reject) {}

FAKEBUILTIN
size_t strspn (const char * s, const char * accept) {}

FAKEBUILTIN
char $_1 * strpbrk (const char $_1 * s, const char * accept) {}

FAKEBUILTIN
char $_1_2 * strstr (const char $_1_2 * haystack, const char $_2 * needle) {}

char $_1_2 * strtok (char $_1_2 * s, const char $_1 * delim) {
    static char * p;
    *p = *delim;
    *s = *p;
    return s;
}

char $_1_2 * __strtok_r (char $_1_2 * s, const char $_1 * delim, char $_1_2 * * save_ptr) {}
char $_1_2 * strtok_r (char $_1_2 * s, const char $_1 * delim, char $_1_2 * * save_ptr) {}

char $_1_2 * strcasestr (const char $_1_2 * haystack, const char $_2 * needle) {}

void $_1_2 * memmem (const void $_1_2 * haystack, size_t haystacklen,
		     const void $_2 * needle, size_t needlelen) {}

void $_1_2 * __mempcpy (void $_1_2 * dest, const void $_1 * src, size_t n) {}
FAKEBUILTIN
void $_1_2 * mempcpy (void $_1_2 * dest, const void $_1 * src, size_t n) {}

FAKEBUILTIN
size_t strlen (const char * s) {}

size_t strnlen (const char * string, size_t maxlen) {}

// 2006-05-08 - this is no longer tainted.  Result is from an array of static
// strings indexed by errnum, so usually just false positives.
char * strerror (int errnum) {}
// char $tainted * strerror (int errnum) {}

// char $tainted * strerror_r (int errnum, char $tainted * buf, size_t buflen) {}
char * strerror_r (int errnum, char * buf, size_t buflen) { return buf; }

FAKEBUILTIN
void bzero (void * s, size_t n) {}
void __bzero (void * s, size_t n) {}

FAKEBUILTIN
void bcopy (const void $_1 * src, void $_1_2 * dest, size_t n) {}

FAKEBUILTIN
int bcmp (const void * s1, const void * s2, size_t n) {}

FAKEBUILTIN
char $_1 * index (const char $_1 * s, int c) {}

FAKEBUILTIN
char $_1 * rindex (const char $_1 * s, int c) {}

FAKEBUILTIN
int ffs (int i) {}
FAKEBUILTIN
int ffsl (long int l) {}
FAKEBUILTIN
int ffsll (long long int ll) {}

int strcasecmp (const char * s1, const char * s2) {}

int strncasecmp (const char * s1, const char * s2, size_t n) {}

int strcasecmp_l (const char * s1, const char * s2, __locale_t loc) {}

int strncasecmp_l (const char * s1, const char * s2, size_t n, __locale_t loc) {}

char $_1_2 * strsep (char $_1_2 * $untainted * stringp, const char $_2 * delim) {}
char $_1_2 * strsep_g (char $_1_2 * $untainted * stringp, const char $_2 * delim) {}

int strverscmp (const char * s1, const char * s2) {}

char * strsignal (int sig) {}

FAKEBUILTIN
char $_1_2 * stpcpy (char $_1_2 * dest, const char $_1 * src) {}
char $_1_2 * __stpcpy (char $_1_2 * dest, const char $_1 * src) {}

char $_1_2 * stpncpy (char $_1_2 * dest, const char $_1 * src, size_t n) {}
char $_1_2 * __stpncpy (char $_1_2 * dest, const char $_1 * src, size_t n) {}

char $_1 * strfry (char $_1 * string) {}

void $_1 * memfrob (void $_1 * s, size_t n) {}

char $_1 * basename (const char $_1 * filename) {}
