/* see License.txt for copyright and terms of use */

#ifndef BUFFER_H
#define BUFFER_H

/*
 * Growable buffers
 *
 * Always null terminated.
 */

#include "libqual/bool.h"
#include "libregion/regions.h"
#include <stdarg.h>

typedef struct growbuf_struct *growbuf;

growbuf growbuf_new(region, int);   /* Make a new buffer with initial size */
void growbuf_reset(growbuf);        /* Empty a buffer */
int gprintf(growbuf, const char *, ...);      /* Print to a buffer */
int gvprintf(growbuf, const char *, va_list); /* Print to a buffer */
char *growbuf_contents(growbuf);    /* Get the contents of a buffer */
bool growbuf_empty(growbuf);        /* Return true iff buffer is empty */

#endif
