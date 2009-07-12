/* see License.txt for copyright and terms of use */

#include "libqual/buffer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Invariant: buffer always null-terminated */
struct growbuf_struct
{
  region r;
  unsigned int maxsize, cursize;
  char *buffer;
};

/* Make a new buffer with initial size */
growbuf growbuf_new(region r, int size)
{
  growbuf b = ralloc(r, struct growbuf_struct);

  assert(size > 0);
  b->r = r;
  b->maxsize = size; /* Force some growth! */
  b->cursize = 1;
  b->buffer = rstralloc(r, size);
  b->buffer[0] = '\0';
  return b;
}

/* Empty a buffer */
void growbuf_reset(growbuf b)
{
  assert(b->maxsize > 0);
  b->cursize = 1;
  b->buffer[0] = '\0';
}

/* Print to a buffer */
int gprintf(growbuf b, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  return gvprintf(b, fmt, args);
}

/* Print to a buffer */
int gvprintf(growbuf b, const char *fmt, va_list args)
{
  int nchars;

  if (!fmt) /* Bug (?)/feature of vnsprintf -- printing \0 returns -1,
	       goes into infinite loop. */
    return 0;
  while (1)
    {
      char *bufStart;
      int sizeLeft;

      bufStart = b->buffer + b->cursize - 1; /* chop trailing \0 */
      sizeLeft = b->maxsize - b->cursize + 1; /* +1 size we're chooping
						 the trailing \0 */
      assert(*bufStart == '\0');
      nchars = vsnprintf(bufStart, sizeLeft, fmt, args);
      if (nchars > -1 && nchars < sizeLeft)
	{
	  b->cursize += nchars; /* nchars doesn't include \0,
				   but we overwrote our \0 */
	  break;
	}
      else
	{
	  /* How much room do we need?  In the new glibc, nchars
	     tells us how much (not including the trailing null).
	     So we need the current size, -1 since we'll remove the null,
	     plus the new size, plus 1 for the new null. */
	  int newSize = (nchars > -1) ? b->cursize - 1 + nchars + 1
	                              : b->maxsize * 2;
	  char *newBuf;

	  /* fprintf(stderr, "Reallocating buffer, newSize=%d\n", newSize); */
	  newBuf = rstralloc(b->r, newSize);
	  memcpy(newBuf, b->buffer, b->cursize);
	  newBuf[b->cursize-1] = '\0';  /* vsnprintf has printed something! */
	  b->buffer = newBuf;
	  b->maxsize = newSize;
	  /* b->cursize unchanged */
	}
    }
  return nchars;
}

/* Get the contents of a buffer */
char *growbuf_contents(growbuf b)
{
  return b->buffer;
}

bool growbuf_empty(growbuf b)
{
  return b->cursize == 1; /* Buffer always null terminated */
}
