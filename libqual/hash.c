/* see License.txt for copyright and terms of use */

#include "libqual/hash.h"
#include "libqual/utils.h"
#include "libregion/regions.h"
#include "libregion/miniregion.h"
#include <assert.h>
#include <string.h>

// There are two modes of operation: Owner mode and NonOwner mode.
//
// In Owner mode, the hash table structure is allocated with malloc(), and it
// contains its own region.  Call destroy_hash_table() to deallocate it.
//
// In NonOwner, everything is allocated in the region that is passed in.  Call
// deleteregion() as usual to deallocate.
//
// The advantage of Owner mode is that space can be reclaimed, e.g. when
// rehashing and resetting; and also the hash table can be deallocated
// individually (you could simulate this last part by creating a region just
// for this hash table; but you would still leak memory when rehashing).
//
// The advantage of NonOwner mode is when creating some temporary hash tables
// you can deallocate everything in one deleteregion call.
//
// To create an Owner hash_table, pass NULL as the region.


// Only bother reclaiming space if there's a lot to reclaim.
// For pages of size 8196, no point reclaiming much less than that.
//
// In general a table occupies size*4 bytes for the table and used*12 bytes
// for buckets, but it may have accumulated previously non-deallocated
// elements.
static const int RECLAIM_SPACE_THRESHOLD = 256;

// TODO: hash_sets shouldn't waste space in Buckets to duplicate key/data.

struct Bucket
{
  hash_key key;
  hash_data data;
  struct Bucket *next;
};

#define scan_bucket(b, var) for (var = b; var; var = var->next)

#define HASH_TABLE_MAXSIZE ((1<<26)-1)

struct Hash_table
{
  union {
    region r;         		/* Region for this table */
    struct miniregion mp[1];       // miniregion; takes 1 word
  };
  // hash_fn hash;     		/* Function for hashing keys */
  // keyeq_fn cmp;     		/* Function for comparing keys */

  // unsigned long size;		/* Number of buckets */
  unsigned long used : 26;		/* Number of elements */
  unsigned long log2size : 5;           /* log2 of size */
  bool owner : 1;
  bucket *table;		/* Array of (size) buckets */
};

HASH_FUNC_INLINE
void rehash(hash_table ht, hash_fn hash, keyeq_fn cmp);

/* Force size to a power of 2 */
static inline unsigned long init_size_pow2(hash_table ht, unsigned long input_size)
{
  unsigned long size = 1;
  unsigned long log2size = 0;
  while (size < input_size) {
    size *= 2;
    log2size++;
  }
  // ht->size = size;
  ht->log2size = log2size;
  return size;
}

// return capacity of hashset_table (previously this was ht->size)
static inline unsigned long hash_table_capacity(hash_table ht)
{
  return 1 << ht->log2size;
}

// Make a new hash table, with SIZE buckets initially.
//
// The Hash table structure is allocated with malloc() and elements are
// allocated in a region.  Must call destroy_hash_table() to deallocate it.
HASH_FUNC_INLINE
hash_table make_hash_table_owner(unsigned long size
                                 /*, hash_fn hash, keyeq_fn cmp*/)
{
  hash_table ht;

  ht = xmalloc(sizeof(struct Hash_table));
  ht->owner = 1;
  // ht->r = newregion();
  miniregion_init(ht->mp);
  // ht->hash = hash;
  // ht->cmp = cmp;
  size = init_size_pow2(ht, size);
  ht->used = 0;
  // ht->table = rarrayalloc(ht->r, ht->size, bucket);
  // ht->table = miniregion_alloc(ht->mp, ht->size * sizeof(bucket));
  ht->table = xcalloc(size, sizeof(bucket));

  return ht;
}

// Make a new hash table, with SIZE buckets initially.
// Hash table and its elements are allocated in region r.
HASH_FUNC_INLINE
hash_table make_hash_table(region r, unsigned long size
                           /*, hash_fn hash, keyeq_fn cmp*/)
{
  if (!r) return make_hash_table_owner(size);

  hash_table ht;

  ht = ralloc(r, struct Hash_table);
  ht->owner = 0;
  ht->r = r;
  // ht->hash = hash;
  // ht->cmp = cmp;
  size = init_size_pow2(ht, size);
  ht->used = 0;
  ht->table = rarrayalloc(ht->r, size, bucket);

  return ht;
}

// /* Make a hash table for strings. */
// HASH_FUNC_INLINE
// hash_table make_string_hash_table(region rhash, unsigned long size)
// {
//   return make_hash_table(rhash, size, (hash_fn) string_hash,
// 			 (keyeq_fn) string_eq);
// }

