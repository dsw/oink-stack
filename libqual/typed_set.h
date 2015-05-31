/* see License.txt for copyright and terms of use */

#ifndef TYPED_SET_H
#define TYPED_SET_H

#include "libqual/dd_list.h"
#include "libqual/bool.h"
#include "libregion/regions.h"

typedef int (*set_cmp_fn)(void *e1, void *e2);

#if 0
#define DECLARE_SET(name,type) \
name empty_ ## name (region r); \
name name ## _copy(region r, name s); \
bool name ## _empty(name s); \
bool name ## _member(name s, type elt); \
int  name ## _size(name s); \
bool name ## _insert(region r, name *s, type elt); \
bool name ## _insert_last(region r, name *s, type elt); \
bool name ## _insert_nocheck(region r, name *s, type elt); \
bool name ## _insert_last_nocheck(region r, name *s, type elt); \
void name ## _remove(name *s, type elt); \
name name ## _union(name s1, name s2); \
name name ## _union_nocheck(name s1, name s2); \
bool name ## _single(name s); \
void name ## _sort(name s); \
void name ## _remove_dups(int (*cmp)(type, type), name s); \
void name ## _scan(name s, name ## _scanner *ss); \
type name ## _next(name ## _scanner *ss);
#endif

#define DEFINE_SET(name,type,cmpfn) \
typedef struct dd_list_struct name ## _ST; \
typedef struct dd_list_pos_struct name ## _ST_scanner; \
typedef name ## _ST *name; \
typedef name ## _ST_scanner *name ## _scanner; \
static inline name empty_ ## name (region r) \
{ \
    return dd_new_list(r); \
} \
static inline name name ## _copy(region r, name s) \
{ \
    if (s == NULL) \
      return NULL; \
    else \
      return dd_copy(r, s); \
} \
static inline bool name ## _empty(name s) \
{ \
  return s == NULL || dd_is_empty(s); \
} \
static inline bool name ## _member(name s, type elt) \
{ \
    return s != NULL && dd_search(s, (dd_cmp_fn) cmpfn, (void *) elt) != NULL; \
} \
static inline long  name ## _size(name s) \
{ \
    if (s == NULL) \
      return 0; \
    else \
      return dd_length(s); \
} \
static inline bool name ## _insert(region r, name *s, type elt) \
{ \
    *s = dd_fix_null(r, *s); \
    if (! name ## _member(*s, elt)) \
      { \
	dd_add_first(r, *s, (void *) elt); \
        return TRUE; \
      } \
    return FALSE; \
} \
static inline bool name ## _insert_last(region r, name *s, type elt) \
{ \
    *s = dd_fix_null(r, *s); \
    if (! name ## _member(*s, elt)) \
      { \
	  dd_add_last(r, *s, (void *) elt); \
          return TRUE; \
      } \
    return FALSE; \
} \
static inline bool name ## _insert_nocheck(region r, name *s, type elt) \
{ \
    *s = dd_fix_null(r, *s); \
    dd_add_first(r, *s, (void *) elt); \
    return TRUE; \
} \
static inline bool name ## _insert_last_nocheck(region r, name *s, type elt) \
{ \
    *s = dd_fix_null(r, *s); \
    dd_add_last(r, *s, (void *) elt); \
    return TRUE; \
} \
static inline void name ## _remove(name *s, type elt) \
{ \
    if (*s) \
      dd_remove_all_matches_from(dd_first(*s), (dd_cmp_fn) cmpfn, (void *) elt); \
} \
static inline name name ## _union(name s1, name s2) \
{ \
    if (s1 == NULL) \
      return s2; \
    else if (s2 == NULL) \
      return s1; \
    dd_append(s1, s2); \
    dd_remove_dups(s1, (dd_cmp_fn)cmpfn); \
    return s1; \
} \
static inline name name ## _union_nocheck(name s1, name s2) \
{ \
    if (s1 == NULL) \
      return s2; \
    else if (s2 == NULL) \
      return s1; \
    dd_append(s1, s2); \
    return s1; \
} \
static inline bool name ## _single(name s) \
{ \
    return name ## _size(s) == 1; \
} \
static inline void name ## _sort(name s) \
{ \
    if (s) \
      dd_sort(s, (set_cmp_fn) cmpfn); \
} \
static inline void name ## _sort_custom(name s, set_cmp_fn f) \
{ \
    if (s) \
      dd_sort(s, f); \
} \
static inline void name ## _remove_dups(int (*cmp)(type, type), name s) \
{ \
    if (s) \
      dd_remove_dups(s, (dd_cmp_fn)cmp); \
} \
static inline size_t name ## _remove_matches(name s, int (*pred)(type, void *), void *arg) \
{ \
    if (s) \
      return dd_remove_all_matches_from(dd_first(s), (dd_cmp_fn)pred, arg);    \
    else                                                                       \
      return 0;                                                                \
} \
static inline void name ## _scan(name s, name ## _scanner *ss) \
{ \
    if (s) \
      *ss = dd_first(s); \
    else *ss = NULL; \
} \
static inline type name ## _next(name ## _scanner *ss) \
{ \
    type result; \
    if (*ss == NULL || dd_is_end(*ss)) \
      return NULL; \
    result = DD_GET(type, *ss); \
    *ss = dd_next(*ss); \
    return result; \
} \

#define DEFINE_SET_S18N(tname, s18nid, etype, region, pred_fn, pred_arg) \
DEFINE_LIST_S18N(tname, s18nid, etype, region, pred_fn, pred_arg)

#endif
