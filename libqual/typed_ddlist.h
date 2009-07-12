/* see License.txt for copyright and terms of use */

#ifndef TYPED_DDLIST_H
#define TYPED_DDLIST_H

#include "libqual/dd_list.h"
#include "libqual/bool.h"
#include "libqual/dd_list-serialize.h"
#include "libregion/regions.h"

#if 0
#define DECLARE_LIST(name,type) \
typedef dd_list name; \
typedef dd_list_pos name ## _scanner; \
typedef void (* name ## _app_fn) (type); \
typedef bool (* name ## _eq_fn) (const type); \
typedef int (* name ## _comparator_fn)(const type,const type); \
name new_ ## name(region r); \
int name ## _length(name a); \
name name ## _cons(region r, name a, type data); \
name name ## _append(name a, name b); \
name name ## _append_elt(region r, name a, type data); \
name name ## _app(name a, name ## _app_fn app); \
type name ## _find(name a, name ## _eq_fn eq); \
type name ## _head(name a); \
type name ## _get_tail(name a); \
name name ## _tail(name a); \
name name ## _front(name a); \
name name ## _copy(region r, name a); \
void name ## _scan(name a, name ##_scanner *scan); \
bool name ## _next(name ##_scanner *scan, type *data); \
bool name ## _empty(name a); \
bool name ## _member(name a, type data);
#endif

#define DEFINE_LIST(name,type) \
typedef dd_list name; \
typedef dd_list_pos name ## _scanner; \
typedef void (* name ## _app_fn) (type, void*); \
typedef bool (* name ## _eq_fn) (const type); \
typedef int (* name ## _comparator_fn)(const type,const type); \
static inline name new_ ## name(region r) \
{ \
 return (name)dd_new_list(r);  \
} \
static inline int name ## _length(name a) \
{ \
 return dd_length((dd_list)a); \
} \
static inline name name ## _cons(region r, name a, type data) \
{ \
 dd_add_first(r, (dd_list)a, (void*)data); \
 return a; \
}\
static inline name name ## _append(name a, name b) \
{ \
 dd_append((dd_list)a, (dd_list)b); \
 return a; \
} \
static inline name name ## _append_elt(region r, name a, type b) \
{ \
 dd_add_last (r, (dd_list)a, (void*)b); \
 return a; \
} \
static inline name name ## _app(name a, name ## _app_fn app, void *arg) \
{ \
 dd_list_pos elt; \
 dd_scan (elt, (dd_list)a) \
   { \
     app (DD_GET(type, elt), arg); \
   } \
 return a; \
} \
static inline name name ## _rev_app(name a, name ## _app_fn app, void *arg) \
{ \
 dd_list_pos elt; \
 dd_scan_reverse (elt, (dd_list)a) \
   { \
     app (DD_GET(type, elt), arg); \
   } \
 return a; \
} \
static inline type name ## _find(name a, name ## _eq_fn eq) \
{ \
 dd_list_pos elt; \
 dd_scan (elt, (dd_list)a) \
   { \
     if (eq (DD_GET(type, elt))) \
       return DD_GET(type, elt); \
   } \
 return NULL; \
} \
static inline void name ## _sort(name a, name ## _comparator_fn f) \
{\
 dd_sort ((dd_list)a, (dd_cmp_fn)f); \
}\
static inline name name ## _tail(name a) \
{\
 dd_remove (dd_first((dd_list)a)); \
 return a; \
}\
static inline name name ## _front(name a) \
{\
 dd_remove (dd_last((dd_list)a)); \
 return a; \
}\
static inline type name ## _head(name a) \
{ \
 return DD_GET(type, dd_first((dd_list)a)); \
} \
static inline type name ## _get_tail(name a) \
{ \
 return DD_GET(type, dd_last((dd_list)a)); \
} \
static inline name name ## _copy(region r, name a) \
{ \
 return (name)dd_copy(r,(dd_list) a); \
} \
static inline void name ## _scan(name a, name ##_scanner *scan) \
{ \
 *scan = dd_first ((dd_list)a); \
}\
static inline bool name ## _next(name ##_scanner *scan, type *data) \
{ \
 if (!dd_is_end ((dd_list_pos)*scan)) \
   { \
     *data = DD_GET (type, (dd_list_pos)*scan); \
     *scan = dd_next((dd_list_pos)*scan); \
     return TRUE; \
   } \
 return FALSE; \
} \
static inline bool name ## _empty(name a) \
{ \
 return dd_is_empty((dd_list)a); \
} \
static inline bool name ## _member(name a, type data) \
{ \
 dd_list_pos elt; \
 dd_scan (elt, (dd_list)a) \
   { \
     if (data == DD_GET(type, (dd_list_pos)a)) \
       return TRUE; \
   } \
 return FALSE; \
} \

#define DEFINE_LIST_S18N(tname, s18nid, etype, region, pred_fn, pred_arg) \
s18n_type tname = { \
  .name = s18nid, \
  .serialize = s18n_container_serialize, \
  .deserialize = s18n_container_deserialize, \
  .register_children = s18n_container_register_children, \
  .private_info = &(s18n_container_info) { \
    .ops = &dd_list_ops, \
    .element_type = &etype, \
    .private_info = &(dd_list_kind) { \
      .r = &region, \
      .ddlp = &(dd_list_predicate) { \
	.f = (dd_cmp_fn)pred_fn, \
	.arg = (void *)pred_arg \
      } \
    } \
  } \
}

#endif /* TYPED_DDLIST_H */
