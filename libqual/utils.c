/* see License.txt for copyright and terms of use */

#include "libqual/buffer.h"
#include "libqual/utils.h"
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

unsigned long align_to(unsigned long n, unsigned long alignment)
{
  int count = (n + alignment - 1) / alignment;

  return count * alignment;
}

unsigned long gcd(unsigned long x, unsigned long y)
{
  unsigned long z;

  for (;;)
    {
      if (y == 0)
	return x;

      z = x % y; x = y; y = z;
    }
}

unsigned long lcm(unsigned long x, unsigned long y)
{
  /* ignoring risk of overflow (used for alignments which are typically <= 16) */
  return (x * y) / gcd(x, y);
}

#ifndef HAVE_STRTOLD
long double strtold(const char *str, char **endptr)
{
  long double ld;
  int count;

  assert(!endptr);
  count = sscanf(str, "%Lf", &ld);
  assert(count == 1);

  return ld;
}
#endif

/* Panic with a message */
static void vfail(const char *fmt, va_list args) __attribute__((__noreturn__));

static void vfail(const char *fmt, va_list args)
{
  vfprintf(stderr, fmt, args);
  fflush(stdin);
  fflush(stderr);
  fflush(stdout);
  sync();
  fsync(STDIN_FILENO);
  fsync(STDERR_FILENO);
  fsync(STDOUT_FILENO);
  abort();
  while (1); /* Work around stupid gcc-2.96-85 bug */
}

/* Panic with a nice message */
void __fail(const char *file, unsigned int line,
	    const char *func __attribute__((unused)),
	    const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "\n%s:%u ", file, line);
  vfail(fmt, args);
}

#ifndef HAVE_VARIADIC_MACROS
/* Panic with a not-quite-as-nice message */
void fail(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfail(fmt, args);
}
#endif

/* Exit with a message */
static void vuser_error(const char *fmt, va_list args) __attribute__((__noreturn__));

static void vuser_error(const char *fmt, va_list args)
{
  vfprintf(stderr, fmt, args);
  fflush(stdin);
  fflush(stderr);
  fflush(stdout);
  sync();
  fsync(STDIN_FILENO);
  fsync(STDERR_FILENO);
  fsync(STDOUT_FILENO);
  exit(4);
  while (1); /* Work around stupid gcc-2.96-85 bug */
}

/* Exit with a nice message */
void __user_error(const char *file, unsigned int line,
		  const char *func __attribute__((unused)),
		  const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "\n%s:%u ", file, line);
  vuser_error(fmt, args);
}

#ifndef HAVE_VARIADIC_MACROS
/* Exit with a not-quite-as-nice message */
void user_error(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfail(fmt, args);
}
#endif

/* Concatenate 2 strings, allocating space in r for the result */
char *rstrcat(region r, const char *s1, const char *s2)
{
  char *result = rarrayalloc(r, strlen(s1)+strlen(s2)+1, char);
  result[0] = '\0';
  strcat(result, s1);
  strcat(result, s2);
  return result;
}

/* Concatenate n strings, allocating space in r for the result.  The
   last argument should be a null pointer. */
char *rstrscat(region r, ...)
{
  char *result;
  int len = 0;
  const char *s;
  va_list args;

  va_start(args, r);
  while ((s = va_arg(args, const char *)))
    len += strlen(s);
  result = rarrayalloc(r, len+1, char);
  result[0] = '\0';

  va_start(args, r);
  while ((s = va_arg(args, const char *)))
    strcat(result, s);

  return result;
}

/* Convert an integer to a string, storing the result in r */
const char *inttostr(region r, int i)
{
  char *result;
  int width, count;

  /* Compute width */
  width = 1;
  if (i < 0) {
    width++;
    i = -i;
  }
  for (count = 10; i >= count; count *= 10)
    width++;

  /* Covert to integer */
  result = rarrayalloc(r, width + 1, char);
  if (snprintf(result, width + 1, "%d", i) == -1) {
    printf("i=%d, width=%d\n", i, width);
    fail ("inttostr width wrong\n");
  }
  return result;
}


/* sprintf a string, allocating space in r for the result */
char *rsprintf(region r, const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  return rvsprintf(r, fmt, args);
}

char *rvsprintf(region r, const char *fmt, va_list args)
{
  growbuf buf = growbuf_new(r, 100);
  gvprintf(buf, fmt, args);
  return growbuf_contents(buf);
}

/* Space for the ASCII representation of a pointer -- 2 hex chars per
   byte, plus 3 chars for 0x prefix and trailing \0 */
#define PTR_ASCII_SIZE  ((int) (3 + sizeof(void *)*2))

/* Convert a pointer to an ascii string with leading 0x.  Re-uses
   internal buffer. */
char *ptr_to_ascii(void *ptr) {
  static char addr[PTR_ASCII_SIZE];
  int nchars;

  nchars = snprintf(addr, PTR_ASCII_SIZE, "%p", ptr);
  if (nchars == -1 || nchars >= PTR_ASCII_SIZE)
    fail("Unable to convert ptr to ascii (need %d bytes, have %d)\n",
	 nchars, PTR_ASCII_SIZE);
  return addr;
}

/* Hash a string */
unsigned long string_hash(const char *str)
{
  const char *c;
  unsigned long h;

  c = str;
  h = 0;
  if (!c)
    return 0;
  while (*c)
    h = 33*h + 720 + *c++; /* SML/NJ's string hash function */
  return h;
}

/* Return TRUE iff s1 == s2 */
bool string_eq(const char *s1, const char *s2)
{
  if (!s1 || !s2) /* Nulls are all distinct */
    return FALSE;
  return !strcmp(s1, s2);
}

// int min(int a, int b) { if (a < b) return a; else return b; }
// int max(int a, int b) { if (a < b) return b; else return a; }

/* A function to let you pass a function pointer which prints to
   different file descriptors */
FILE *fdprintf_fd;

int fdprintf(const char *format, ...)
{
  va_list args;

  va_start(args, format);
  return vfprintf(fdprintf_fd, format, args);
}

