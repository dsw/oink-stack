// see License.txt for copyright and terms of use

#include "libqual/hashset-serialize.h"
#include "libqual/dd_list.h"
#include <assert.h>

void s18n_hashset_table_iterator(s18n_type *htt, void *hashtable, void *iterator, void *arg)
{
  hashset_table_scanner **hts = iterator;
  *hts = xmalloc(sizeof(hashset_table_scanner));
  hashset_table_scan(hashtable, *hts);
}

int s18n_hashset_table_next(s18n_type *htt, void *hashtable, void *iterator, void *key,
                            void *value, void *arg)
{
  assert(value == NULL);

  // hashset_table_kind *htk = (hashset_table_kind *)
  //   ((s18n_hashtable_info *)htt->private_info)->private_info;
  hashset_table_scanner **hts = iterator;

  if (*hts == NULL)
    return 0;

  if (hashset_table_next(*hts, key))
    return 1;

  free(*hts);
  *hts = NULL;
  return 0;
}

void *s18n_hashset_table_constructor(s18n_type *htt, size_t size, void *arg)
{
  hashset_table_kind *htk = (hashset_table_kind *)
    ((s18n_hashtable_info *)htt->private_info)->private_info;
  return make_hashset_table(*htk->rhash, size /*, htk->hash, htk->cmp */);
}

// this function takes key/value args so we can reuse the htkvi closures
void s18n_hashset_table_insert(s18n_type *htt, void *hashset_table, void *key, void *value, void *arg)
{
  hashset_table_kind *htk = (hashset_table_kind *)
    ((s18n_hashtable_info *)htt->private_info)->private_info;
  assert(value == NULL);
  assert(arg == NULL);
  hashset_table_insert(hashset_table, htk->hash, htk->cmp, key);
}

size_t s18n_hashset_table_count(s18n_type *htt, void *h)
{
  hashset_table ht = h;
  return hashset_table_size(ht);
}

s18n_hashtable_ops hashset_ops = {
  .iterator = s18n_hashset_table_iterator,
  .next = s18n_hashset_table_next,
  .constructor = s18n_hashset_table_constructor,
  .insert = s18n_hashset_table_insert,
  .count = s18n_hashset_table_count
};
