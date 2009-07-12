/* see License.txt for copyright and terms of use */

#ifndef TYPED_MAP_H
#define TYPED_MAP_H

#include "libqual/hash.h"
#include "libqual/bool.h"
#include "libqual/hash-serialize.h"
#include "libregion/regions.h"
#include "libregion/cqual-stdint.h"

#if 0
#define DECLARE_MAP(name,domain_type,range_type) \
static inline name make_ ## name(region r, unsigned long size); \
static inline void name ## _reset(name m); \
static inline unsigned long name ## _size(name m); \
static inline bool name ## _lookup(name m, domain_type k, range_type *d); \
static inline bool name ## _insert(name m, domain_type k, range_type d); \
static inline bool name ## _remove(name m, domain_type k); \
static inline name name ## _copy(region r, name m); \
static inline name name ## _map(region r, name m, \
                                range_type (*f)(domain_type, range_type, void *), \
                                void *arg); \
static inline void name ## _scan(name m, name ## _scanner *ms); \
static inline bool name ## _next(name ## _scanner *ms, \
                                 domain_type *k, range_type *d); \
static inline void name ## _scan_sorted(name m, int (*f)(domain_type, domain_type), \
                                        name ## _scanner_sorted *mss); \
static inline bool name ## _next_sorted(name ## _scanner_sorted *ms, domain_type *k, \
                                        range_type *d);
#endif

#define DEFINE_MAP(name,domain_type,range_type,hash_func,keyeq_func)           \
  unsigned long num_##name##s_created = 0;                                     \
  typedef struct name##_t *name;                                               \
  typedef struct { hash_table_scanner hts; } name ## _scanner;                 \
  typedef struct { hash_table_scanner_sorted htss; } name ## _scanner_sorted;  \
  static inline name make_ ## name(region r, unsigned long size)               \
  { ++num_##name##s_created; return (name) make_hash_table(r, size); }         \
  static inline void name ## _reset(name m)                                    \
  { hash_table_reset((hash_table) m); }                                        \
  static inline unsigned long name ## _size(name m)                            \
  { return hash_table_size((hash_table) m); }                                  \
  static inline bool name ## _lookup(name m, domain_type k, range_type *d)     \
  { return hash_table_hash_search((hash_table) m, (hash_fn) hash_func, (keyeq_fn) keyeq_func, (hash_key) k, (hash_data *) d); } \
  static inline bool name ## _insert(name m, domain_type k, range_type d)      \
  { return hash_table_insert((hash_table) m, (hash_fn) hash_func, (keyeq_fn) keyeq_func, (hash_key) k, (hash_data) (intptr_t) d); } \
  static inline bool name ## _remove(name m, domain_type k)                    \
  { return hash_table_remove((hash_table) m, (hash_fn) hash_func, (keyeq_fn) keyeq_func, (hash_key) k); }                  \
  static inline name name ## _copy(region r, name m)                           \
  { return (name) hash_table_copy(r, (hash_table) m); }                        \
  static inline name name ## _map(region r, name m, range_type (*f)(domain_type, range_type, void *), void *arg) \
  { return (name) hash_table_map(r, (hash_table) m, (hash_map_fn) f, arg); }   \
  static inline void name ## _scan(name m, name ## _scanner *ms)               \
  { hash_table_scan((hash_table) m, &ms->hts); }                               \
  static inline bool name ## _next(name ## _scanner *ms, domain_type *k, range_type *d) \
  { return hash_table_next(&ms->hts, (hash_key *) k, (hash_data *) d); }       \
  static inline void name ## _scan_sorted(name m, int (*f)(domain_type, domain_type), name ## _scanner_sorted *mss) \
  { hash_table_scan_sorted((hash_table) m, (keycmp_fn) f, &mss->htss); }       \
  static inline bool name ## _next_sorted(name ## _scanner_sorted *ms, domain_type *k, range_type *d) \
  { return hash_table_next_sorted(&ms->htss, (hash_key *) k, (hash_data *) d); } \
  static inline void destroy_##name(name m)                                    \
  { destroy_hash_table((hash_table) m); }

#define DEFINE_MAP_S18N(__tname, __s18nid, __ktype, __vtype, __region, __hash, __eq, __pred, __arg1, __arg2) \
s18n_type __tname = { \
  .name = __s18nid, \
  .serialize = s18n_hashtable_serialize, \
  .deserialize = s18n_hashtable_deserialize, \
  .register_children = s18n_hashtable_register_children, \
  .private_info = &(s18n_hashtable_info) { \
    .ops = &hash_ops, \
    .key_type = &__ktype, \
    .value_type = &__vtype, \
    .private_info = &(hash_table_kind) { \
      .rhash = &__region, \
      .hash = (hash_fn)__hash, \
      .cmp = (keyeq_fn)__eq, \
      .hp = &(hash_predicate) { \
	.f = (hash_pred_fn)__pred, \
	.arg1 = (void *)__arg1, \
	.arg2 = __arg2 \
      } \
    } \
  } \
}

#endif
