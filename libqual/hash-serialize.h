/* see License.txt for copyright and terms of use */

#ifndef HASH_SERIALIZE_H
#define HASH_SERIALIZE_H

/****************
 * Interface between hash.h and serialize.h
 ****************/

#include "libqual/hash.h"
#include "libqual/serialize.h"

/* Most clients don't need these: just use the standard
   s18n_hashtable_ops hash_ops.  These are just available in case you
   need to overload some of them. */
void s18n_hash_table_iterator(s18n_type *htt, void *hashtable,
			      void *iterator, void *arg);
int s18n_hash_table_next(s18n_type *htt, void *hashtable, void *iterator,
			 void *key, void *value, void *arg);
void *s18n_hash_table_constructor(s18n_type *htt, size_t size, void *arg);
void s18n_hash_table_insert(s18n_type *htt, void *hashtable,
			    void *key, void *value, void *arg);

/* The standard operations on hash.h hashtables. */
extern s18n_hashtable_ops hash_ops;

/* Predicates on hashtables */
typedef int (*hash_pred_fn)(void *key, void *value, void *arg1, void *arg2);
typedef struct hash_predicate_t
{
  hash_pred_fn f;
  void *arg1;
  void *arg2;
} hash_predicate;

/* This predicate function is a convenience for hashset */
int hashset_pred_adapter(void *key, void *value, void *arg1, void *arg2);

/* Describes a specialization of hash_table */
typedef struct hash_table_kind_t
{
  region *rhash;
  hash_fn hash;
  keyeq_fn cmp;
  hash_predicate *hp;
} hash_table_kind;

#endif