// This destroys the contents of the hash table; must have been created with
// make_hash_table_owner().
HASH_FUNC_INLINE
void destroy_hash_table(hash_table ht)
{
  assert(ht->owner);
  // deleteregion(ht->r);
  free(ht->table);
  miniregion_delete(ht->mp);
  free(ht);
}

/* Zero out ht. */
HASH_FUNC_INLINE
void hash_table_reset(hash_table ht)
{
  if (ht->owner) {
    // if (ht->size > RECLAIM_SPACE_THRESHOLD) {
    //   deleteregion(ht->r);
    //   ht->r = newregion();
    // }
    miniregion_reset(ht->mp);
    // init_size_pow2(ht, 8);
    // free(ht->table);
    // ht->table = rarrayalloc(ht->r, ht->size, bucket);
    // ht->table = miniregion_alloc(ht->mp, ht->size * sizeof(bucket));
    // ht->table = xcalloc(ht->size, sizeof(bucket));
  }

  memset(ht->table, 0, sizeof(ht->table[0]) * hash_table_capacity(ht));
  ht->used = 0;
}

/* Return the number of entries in ht */
HASH_FUNC_INLINE
unsigned long hash_table_size(hash_table ht)
{
  return ht->used;
}

#define MAGIC 2*0.6180339987

#define LLMAGIC ((unsigned long long)(MAGIC * (1ULL << (8 * sizeof(unsigned long) - 1))))

/* Return the bucket corresponding to k in ht */
static inline bucket *find_bucket(hash_table ht, hash_fn hash,
                                  hash_key k)
{
  unsigned long hashVal;

  if (hash_table_capacity(ht) == 1) {
    hashVal = 0;
  } else {
    hashVal = hash(k);
    hashVal = hashVal * LLMAGIC;
    hashVal = hashVal >> (8 * sizeof(unsigned long) - ht->log2size);
    /* hashVal = hashVal % ht->size; */
  }
  assert(hashVal < hash_table_capacity(ht));
  return &ht->table[hashVal];
}

/* Given a comparison function which agrees with our hash_function,
   search for the given element. */
HASH_FUNC_INLINE
bool hash_table_hash_search(hash_table ht,
                            hash_fn hash, keyeq_fn cmp,
                            hash_key k, hash_data *d)
{
  bucket cur;

  assert(ht);
  cur = *find_bucket(ht, hash, k);
  while (cur)
    {
      if (cmp(k, cur->key))
	{
          // assert(ht->hash(k) == ht->hash(cur->key));
	  if (d)
	    *d = cur->data;
	  return TRUE;
	}
      cur = cur->next;
    }
  return FALSE;
}

// following is obsolete since we now separate hashsets.

// // check that all keys are equal to data, as required for hash set
// HASH_FUNC_INLINE
// void hash_table_really_check_hashset(hash_table ht)
// {
//   assert(ht);

//   int i;
//   for (i = 0; i < ht->size; i++) {
//     bucket cur = ht->table[i];

//     while (cur)
//     {
//       if (cur->key != cur->data) {
//         fprintf(stderr, "Failed hash_table_really_check_hashset (12d2b0ab-a40f-4345-b298-937b3ed88c87)");
//         abort();
//       }
//       cur = cur->next;
//     }
//   }
// }

/* brute-force search entire table (for debugging purposes) */
HASH_FUNC_INLINE
bool hash_table_hash_search_hard(hash_table ht, hash_fn hash, keyeq_fn cmp,
                                 hash_key k, hash_data *d)
{
  assert(ht);

  int i;
  for (i = 0; i < hash_table_capacity(ht); i++) {
    bucket cur = ht->table[i];

    while (cur)
    {
      if (cmp(k, cur->key))
      {
        // assert(ht->hash(k) == ht->hash(cur->key));
        if (d)
          *d = cur->data;
        return TRUE;
      }
      cur = cur->next;
    }
  }
  return FALSE;
}

// /* Lookup k in ht.  Returns corresponding data in *d, and function
//    result is TRUE if the k was in ht, false otherwise. */
// HASH_FUNC_INLINE
// bool hash_table_lookup(hash_table ht, hash_key k, hash_data *d)
// {
//   return hash_table_hash_search(ht, ht->cmp, k, d);
// }

// Note: ralloc() call should NOT have a sizeof() around the struct being
// allocated!
#define hash_table_alloc_bucket(ht)                               \
  (ht->owner                                                      \
   ? miniregion_alloc(ht->mp, sizeof(struct Bucket))              \
   : ralloc(ht->r, struct Bucket))

