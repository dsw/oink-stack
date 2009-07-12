// see License.txt for copyright and terms of use

// quarl 2006-06-23
//    Initial version.  Space-optimized hash set: no key/value distinction --
//    pointers to target objects stored directly in the table; collisions
//    resolved via quadratic probing instead of chaining; header as small as
//    possible.
//
//    At sizes < 8, the table is really an array that grows linearly.
//
//    The table can use regions or malloc/free.

// TODO: when we C++-ify, templatize on allocator (and hash, etc., of course)

#include "libqual/hashset.h"
#include "libqual/utils.h"
#include <string.h>

static int HASHSET_TOMBSTONE_DUMMY_DATA = 0xbeefbeef;
static hash_key HASHSET_TOMBSTONE = &HASHSET_TOMBSTONE_DUMMY_DATA;

// TODO: move Hashset_table into header so we can inline its contents into
// parent structs, thus getting rid of a pointer!

#define HASHSET_TABLE_MAXSIZE ((1<<27)-1)

#define HASHSET_TABLE_SUPPORT_REGION 0

#define HASHSET_TABLE_ARRAY_THRESHOLD 8

// Hashset_table operates in two modes.
//    1) Hash mode - regular hash table
//    2) Array mode - table is really a linearly-growing array.
//       ht->log2size==0 and ht->used == size of table; the table is always at
//       ht->full capacity.  If ht->used==0 then table=NULL.

struct Hashset_table
{
#if HASHSET_TABLE_SUPPORT_REGION
  region r;                     // region for this table, or NULL
#endif
  // hash_fn hash;                 /* Function for hashing keys */
  // keyeq_fn cmp;                 /* Function for comparing keys */

  // unsigned long size;           /* Number of buckets */
  unsigned long used : 27;          /* Number of elements, including tombstones */
  unsigned long log2size : 5;       /* log2 of size, or 0 if array-mode */
  hash_key *table;              /* Array of (size) buckets */
};

HASH_FUNC_INLINE
void hashset_rehash(hashset_table ht, hash_fn hash, keyeq_fn cmp);

