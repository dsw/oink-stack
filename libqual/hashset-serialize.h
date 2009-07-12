// see License.txt for copyright and terms of use

#ifndef HASHSET_SERIALIZE_H
#define HASHSET_SERIALIZE_H

/****************
 * Interface between hash.h and serialize.h
 ****************/

#include "libqual/hashset.h"
#include "libqual/serialize.h"

/* Most clients don't need these: just use the standard
   s18n_hashtable_ops hashset_ops.  These are just available in case you
   need to overload some of them. */
void s18n_hashset_table_iterator(s18n_type *htt, void *hashtable,
                                 void *iterator, void *arg);
int s18n_hashset_table_next(s18n_type *htt, void *hashtable, void *iterator,
                            void *key, void *value, void *arg);
void *s18n_hashset_table_constructor(s18n_type *htt, size_t size, void *arg);
void s18n_hashset_table_insert(s18n_type *htt, void *hashtable,
                               void *key, void *value, void *arg);

/* The standard operations on hashset.h hashtables. */
extern s18n_hashtable_ops hashset_ops;

/* Describes a specialization of hashset_table */
typedef struct hashset_table_kind_t
{
  region *rhash;
  hash_fn hash;
  keyeq_fn cmp;
} hashset_table_kind;

#endif
