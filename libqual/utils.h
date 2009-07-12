/* see License.txt for copyright and terms of use */

#ifndef UTILS_H
#define UTILS_H

#include "libqual/bool.h"
#include "libregion/regions.h"
#include <assert.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

unsigned long align_to(unsigned long n, unsigned long alignment);

/* least common multiple */
unsigned long lcm(unsigned long x, unsigned long y);

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifdef HAVE_VARIADIC_MACROS
#define fail(args...) __fail(__FILE__, __LINE__, __FUNCTION__, args)
#else
void fail(const char *fmt, ...) __attribute__ ((__noreturn__));
#endif

void __fail(const char *file, unsigned int line,
	    const char *func, const char *fmt, ...) __attribute__ ((__noreturn__));

#ifdef HAVE_VARIADIC_MACROS
#define user_error(args...) __user_error(__FILE__, __LINE__, __FUNCTION__, args)
#else
void user_error(const char *fmt, ...) __attribute__ ((__noreturn__));
#endif

void __user_error(const char *file, unsigned int line,
		  const char *func, const char *fmt, ...)
  __attribute__ ((__noreturn__));

/* insist(action) is like assert(action), but action may have
   side-effects */
#ifdef NDEBUG
# define insist(action)  (action)
#else
# define insist assert
#endif

#ifdef NDEBUG
# define insistnot(action)  (action)
#else
# define insistnot(action) assert(!(action))
#endif

/* Concatenate 2 strings, allocating space in r for the result */
char *rstrcat(region, const char *, const char *);

/* Concatenate n strings, allocating space in r for the result.  The
   last argument should be a null pointer. */
char *rstrscat(region, ...);

/* Convert an integer to a string, storing the result in r */
const char *inttostr(region r, int);

/* sprintf a string, allocating space in r for the result */
char *rsprintf(region r, const char *fmt, ...);
char *rvsprintf(region r, const char *fmt, va_list args);

/* Convert a pointer to an ascii string with leading 0x.  Re-uses
   internal buffer. */
char *ptr_to_ascii(void *ptr);

/* Convert a pointer to an integer */
static inline unsigned long ptr_hash(void *ptr)
{
  static const unsigned long  LLMAGIC =
    ((unsigned long long)((2*0.6180339987) * (1ULL << (8 * sizeof(unsigned long) - 1))));

  return ((unsigned long) ptr >> 2) * LLMAGIC;
}

/* Return TRUE iff ptr1 == ptr2 */
static inline bool ptr_eq(void *ptr1, void *ptr2)
{
  return ptr1 == ptr2;
}

/* Hash a string */
unsigned long string_hash(const char *str);

/* Return TRUE iff s1 == s2 */
bool string_eq(const char *s1, const char *s2);

/* A total ordering on pointers.  Returns 0 if ptr1 = ptr2, a value <0
   if ptr1 < ptr2, or a value >0 if ptr1 > ptr2. */
static inline
int ptr_cmp(void *ptr1, void *ptr2)
{
  return (char *) ptr1 - (char *) ptr2;
}

/* A function to let you pass a function pointer which prints to
   different file descriptors */
extern FILE *fdprintf_fd;
int fdprintf(const char *format, ...);

#endif
