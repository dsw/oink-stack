/* see License.txt for copyright and terms of use */

#ifndef REGIONS_H
#define REGIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define RMAXMEMLOG (8*sizeof (void*))
#define RPAGELOG 13

typedef struct region_ *region;
extern region permanent;

#include <stdlib.h>

void region_init(void);
void region_cleanup(void);

region newregion(void);
region newsubregion(region parent);

void regions_print_stats(void);

typedef int type_t;
#define rctypeof(type) 0

/* Low-level alloc with dynamic type info */
void *__rc_typed_ralloc(region r, size_t size, type_t type);
void *__rc_typed_rarrayalloc(region r, size_t n, size_t size, type_t type);
void *__rc_typed_rarrayextend(region r, void *old, size_t n, size_t size, type_t type);
void typed_rarraycopy(void *to, void *from, size_t n, size_t size, type_t type);

void *__rc_ralloc_small0(region r, size_t size);

/* In theory, the test at the start of qalloc should give the same benefit.
   In practice, it doesn't (gcc, at least, generates better code for
   __rcralloc_small0 than the equivalent path through typed_ralloc */
#define ralloc(r, type) (sizeof(type) < (1 << (RPAGELOG - 3)) ? __rcralloc_small0((r), sizeof(type)) : typed_ralloc((r), sizeof(type), rctypeof(type)))
#define rarrayalloc(r, n, type) typed_rarrayalloc((r), (n), sizeof(type), rctypeof(type))
#define rarrayextend(r, old, n, type) typed_rarrayextend((r), (old), (n), sizeof(type), rctypeof(type))
#define rarraycopy(to, from, n, type) typed_rarraycopy((to), (from), (n), sizeof(type), rctypeof(type))

char *__rc_rstralloc(region r, size_t size);
char *__rc_rstralloc0(region r, size_t size);
char *__rc_rstrdup(region r, const char *s);

/* rstrextend is used to extend an old string. The string MUST have been
   initially allocated by a call to rstrextend with old == NULL (you cannot
   initially allocate the string with rstralloc) */
char *__rc_rstrextend(region r, const char *old, size_t newsize);
char *__rc_rstrextend0(region r, const char *old, size_t newsize);

void deleteregion(region r);
void deleteregion_ptr(region *r);
void deleteregion_array(int n, region *regions);
region regionof(void *ptr);

typedef void (*nomem_handler)(void);
nomem_handler set_nomem_handler(nomem_handler newhandler);
void out_of_memory();

static inline
void *xmalloc(size_t n)
{
  void *p = malloc(n);
  if (p == NULL) { out_of_memory(); abort(); }
  return p;
}

static inline
void *xcalloc(size_t n, size_t s)
{
  void *p = calloc(n, s);
  if (p == NULL) { out_of_memory(); abort(); }
  return p;
}

/* Debugging support */
void findrefs(region r, void *from, void *to);
void findgrefs(region r);
void findrrefs(region r, region from);

#ifdef DEBUG_RALLOC
extern int __rc_line;
extern char *__rc_file;
#define RCDEBUG (__rc_line = __LINE__, __rc_file = __FILE__)
#else
#define RCDEBUG ((void)0)
#endif

#ifndef REGION_PROFILE
#define typed_ralloc (RCDEBUG, __rc_typed_ralloc)
#define typed_rarrayalloc (RCDEBUG, __rc_typed_rarrayalloc)
#define typed_rarrayextend (RCDEBUG, __rc_typed_rarrayextend)
#define rstralloc (RCDEBUG, __rc_rstralloc)
#define rstralloc0 (RCDEBUG, __rc_rstralloc0)
#define rstrdup (RCDEBUG, __rc_rstrdup)
#define rstrextend (RCDEBUG, __rc_rstrextend)
#define rstrextend0 (RCDEBUG, __rc_rstrextend0)
#define __rcralloc_small0 (RCDEBUG, __rc_ralloc_small0)
#else
#include "libregion/profile.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
