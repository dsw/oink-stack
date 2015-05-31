/* see License.txt for copyright and terms of use */

#ifndef TYPED_BAG_H
#define TYPED_BAG_H

#include "libqual/dd_list.h"
#include "libqual/bool.h"
#include "libqual/typed_set.h"
#include "libqual/typed_ddlist.h"
#include "libregion/regions.h"

#if 0
#define DECLARE_BAG(name, element_type) \
static inline name empty_ ## name(region r); \
static inline name name ## _copy(region r, name s); \
static inline bool name ## _empty(name s); \
static inline bool name ## _member(int (*cmp)(element_type, element_type), name s, element_type elt); \
static inline int  name ## _size(name s); \
static inline bool name ## _insert(region r, name *s, element_type elt); \
static inline bool name ## _insert_last(region r, name *s, element_type elt); \
static inline name name ## _union(name s1, name s2); \
static inline bool name ## _single(name s); \
static inline void name ## _sort(int (*cmp)(element_type, element_type), name s); \
static inline void name ## _remove_dups(int (*cmp)(element_type, element_type), \
                                        name s); \
static inline void name ## _scan(name s, name ## _scanner *ss); \
static inline element_type name ## _next(name ## _scanner *ss);
#endif

#define DEFINE_BAG(name, element_type) \
typedef dd_list name; \
typedef dd_list_pos name ## _scanner; \
static inline name empty_ ## name(region r) { return dd_new_list(r); } \
static inline name name ## _copy(region r, name s) { if (s == NULL) return NULL; return dd_copy(r, s); } \
static inline bool name ## _empty(name s) { return s == NULL || dd_is_empty(s); } \
static inline bool name ## _member(int (*cmp)(element_type, element_type), name s, element_type elt) { return s != NULL && dd_search(s, (dd_cmp_fn) cmp, (void *) elt) != NULL; } \
static inline long name ## _size(name s) { if (s == NULL) return 0; return dd_length(s); } \
static inline bool name ## _insert(region r, name *s, element_type elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return TRUE; } \
static inline bool name ## _insert_last(region r, name *s, element_type elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return TRUE; } \
static inline name name ## _union(name s1, name s2) { if (s1 == NULL) return s2; else if (s2 == NULL) return s1; dd_append(s1, s2); return s1; } \
static inline bool name ## _single(name s) { return name ## _size(s) == 1; } \
static inline void name ## _sort(int (*cmp)(element_type, element_type), name s) { if (s == NULL) return; dd_sort(s, (set_cmp_fn) cmp); } \
static inline void name ## _remove_dups(int (*cmp)(element_type, element_type), name s) { if (s == NULL) return; dd_remove_dups(s, (dd_cmp_fn)cmp); } \
static inline void name ## _scan(name s, name ## _scanner *ss) { if (s == NULL) *ss = NULL; else *ss = dd_first(s); } \
static inline element_type name ## _next(name ## _scanner *ss) { element_type result; if (*ss == NULL || dd_is_end(*ss)) return NULL; result = DD_GET(element_type, *ss); *ss = dd_next(*ss); return result; }

#define DEFINE_BAG_S18N(tname, s18nid, etype, region, pred_fn, pred_arg) \
DEFINE_LIST_S18N(tname, s18nid, etype, region, pred_fn, pred_arg)

#endif
