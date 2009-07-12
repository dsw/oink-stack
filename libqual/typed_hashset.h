/* see License.txt for copyright and terms of use */

#ifndef TYPED_HASHSET_H
#define TYPED_HASHSET_H

// #include "libqual/hash.h"
#include "libqual/hashset.h"
#include "libqual/hashset-serialize.h"
#include "libqual/bool.h"
#include "libregion/regions.h"

#if 0
#define DECLARE_HASHSET(name,element_type) \
static bool name ## _eqfn(element_type l, element_type r); \
static inline name empty_ ## name(region r); \
static inline name name ## _copy(region r, name s); \
static inline bool name ## _empty(name s); \
static inline bool name ## _member(name s, element_type elt); \
static inline bool name ## _hash_search(name s, \
                                        bool (*eqfn)(element_type, element_type), \
                                        element_type k, element_type *d); \
static inline unsigned long name ## _size(name s); \
static inline bool name ## _insert(name *s, element_type elt); \
static inline void name ## _remove(name *s, element_type elt); \
static inline bool name ## _single(name s); \
static inline void name ## _scan(name s, name ## _scanner *ss); \
static inline element_type name ## _next(name ## _scanner *ss); \
static inline name name ## _union(name s1, name s2);
#endif

// quarl 2006-07-10
//    Since there are *tons* of hash tables with 1-2 elements, it's worth
//    starting with a tiny table.  Note that at size<8, hashset_table is
//    really an array that grows linearly.

#define DEFINE_HASHSET(name,cmp_fn,hashset_func,element_type)                  \
  unsigned long num_##name##s_created = 0;                                     \
  typedef struct name##_t *name;                                               \
  typedef struct { hashset_table_scanner hts; } name ## _scanner;              \
  static inline bool name ## _eqfn(element_type l, element_type r)             \
  { return cmp_fn(l,r) == 0; }                                                 \
  static inline name empty_ ## name(region r)                                  \
  { ++num_##name##s_created; return (name) make_hashset_table(r, 0); }         \
  static inline name name ## _copy(region r, name s)                           \
  { return (name) hashset_table_copy(r, (hashset_table) s, (hash_fn) hashset_func, (keyeq_fn) name ## _eqfn); } \
  static inline bool name ## _empty(name s)                                    \
  { return hashset_table_size((hashset_table) s) == 0; }                       \
  static inline void name##_reset(name s)                                      \
  { hashset_table_reset((hashset_table) s); }                                  \
  static inline bool name ## _member(name s, element_type elt)                 \
  { return hashset_table_hash_search((hashset_table) s, (hash_fn) hashset_func, (keyeq_fn) name ## _eqfn, (hash_key) elt, NULL); } \
  static inline bool name ## _hash_search(name s, bool (*eqfn)(element_type, element_type), element_type k, element_type *d) \
  { return hashset_table_hash_search((hashset_table)s, (hash_fn) hashset_func, (keyeq_fn) eqfn, (hash_key)k, (hash_key*)d); } \
  static inline unsigned long name ## _size(name s)                            \
  { return hashset_table_size((hashset_table) s); }                            \
  static inline bool name ## _insert(name *s, element_type elt)                \
  { return hashset_table_insert((hashset_table) *s, (hash_fn) hashset_func, (keyeq_fn) name ## _eqfn, (hash_key) elt); } \
  static inline bool name ## _remove(name *s, element_type elt)                \
  { return hashset_table_remove((hashset_table) *s, (hash_fn) hashset_func, (keyeq_fn) name ## _eqfn, (hash_key) elt); } \
  static inline void name ## _update_hashes(name s)                            \
  { hashset_table_update_hashes((hashset_table)s, (hash_fn) hashset_func, (keyeq_fn) name ## _eqfn); } \
  static inline bool name ## _single(name s)                                   \
  { return name ## _size(s) == 1; }                                            \
  static inline void name ## _scan(name s, name ## _scanner *ss)               \
  { hashset_table_scan((hashset_table) s, &ss->hts); }                         \
  static inline element_type name ## _next(name ## _scanner *ss)               \
  { element_type result; if (hashset_table_next(&ss->hts, (hash_data *) &result)) return result; return NULL; } \
  static inline name name ## _union(name s1, name s2)                          \
  { name ## _scanner ss; element_type elt; name ## _scan (s2, &ss); while ((elt = name ## _next (&ss))) name ## _insert (&s1, elt); return s1; } \
  static inline void destroy_##name(name s)                                    \
  { destroy_hashset_table((hashset_table) s); }

// static inline name name ## _union_new(region r, name s1, name s2)
//   { name ht = empty_##name(r); name##_union(ht, s1); name##_union(ht, s2); return ht; }

#define DEFINE_HASHSET_S18N(__name, __tname, __s18nid, __etype, __region, __hash) \
s18n_type __tname = { \
  .name = __s18nid, \
  .serialize = s18n_hashset_table_serialize, \
  .deserialize = s18n_hashset_table_deserialize, \
  .register_children = s18n_hashset_table_register_children, \
  .private_info = &(s18n_hashtable_info) { \
    .ops = &hashset_ops, \
    .key_type = &__etype, \
    .value_type = NULL, \
    .private_info = &(hashset_table_kind) { \
      .rhash = &__region, \
      .hash = (hash_fn)__hash, \
      .cmp = (keyeq_fn)__name ## _eqfn, \
    } \
  } \
}

#endif
