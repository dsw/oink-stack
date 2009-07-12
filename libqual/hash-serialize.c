/* see License.txt for copyright and terms of use */

#include "libqual/hash-serialize.h"
#include "libqual/dd_list.h"

void s18n_hash_table_iterator(s18n_type *htt, void *hashtable, void *iterator, void *arg)
{
  hash_table_scanner **hts = iterator;
  *hts = xmalloc(sizeof(hash_table_scanner));
  hash_table_scan(hashtable, *hts);
}

int s18n_hash_table_next(s18n_type *htt, void *hashtable, void *iterator,
			 void *key, void *value, void *arg)
{
  hash_table_kind *htk = (hash_table_kind *)
    ((s18n_hashtable_info *)htt->private_info)->private_info;
  hash_table_scanner **hts = iterator;

  if (*hts == NULL)
    return 0;

  while (hash_table_next(*hts, key, value))
    if (htk->hp == NULL || htk->hp->f == NULL ||
	htk->hp->f(*(void **)key, *(void **)value, htk->hp->arg1, htk->hp->arg2) == 0)
      return 1;

  free(*hts);
  *hts = NULL;
  return 0;
}

void *s18n_hash_table_constructor(s18n_type *htt, size_t size, void *arg)
{
  hash_table_kind *htk = (hash_table_kind *)
    ((s18n_hashtable_info *)htt->private_info)->private_info;
  return make_hash_table(*htk->rhash, size);
}

void s18n_hash_table_insert(s18n_type *htt, void *hashtable, void *key, void *value, void *arg)
{
  hash_table_kind *htk = (hash_table_kind *)
    ((s18n_hashtable_info *)htt->private_info)->private_info;
  hash_table_insert(hashtable, htk->hash, htk->cmp, key, value);
}

size_t s18n_hash_table_count(s18n_type *htt, void *hashtable)
{
  hash_table ht = hashtable;
  return hash_table_size(ht);
}

s18n_hashtable_ops hash_ops = {
  .iterator = s18n_hash_table_iterator,
  .next = s18n_hash_table_next,
  .constructor = s18n_hash_table_constructor,
  .insert = s18n_hash_table_insert,
  .count = s18n_hash_table_count
};

/* This predicate function is a convenience for hashset */
int hashset_pred_adapter(void *key, void *value, void *arg1, void *arg2)
{
  dd_cmp_fn f = (dd_cmp_fn)arg1;
  return f != NULL && f(key, arg2);

}