/* Force size to a power of 2; hash mode */
static inline unsigned long hashset_init_size_pow2(hashset_table ht, unsigned long input_size)
{
  if (input_size <= 2) {
    // log2size must be at least 1 because 0 indicates array mode
    ht->log2size = 1;
    return 2;
  }

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
static inline unsigned long hashset_table_capacity(hashset_table ht)
{
  assert(ht->log2size > 0); // not array mode
  return 1 << ht->log2size;
}

static inline unsigned long hashset_table_capacity_or_array_size(hashset_table ht)
{
  if (ht->log2size == 0) {      // array mode
    return ht->used;
  } else {                      // hash mode
    return 1 << ht->log2size;
  }
}

static inline bool hashset_do_cmp(hash_key table_key,
                                  hash_key cmp_key, keyeq_fn cmp)
{
  assert(table_key != NULL);
  // Note: HASHSET_TOMBSTONE is not a valid key so we're not allowed to call
  // cmp() with it.
  if (table_key == HASHSET_TOMBSTONE) return FALSE;
  if (table_key == cmp_key) return TRUE;
  return cmp(table_key, cmp_key);
}

// Make a new hash table, with SIZE buckets initially.
//
// The Hash table structure is allocated with malloc() and elements are
// allocated in a region.  Must call destroy_hashset_table() to deallocate it.
HASH_FUNC_INLINE
hashset_table make_hashset_table_owner(unsigned long size /*, hash_fn hash,
                                                            keyeq_fn cmp*/)
{
  hashset_table ht;

  ht = xmalloc(sizeof(struct Hashset_table));
#if HASHSET_TABLE_SUPPORT_REGION
  ht->r = NULL;
#endif
  // ht->hash = hash;
  // ht->cmp = cmp;
  ht->used = 0;
  if (size) {
    size = hashset_init_size_pow2(ht, size);
    ht->table = xcalloc(size, sizeof(hash_key));
  } else {
    ht->log2size = 0;
    ht->table = NULL;
  }

  return ht;
}

// Make a new hash table, with SIZE buckets initially.
// Hash table and its elements are allocated in region r.
HASH_FUNC_INLINE
hashset_table make_hashset_table(region r, unsigned long size //,
                                 // hash_fn hash, keyeq_fn cmp
  )
{
#if HASHSET_TABLE_SUPPORT_REGION
  if (!r) return make_hashset_table_owner(size /*, hash, cmp */);

  hashset_table ht;

  ht = ralloc(r, struct Hashset_table);
  ht->r = r;
  // ht->hash = hash;
  // ht->cmp = cmp;
  ht->used = 0;
  if (size) {
    size = hashset_init_size_pow2(ht, size);
    ht->table = rarrayalloc(ht->r, size, hash_key);
  } else {
    ht->log2size = 0;
    ht->table = NULL;
  }

  return ht;
#else
  assert(r == NULL);
  return make_hashset_table_owner(size);
#endif
}

// /* Make a hash table for strings. */
// HASH_FUNC_INLINE
// hashset_table make_string_hashset_table(region rhash, unsigned long size)
// {
//   return make_hashset_table(rhash, size, (hash_fn) string_hash,
//                             (keyeq_fn) string_eq);
// }

// This destroys the contents of the hash table; must have been created with
// make_hashset_table_owner().
HASH_FUNC_INLINE
void destroy_hashset_table(hashset_table ht)
{
#if HASHSET_TABLE_SUPPORT_REGION
  assert(!ht->r);
#endif
  free(ht->table);
  ht->table = (void*) 0xbeefbeef;
  free(ht);
}

/* Zero out ht. */
HASH_FUNC_INLINE
void hashset_table_reset(hashset_table ht)
{
  if (ht->log2size == 0) {
    // array mode
    free(ht->table);
    ht->table = NULL;
    ht->used = 0;
  } else {
    // hash mode
    memset(ht->table, 0, sizeof(hash_key)*hashset_table_capacity(ht));
    ht->used = 0;
  }
}

/* Return the number of entries in ht */
HASH_FUNC_INLINE
unsigned long hashset_table_size(hashset_table ht)
{
  if (ht->log2size == 0) {
    // quarl 2006-07-10
    //    Array mode: ht->used is full size; no tombstones
    return ht->used;
  } else {
    // return ht->used;

    // quarl 2006-07-09
    //    ht->used now includes tombstones, so we have to count.

    unsigned long i;
    unsigned long count = 0;
    for (i = 0; i < hashset_table_capacity(ht); ++i) {
      if (ht->table[i] != NULL && ht->table[i] != HASHSET_TOMBSTONE) {
        ++count;
      }
    }
    return count;
  }
}

#define MAGIC 2*0.6180339987

#define LLMAGIC ((unsigned long long)(MAGIC * (1ULL << (8 * sizeof(unsigned long) - 1))))

/* Return the bucket index corresponding to k in ht */
static inline size_t hashset_find_bucket(hashset_table ht, hash_fn hash,
                                         hash_key k)
{
  unsigned long hashVal;

  if (hashset_table_capacity(ht) == 1) {
    hashVal = 0;
  } else {
    hashVal = hash(k);
    hashVal = hashVal * LLMAGIC;
    hashVal = hashVal >> (8 * sizeof(unsigned long) - ht->log2size);
    /* hashVal = hashVal % ht->size; */
  }
  assert(hashVal < hashset_table_capacity(ht));
  return hashVal;
}

static inline int hashset_probe(hashset_table ht, size_t hash, size_t i)
{
  // quadratic probing h(k,i) = h(k) + c_1*i + c_2*i^2(mod m)
  //   c_1 = c_2 = 1/2
  return (hash + (i*(i+1))/2) & ((1 << ht->log2size) - 1);
}

/* Given a comparison function which agrees with our hash_function,
   search for the given element. */
HASH_FUNC_INLINE
bool hashset_table_hash_search(hashset_table ht,
                               hash_fn hash,
                               keyeq_fn cmp,
                               hash_key k, hash_data *d)
{
  assert(ht);

  if (ht->log2size == 0) {
    // array mode: exhaustively search the entire table.

    size_t i;
    for (i = 0; i < ht->used; ++i) {
      hash_key key = ht->table[i];
      assert(key != NULL);
      if (key == k || cmp(key, k)) {
        if (d) *d = key;
        return TRUE;
      }
    }
    return FALSE;

  } else {
    // hash mode
    size_t hashVal = hashset_find_bucket(ht, hash, k);
    size_t curIndex = hashVal;
    size_t i = 0;
    while (1) {
      hash_key key = ht->table[curIndex];

      if (key == NULL)
        return FALSE;

      if (hashset_do_cmp(key, k, cmp)) {
        if (d) *d = key;
        return TRUE;
      }

      ++i;
      if (i == hashset_table_capacity(ht))
        return FALSE;

      curIndex = hashset_probe(ht, hashVal, i);
      assert(curIndex != hashVal);
    }
  }
}

/* brute-force search entire table (for debugging purposes) */
HASH_FUNC_INLINE
bool hashset_table_hash_search_hard(hashset_table ht, keyeq_fn cmp,
                                    hash_key k, hash_data *d)
{
  assert(ht);

  if (ht->log2size == 0) {
    // array mode
    size_t i;
    for (i = 0; i < ht->used; ++i) {
      assert(ht->table[i] != NULL && ht->table[i] != HASHSET_TOMBSTONE);
      if (ht->table[i] == k || cmp(ht->table[i], k)) {
        if (d)
          *d = ht->table[i];
        return TRUE;
      }
    }
    return FALSE;
  } else {
    // hash mode
    int i;
    for (i = 0; i < hashset_table_capacity(ht); i++) {
      if (ht->table[i] == NULL) continue;

      if (hashset_do_cmp(ht->table[i], k, cmp)) {
        if (d)
          *d = ht->table[i];
        return TRUE;
      }
    }
    return FALSE;
  }
}

// Insert a key, but don't check for existing keys nor tombstones, nor check
// for fullness, nor increment count.  Should only be used internally.
HASH_FUNC_INLINE
void hashset_table_insert_internal(hashset_table ht, hash_fn hash, keyeq_fn cmp,
                                   hash_key k)
{
  assert(ht->log2size != 0);    // hash mode
  size_t hashVal = hashset_find_bucket(ht, hash, k);
  size_t curIndex = hashVal;
  size_t i = 0;
  while (1) {
    hash_key key = ht->table[curIndex];

    if (key == NULL) {
      ht->table[curIndex] = k;
      return;
    }

    ++i;
    curIndex = hashset_probe(ht, hashVal, i);
    assert(i < hashset_table_capacity(ht));
    assert(curIndex != hashVal);
  }
}


HASH_FUNC_INLINE
void hashset_table_convert_to_hash_mode(hashset_table ht, hash_fn hash, keyeq_fn cmp)
{
  // array mode
  hash_key *old_table = ht->table;
  size_t old_table_size = ht->used;

  // add 1 since we're about to insert after this; don't multiply by 2 because
  // the whole point of array mode is to save memory!
  hashset_init_size_pow2(ht, ht->used+1);

#if HASHSET_TABLE_SUPPORT_REGION
  if (!ht->r) {
    ht->table = xcalloc(hashset_table_capacity(ht), sizeof(hash_key));
  } else {
    ht->table = rarrayalloc(ht->r, hashset_table_capacity(ht), hash_key);
  }
#else
  ht->table = xcalloc(hashset_table_capacity(ht), sizeof(hash_key));
#endif

  size_t i;
  for (i = 0; i < old_table_size; i++) {
    hash_key k = old_table[i];
    assert(k != NULL && k != HASHSET_TOMBSTONE);
    hashset_table_insert_internal(ht, hash, cmp, k);
  }

  // ht->used doesn't change.

#if HASHSET_TABLE_SUPPORT_REGION
  if (!ht->r) {
    free(old_table);
  }
#else
  free(old_table);
#endif
}

// /* Lookup k in ht.  Returns corresponding data in *d, and function
//    result is TRUE if the k was in ht, false otherwise. */
// HASH_FUNC_INLINE
// bool hashset_table_lookup(hashset_table ht, hash_key k, hash_data *d)
// {
//   return hashset_table_hash_search(ht, ht->cmp, k, d);
// }

/* Add k:d to ht.  If k was already in ht, replace old entry by k:d.
   Rehash if necessary.  Returns TRUE if k was not already in ht. */
HASH_FUNC_INLINE
bool hashset_table_insert(hashset_table ht, hash_fn hash, keyeq_fn cmp,
                          hash_key k)
{
  assert(ht);

  if (ht->log2size == 0) {
    // array mode

    // replace?
    size_t i;
    for (i = 0; i < ht->used; ++i) {
      hash_key key = ht->table[i];
      if (key == k || cmp(key, k)) {
        ht->table[i] = k;
        return FALSE; /* replace */
      }
    }

    if (ht->used >= HASHSET_TABLE_ARRAY_THRESHOLD) {
      hashset_table_convert_to_hash_mode(ht, hash, cmp);
      // hash mode now
      assert(ht->log2size != 0);
      hashset_table_insert_internal(ht, hash, cmp, k);
      ht->used += 1;
    } else {
      // Resize.  We depend on the system realloc being smart and only
      // re-allocating if necessary (i.e. between 4 and 8 bytes, or if nothing
      // after, don't need to reallocate).
      ht->table = realloc(ht->table, (sizeof (hash_key))*(ht->used + 1));

      // append
      ht->table[ht->used] = k;
      ht->used += 1;
    }

    return TRUE; /* new key */

  } else {
    // hash mode

    if (ht->used > 3 * hashset_table_capacity(ht) / 4)
      hashset_rehash(ht, hash, cmp);
    if (ht->used >= HASHSET_TABLE_MAXSIZE) {
      abort();
    }

    size_t hashVal = hashset_find_bucket(ht, hash, k);
    size_t curIndex = hashVal;
    size_t tombstoneIndex = (size_t) -1;
    size_t i = 0;
    while (1) {
      hash_key key = ht->table[curIndex];

      if (key == NULL) {
        if (tombstoneIndex != (size_t) -1) {
          assert(ht->table[tombstoneIndex] == HASHSET_TOMBSTONE);
          ht->table[tombstoneIndex] = k;
          // don't increment ht->used since it already counts tombstones.
        } else {
          ht->table[curIndex] = k;
          ht->used++;
        }
        return TRUE; /* new key */
      }

      if (key == HASHSET_TOMBSTONE) {
        // quarl 2006-10-24
        //     We can replace this tombstone entry with the new key.  BUT, we
        //     still have to see if the key is currently in the table.  So keep
        //     looking, and if we don't find it, then replace the first tombstone
        //     we found with the new key.  Thanks to Simon Goldsmith & trendprof.
        if (tombstoneIndex == (size_t) -1) {
          tombstoneIndex = curIndex;
        }
      }

      if (hashset_do_cmp(key, k, cmp)) {
        // Update the data.
        //
        // quarl 2006-05-28:
        //     We *must* update the key since keys are potentially mutable and
        //     thus not the same even if they compare the same; not updating
        //     will cause inconsistencies later.
        ht->table[curIndex] = k;
        return FALSE; /* replace */
      }

      ++i;
      curIndex = hashset_probe(ht, hashVal, i);
      assert(i < hashset_table_capacity(ht)); // can't be 100% full - would have rehashed
      assert(curIndex != hashVal);
    }
  }
}

/* Remove mapping for k in ht.  Returns TRUE if k was in ht. */
HASH_FUNC_INLINE
bool hashset_table_remove(hashset_table ht,
                          hash_fn hash, keyeq_fn cmp,
                          hash_key k)
{
  assert(ht);
  if (ht->log2size == 0) {
    // array mode

    size_t i;
    for (i = 0; i < ht->used; ++i) {
      hash_key key = ht->table[i];
      assert(key != NULL);
      if (key == k || cmp(key, k)) {
        // found key to delete.

        if (i != ht->used-1) {
          // move the last item in so we can resize.
          ht->table[i] = ht->table[ht->used-1];
        }
        // Resize.  In array mode, ht->used always represents number of items.
        // We depend on the system realloc() being smart; see
        // hashset_table_insert().
        ht->used -= 1;
        // Note that if new size is 0, realloc frees (exactly what we want).
        ht->table = realloc(ht->table, (sizeof (hash_key))*(ht->used));
        return TRUE;
      }
    }
    return FALSE;
  } else {
    size_t hashVal = hashset_find_bucket(ht, hash, k);
    size_t curIndex = hashVal;
    size_t i = 0;
    while (1) {
      hash_key key = ht->table[curIndex];

      if (key == NULL) return FALSE;
      if (hashset_do_cmp(key, k, cmp)) {
        break;
      }
      ++i;
      curIndex = hashset_probe(ht, hashVal, i);
      assert(i < hashset_table_capacity(ht)); // can't be 100% full - would have rehashed
      assert(curIndex != hashVal);
    }

    // quarl 2006-07-10
    //    We store a "tombstone" that indicates this key is dead.  We don't
    //    decrement ht->used.  After a while, the table will fill up with
    //    tombstones, but we remove them whenever we rehash.  Another thing we
    //    could do is probabilistically remove tombstones, e.g. every
    //    O(1/capacity) removes (we don't want to count tombstones since that
    //    would increase the size of the header).

    ht->table[curIndex] = HASHSET_TOMBSTONE;
    // don't decrement ht->used.
    return TRUE;
  }
}

/* Return a copy of ht */
HASH_FUNC_INLINE
hashset_table hashset_table_copy(region r, hashset_table ht,
                                 hash_fn hash, keyeq_fn cmp)
{
  hashset_table result;
  if (ht->log2size == 0) {
    // array mode
    result = make_hashset_table(r, 0);
    assert(result->log2size == 0);
    result->used = ht->used;
    result->table = malloc(ht->used * sizeof(hash_key));
    memcpy(result->table, ht->table, ht->used * sizeof(hash_key));
  } else {
    result = make_hashset_table(r, hashset_table_capacity(ht) /*, ht->hash, ht->cmp */);

    size_t curIndex;
    for (curIndex = 0; curIndex < hashset_table_capacity(ht); curIndex++) {
      hash_key key = ht->table[curIndex];
      if (key != NULL && key != HASHSET_TOMBSTONE) {
        hashset_table_insert_internal(result, hash, cmp, key);
        result->used += 1;
      }
    }
  }
  return result;
}

/* Reinsert all elements.  New size of table is (used*2) rounded up to a power
   of 2.  Duplicates are re-duplicated.  If using regions, the memory used by
   the old table is lost until region deletion. */
HASH_FUNC_INLINE
void hashset_rehash(hashset_table ht, hash_fn hash, keyeq_fn cmp)
{
#ifdef DEBUG
  // printf("Rehash table size=%ld, used=%ld\n", ht->size, ht->used);
#endif

  unsigned long old_table_size = hashset_table_capacity(ht);
  hash_key *old_table = ht->table;

  hashset_init_size_pow2(ht, ht->used*2);

#if HASHSET_TABLE_SUPPORT_REGION
  if (!ht->r) {
    ht->table = xcalloc(hashset_table_capacity(ht), sizeof(hash_key));
  } else {
    ht->table = rarrayalloc(ht->r, hashset_table_capacity(ht), hash_key);
  }
#else
  ht->table = xcalloc(hashset_table_capacity(ht), sizeof(hash_key));
#endif

  size_t i;
  for (i = 0; i < old_table_size; i++) {
    hash_key k = old_table[i];
    if (k == HASHSET_TOMBSTONE) {
      ht->used -= 1;
      continue;
    }

    if (k != NULL) {
      // For performance, we don't check for duplicates; use
      // hashset_rehash_check() if you want that.

      hashset_table_insert_internal(ht, hash, cmp, k);
    }
  }

#if HASHSET_TABLE_SUPPORT_REGION
  if (!ht->r) {
    free(old_table);
  }
#else
  free(old_table);
#endif
}

/* Reinsert all elements, checking for duplicates */
HASH_FUNC_INLINE
void hashset_rehash_check(hashset_table ht,
                          hash_fn hash, keyeq_fn cmp)
{
  unsigned long old_table_size = hashset_table_capacity(ht);
  hash_key *old_table = ht->table;

  hashset_init_size_pow2(ht, ht->used*2);

#if HASHSET_TABLE_SUPPORT_REGION
  if (!ht->r) {
    ht->table = xcalloc(hashset_table_capacity(ht), sizeof(hash_key));
  } else {
    ht->table = rarrayalloc(ht->r, hashset_table_capacity(ht), hash_key);
  }
#else
  ht->table = xcalloc(hashset_table_capacity(ht), sizeof(hash_key));
#endif

  size_t i;
  for (i = 0; i < old_table_size; i++) {
    hash_key k = old_table[i];
    if (k == HASHSET_TOMBSTONE) {
      ht->used -= 1;
      continue;
    }

    if (k != NULL) {
      // inline the following but without hash check:
      //    hashset_table_insert(ht, key);

      size_t hashVal = hashset_find_bucket(ht, hash, k);
      size_t curIndex = hashVal;
      size_t i = 0;
      while (1) {
        hash_key key = ht->table[curIndex];

        if (key == NULL) {
          ht->table[curIndex] = k;
          break;
        }

        // no reason for the table we're creating to have tombstones!
        assert(key != HASHSET_TOMBSTONE);
        if (key == k || cmp(k, key)) {
          // duplicate!
          ht->used--;
          break;
        }

        ++i;
        curIndex = hashset_probe(ht, hashVal, i);
        assert(i < hashset_table_capacity(ht));
        assert(curIndex != hashVal);
      }
    }
  }

#if HASHSET_TABLE_SUPPORT_REGION
  if (!ht->r) {
    free(old_table);
  }
#else
  free(old_table);
#endif
}

/* Begin scanning ht */
HASH_FUNC_INLINE
void hashset_table_scan(hashset_table ht, hashset_table_scanner *hts)
{
  assert(ht);
  hts->ht = ht;
  hts->i = 0;
}

#define DEBUG_HASHSET_NEXT 1

#ifdef DEBUG_HASHSET_NEXT
extern long debug_count_hashset_table_next_calls;
extern long debug_count_hashset_table_next_loops;
extern long debug_count_hashset_table_next_nulls;
extern long debug_count_hashset_table_next_tombstones;

#ifdef HASHSET_DEF

long debug_count_hashset_table_next_calls = 0;
long debug_count_hashset_table_next_loops = 0;
long debug_count_hashset_table_next_nulls = 0;
long debug_count_hashset_table_next_tombstones = 0;

void hashset_table_next_debug_report()
{
  fprintf(stderr, "## debug count_hashset_table_next:\n");
  fprintf(stderr, "##   calls = %ld\n", debug_count_hashset_table_next_calls);
  fprintf(stderr, "##   loops = %ld\n", debug_count_hashset_table_next_loops);
  fprintf(stderr, "##   nulls = %ld\n", debug_count_hashset_table_next_nulls);
  fprintf(stderr, "##   tombstones = %ld\n", debug_count_hashset_table_next_tombstones);
}
#endif
#endif

/* Get next elt in table, storing the elt in *k and *d if k and d are
   non-NULL, respectively.  Returns TRUE if there is a next elt, FALSE
   otherwise. */
HASH_FUNC_INLINE
bool hashset_table_next(hashset_table_scanner *hts, hash_key *k)
{
#ifdef DEBUG_HASHSET_NEXT
  ++debug_count_hashset_table_next_calls;
#endif
  while (hts->i < hashset_table_capacity_or_array_size(hts->ht)) {
#ifdef DEBUG_HASHSET_NEXT
    ++debug_count_hashset_table_next_loops;
    if (hts->ht->table[hts->i] == NULL) {
      ++debug_count_hashset_table_next_nulls;
    }
    if (hts->ht->table[hts->i] == HASHSET_TOMBSTONE) {
      ++debug_count_hashset_table_next_tombstones;
    }
#endif
    if (hts->ht->table[hts->i] != NULL && hts->ht->table[hts->i] != HASHSET_TOMBSTONE) {
      *k = hts->ht->table[hts->i];
      hts->i++;
      return TRUE;
    } else {
      hts->i++;
    }
  }

  return FALSE;
}

// We don't use hash values but we do need to remove duplicates.  Create a
// temporary hash table to find duplicates.
HASH_FUNC_INLINE
void hashset_array_mode_update_hashes(hashset_table ht, hash_fn hash, keyeq_fn cmp)
{
  assert(ht->used <= HASHSET_TABLE_ARRAY_THRESHOLD);
  hashset_table tmp = make_hashset_table(NULL, HASHSET_TABLE_ARRAY_THRESHOLD);
  size_t i = 0;
  size_t new_size = ht->used;
  while (i < new_size) {
    if (!hashset_table_insert(tmp, hash, cmp, ht->table[i])) {
      // duplicate!  replace this item with the one at the end of the table.
      new_size -= 1;
      if (i != new_size) {
        ht->table[i] = ht->table[new_size];
      }
      continue;                 // don't ++i
    }

    ++i;
  }
  ht->table = realloc(ht->table, new_size*sizeof(hash_key));
  ht->used = new_size;
  destroy_hashset_table(tmp);
}

/* Re-arrange hash-table in place after some elements may have their
   hash value change (including some may now be duplicates). */
HASH_FUNC_INLINE
void hashset_table_update_hashes(hashset_table ht, hash_fn hash, keyeq_fn cmp)
{
  if (ht->log2size == 0) {
    hashset_array_mode_update_hashes(ht, hash, cmp);
  } else {
    hashset_rehash_check(ht, hash, cmp);
  }
}

// bool in_DEBUG_CHECK_HASHSET_TABLE = 0;
HASH_FUNC_INLINE
void hashset_table_really_check_for_duplicates(hashset_table ht,
                                               hash_fn hash, keyeq_fn cmp)
{
  // in_DEBUG_CHECK_HASHSET_TABLE=1;
  // region scratch = newregion();
  hashset_table tmp = make_hashset_table(NULL/*scratch*/, ht->used/*, ht->hash, ht->cmp*/);

  size_t actual_used = 0;
  size_t i;
  for (i = 0; i < hashset_table_capacity_or_array_size(ht); i++) {
    hash_key k = ht->table[i];
    if (k == NULL) continue;
    actual_used ++;
    if (k == HASHSET_TOMBSTONE) continue;

    bool duplicate_key = hashset_table_hash_search(tmp, hash, cmp, k, NULL);
    assert (!duplicate_key);

    bool inserted = hashset_table_insert(tmp, hash, cmp, k);
    assert (inserted);
  }
  assert( ht->used == actual_used );
  // deleteregion(scratch);
  destroy_hashset_table(tmp);
  // in_DEBUG_CHECK_HASHSET_TABLE=0;
}