/* Add k:d to ht.  If k was already in ht, replace old entry by k:d.
   Rehash if necessary.  Returns TRUE if k was not already in ht. */
HASH_FUNC_INLINE
bool hash_table_insert(hash_table ht,
                       hash_fn hash, keyeq_fn cmp,
                       hash_key k, hash_data d)
{
  bucket *cur;

  if (ht->used > 3 * hash_table_capacity(ht) / 4)
    rehash(ht, hash, cmp);
  if (ht->used >= HASH_TABLE_MAXSIZE) {
    abort();
  }
  cur = find_bucket(ht, hash, k);
  while (*cur)
    {
      if (cmp(k, (*cur)->key))
	{
          // assert(ht->hash(k) == ht->hash((*cur)->key));

          // Update the data.
          // quarl 2006-05-28: Also, we *must* update the key with the new key
          // because if this is a hash set, then the invariant is that
          // key==data.  Since keys are potentially mutable, not updating will
          // cause inconsistencies later.
          (*cur)->key = k;
	  (*cur)->data = d;
	  return FALSE; /* Replace */
	}
      cur = &(*cur)->next;
    }
  *cur = hash_table_alloc_bucket(ht);
  (*cur)->key = k;
  (*cur)->data = d;
  (*cur)->next = NULL;
  ht->used++;

  // We can't run hash table integrity check because the caller may be
  // modifying the hash values and planning to call hash_table_update_hashes()
  // later.
// #ifdef DEBUG
//   if (!in_DEBUG_CHECK_HASH_TABLE)
//     DEBUG_CHECK_HASH_TABLE(ht);
// #endif

  return TRUE; /* New key */
}

/* Remove mapping for k in ht.  Returns TRUE if k was in ht. */
HASH_FUNC_INLINE
bool hash_table_remove(hash_table ht, hash_fn hash, keyeq_fn cmp, hash_key k)
{
  bucket *cur;
  bucket prev = NULL;

  cur = find_bucket(ht, hash, k);
  while (*cur)
    {
      if (cmp(k, (*cur)->key))
	{
	  if (prev)
	    prev->next = (*cur)->next;
	  else
	    *cur = (*cur)->next;
	  ht->used--;
	  return TRUE;
	}
      prev = *cur;
      cur = &(*cur)->next;
    }
  return FALSE;
}

/* Return a copy of ht */
HASH_FUNC_INLINE
hash_table hash_table_copy(region r, hash_table ht)
{
  unsigned long i;
  hash_table result;
  bucket cur, newbucket, *prev;

  result = make_hash_table(r, hash_table_capacity(ht));
  result->used = ht->used;

  for (i = 0; i < hash_table_capacity(ht); i++)
    {
      prev = &result->table[i];
      scan_bucket(ht->table[i], cur)
	{
	  newbucket = hash_table_alloc_bucket(result);
	  newbucket->key = cur->key;
	  newbucket->data = cur->data;
	  newbucket->next = NULL;
	  assert(!*prev);
	  *prev = newbucket;
	  prev = &newbucket->next;
	}
    }
  return result;
}

/* Reinsert all elements.  New size of table is (used*2) rounded up to a power
   of 2.  The table is re-allocated but buckets are re-used. */
HASH_FUNC_INLINE
void rehash(hash_table ht, hash_fn hash, keyeq_fn cmp)
{
#ifdef DEBUG
  // printf("Rehash table size=%ld, used=%ld\n", hash_table_capacity(ht), ht->used);
#endif

  unsigned long old_table_size = hash_table_capacity(ht);
  bucket *old_table = ht->table;

  init_size_pow2(ht, ht->used*2);

  // region old_r = NULL;
  // if (ht->owner && hash_table_capacity(ht) > RECLAIM_SPACE_THRESHOLD) {
  //   old_r = ht->r;
  //   ht->r = newregion();
  // }
  // struct miniregion old_mp[1] = { {NULL} };
  if (ht->owner) {
    // We no longer need to reset the region here, since we just reuse our
    // buckets.

    // old_mp->node = ht->mp->node;
    // miniregion_init(ht->mp);

    // ht->table = miniregion_alloc(ht->mp, hash_table_capacity(ht) * sizeof(bucket));
    ht->table = xcalloc(hash_table_capacity(ht), sizeof(bucket));
  } else {
    ht->table = rarrayalloc(ht->r, hash_table_capacity(ht), bucket);
  }

  unsigned long i;
  for (i = 0; i < old_table_size; i++) {
    bucket oldcur = old_table[i];
    while (oldcur) {
      // inline the following:
      //    hash_table_insert(ht, cur->key, cur->data);
      // but much faster, since we don't have to check existing keys (instead
      // just prepend), don't have to check check the size on every insert,
      // and we avoid leaking memory by reusing buckets.
      bucket *newcur = find_bucket(ht, hash, oldcur->key);
      bucket next = oldcur->next;
      // bucket b = hash_table_alloc_bucket(ht);
      // b->key = oldcur->key;
      // b->data = oldcur->data;
      // b->next = *newcur;
      // *newcur = b;

      // Mutate the bucket in-place to prepend oldcur to newcur
      oldcur->next = *newcur;
      *newcur = oldcur;
      oldcur = next;
    }
  }
  // if (old_r)
  //   deleteregion(old_r);
  // if (old_mp->node) {
  //   free(old_table);
  //   miniregion_delete(old_mp);
  // }
  if (ht->owner) {
    free(old_table);
  }
}

/* Reinsert all elements, checking for duplicates */
HASH_FUNC_INLINE
void rehash_check(hash_table ht, hash_fn hash, keyeq_fn cmp)
{
  unsigned long old_table_size = hash_table_capacity(ht);
  bucket *old_table = ht->table;

  init_size_pow2(ht, ht->used*2);

  if (ht->owner) {
    ht->table = xcalloc(hash_table_capacity(ht), sizeof(bucket));
  } else {
    ht->table = rarrayalloc(ht->r, hash_table_capacity(ht), bucket);
  }

  unsigned long i;
  for (i = 0; i < old_table_size; i++) {
    bucket oldcur = old_table[i];
    while (oldcur) {
      bucket *newcur = find_bucket(ht, hash, oldcur->key);
      bucket next = oldcur->next;

      while (*newcur)
      {
        if (cmp(oldcur->key, (*newcur)->key))
        {
          ht->used--;
          // key already exists (i.e. oldcur is a duplicate)
          goto rehash_check_next;
        }
        newcur = &(*newcur)->next;
      }

      // Mutate the bucket in-place to append oldcur to newcur
      oldcur->next = NULL;
      *newcur = oldcur;
    rehash_check_next:
      oldcur = next;
    }
  }
  if (ht->owner) {
    free(old_table);
  }
}

/* Begin scanning ht */
HASH_FUNC_INLINE
void hash_table_scan(hash_table ht, hash_table_scanner *hts)
{
  assert(ht);
  hts->ht = ht;
  hts->i = 0;
  hts->cur = hts->ht->table[0];
}

/* Get next elt in table, storing the elt in *k and *d if k and d are
   non-NULL, respectively.  Returns TRUE if there is a next elt, FALSE
   otherwise. */
HASH_FUNC_INLINE
bool hash_table_next(hash_table_scanner *hts, hash_key *k, hash_data *d)
{
  while (hts->cur == NULL)
    {
      hts->i++;
      if (hts->i < hash_table_capacity(hts->ht))
	hts->cur = hts->ht->table[hts->i];
      else
	break;
    }

  if (hts->i == hash_table_capacity(hts->ht))
    {
      return FALSE;
    }
  else
    {
      if (k)
	*k = hts->cur->key;
      if (d)
	*d = hts->cur->data;
      hts->cur = hts->cur->next;
    }
  return TRUE;
}

/* Apply f to all elements of ht, in some arbitrary order */
HASH_FUNC_INLINE
void hash_table_apply(hash_table ht, hash_apply_fn f, void *arg)
{
  unsigned long i;
  bucket cur;

  for (i = 0; i < hash_table_capacity(ht); i++)
    scan_bucket(ht->table[i], cur)
      f(cur->key, cur->data, arg);
}

/* Map f to all elements on ht, creating a new hash table in region r */
HASH_FUNC_INLINE
hash_table hash_table_map(region r, hash_table ht, hash_map_fn f, void *arg)
{
  unsigned long i;
  hash_table result;
  bucket cur, newbucket, *prev;

  result = make_hash_table(r, hash_table_capacity(ht));
  result->used = ht->used;

  for (i = 0; i < hash_table_capacity(ht); i++)
    {
      prev = &result->table[i];
      scan_bucket(ht->table[i], cur)
	{
	  newbucket = hash_table_alloc_bucket(result);
	  newbucket->key = cur->key;
	  newbucket->data = f(cur->key, cur->data, arg);
	  newbucket->next = NULL;
	  assert(!*prev);
	  *prev = newbucket;
	  prev = &newbucket->next;
	}
    }
  return result;
}

// int DEBUG_UPDATE_HASHES = 0;
/* Re-arrange hash-table in place after some elements may have their
   hash value change. */
HASH_FUNC_INLINE
void hash_table_update_hashes(hash_table ht, hash_fn hash, keyeq_fn cmp)
{
#if 0
  // quarl 2006-05-31
  //    alternate implementation with comparable performance, but uses more
  //    memory if using regions
  rehash_check(ht, hash, cmp);
#else
  unsigned long i;
  bucket *cur;

  for (i = 0; i < hash_table_capacity(ht); i++)
    {
      cur = &ht->table[i];
      while (*cur)
	{
	  bucket curp = *cur;

	  bucket *target = find_bucket(ht, hash, curp->key);
	  bool inplace = (target - ht->table == i);

          if (inplace && *target == *cur && (*cur)->next == NULL) {
            // The hash value hasn't changed, and it's the only entry for this
            // hash, so don't need to do anything.
            break;
          }

	  /* Remove the node in question */
	  *cur = curp->next;
	  curp->next = NULL;

	  while (*target)
	    {
	      if (cmp((*target)->key, curp->key))
		break;
	      target = &(*target)->next;
	    }

	  if (*target == NULL)
	    {
	      if (inplace)
		{
		  curp->next = *cur;
		  *cur = curp;
		  cur = &curp->next;
		}
	      else
                {
                  // if (DEBUG_UPDATE_HASHES) { BREAKER(); }
                  *target = curp;
                }
	    }
	  else
            {
              // if (DEBUG_UPDATE_HASHES) { BREAKER(); }
	      ht->used--;
            }


	}
    }
#endif
  // DEBUG_CHECK_HASH_TABLE(ht);
}

static keycmp_fn cur_cmp = NULL;

HASH_FUNC_INLINE
int entry_cmp(const void *a, const void *b)
{
  struct sorted_entry *ae = (struct sorted_entry *) a;
  struct sorted_entry *be = (struct sorted_entry *) b;
  return cur_cmp(ae->k, be->k);
}

/* Begin scanning ht in sorted order according to f */
HASH_FUNC_INLINE
void hash_table_scan_sorted(hash_table ht, keycmp_fn f,
			    hash_table_scanner_sorted *htss)
{
  hash_table_scanner hts;
  unsigned long i;

  htss->r = newregion();
  htss->size = hash_table_size(ht);
  htss->entries = rarrayalloc(htss->r, htss->size, struct sorted_entry);
  htss->i = 0;

  hash_table_scan(ht, &hts);
  i = 0;
  while (hash_table_next(&hts, &htss->entries[i].k,
			 &htss->entries[i].d))
    i++;
  assert(i == htss->size);
  cur_cmp = f;
  qsort(htss->entries, htss->size, sizeof(struct sorted_entry), entry_cmp);
  cur_cmp = NULL;
}

/* Just like hash_table_next, but scans in sorted order */
HASH_FUNC_INLINE
bool hash_table_next_sorted(hash_table_scanner_sorted *htss, hash_key *k,
			    hash_data *d)
{
  if (htss->i < htss->size)
    {
      if (k)
	*k = htss->entries[htss->i].k;
      if (d)
	*d = htss->entries[htss->i].d;
      htss->i++;
      return TRUE;
    }
  else
    {
      deleteregion(htss->r);
      htss->r = NULL;
      return FALSE;
    }
}

// bool in_DEBUG_CHECK_HASH_TABLE = 0;
HASH_FUNC_INLINE
void hash_table_really_check_for_duplicates(hash_table ht, hash_fn hash, keyeq_fn cmp)
{
  // in_DEBUG_CHECK_HASH_TABLE=1;
  region scratch = newregion();
  hash_table tmp = make_hash_table(scratch, ht->used);

  int actual_used = 0;
  int i;
  for (i = 0; i < hash_table_capacity(ht); i++) {
    bucket b = ht->table[i];
    while (b != NULL) {
      actual_used++;
      // DEBUG_CHECK_BUCKET(b);
      // static int count1 = 0;
      // ++count1;

      // unsigned long h = ht->hash(b->key);
      // printf("## key %p hash=%lu\n", b->key, h);

      bucket expected_bucket = *find_bucket(ht, hash, b->key);
      assert (expected_bucket == ht->table[i]);

      bool duplicate_key = hash_table_hash_search(tmp, hash, cmp, b->key, NULL);

      assert (!duplicate_key);

      bool inserted = hash_table_insert(tmp, hash, cmp, b->key, b->data);
      assert (inserted);
      b = b->next;
    }
  }
  assert( ht->used == actual_used );
  deleteregion(scratch);
  // in_DEBUG_CHECK_HASH_TABLE=0;
}
