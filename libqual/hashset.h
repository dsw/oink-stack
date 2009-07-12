// see License.txt for copyright and terms of use

// quarl 2006-07-01
//    hashset - a hashset highly optimized for space.

#ifndef HASHSET_H
#define HASHSET_H

#include "libqual/hash.h"

typedef struct Hashset_table *hashset_table;

/* Make a new hash table, with size buckets initially.  Hash table
   elements are allocated in region rhash. */
HASH_FUNC_INLINE
hashset_table make_hashset_table(region rhash, unsigned long size
                                 // ,hash_fn hash, keyeq_fn cmp
  );

// /* Make a hash table for strings. */
// HASH_FUNC_INLINE
// hashset_table make_string_hashset_table(region rhash, unsigned long size);

/* Zero out ht.  Doesn't reclaim bucket space. */
HASH_FUNC_INLINE
void hashset_table_reset(hashset_table ht);

HASH_FUNC_INLINE
void destroy_hashset_table(hashset_table ht);

/* Return the number of entries in ht */
HASH_FUNC_INLINE
unsigned long hashset_table_size(hashset_table ht);

/* Given an equality predicate function which agrees with our
   hash_function, search for the given element. */
HASH_FUNC_INLINE
bool hashset_table_hash_search(hashset_table ht,
                               hash_fn hash, keyeq_fn cmp,
                               hash_key k, hash_key *d);

/* brute-force search entire table (for debugging purposes) */
HASH_FUNC_INLINE
bool hashset_table_hash_search_hard(hashset_table ht, keyeq_fn cmp,
                                    hash_key k, hash_key *d);

// /* Lookup k in ht.  If d is not NULL, returns corresponding data in *d.
//    Function result is TRUE if the k was in ht, false otherwise. */
// HASH_FUNC_INLINE
// bool hashset_table_lookup(hashset_table ht, hash_key k, hash_key *d);

/* Add k:d to ht.  If k was already in ht, replace old entry by k:d.
   Rehash if necessary.  Returns TRUE if k was not already in ht. */
HASH_FUNC_INLINE
bool hashset_table_insert(hashset_table ht,
                          hash_fn hash, keyeq_fn cmp,
                          hash_key k);

/* Remove mapping for k in ht.  Returns TRUE if k was in ht. */
HASH_FUNC_INLINE
bool hashset_table_remove(hashset_table ht,
                          hash_fn hash, keyeq_fn cmp,
                          hash_key k);

/* Return a copy of ht, allocated in rhash */
HASH_FUNC_INLINE
hashset_table hashset_table_copy(region rhash, hashset_table ht,
                                 hash_fn hash, keyeq_fn cmp);

// /* Map f to all elements on ht, creating a new hash table */
// HASH_FUNC_INLINE
// hashset_table hashset_table_map(region r, hashset_table ht, hashset_map_fn f, void *arg);

/* Re-arrange hash-table in place after some element's may have their
   hash value change. */
HASH_FUNC_INLINE
void hashset_table_update_hashes(hashset_table ht, hash_fn hash, keyeq_fn cmp);

typedef struct
{
  hashset_table ht;
  size_t i;
} hashset_table_scanner; /* Opaque type!  Do not modify fields. */

/* Begin scanning ht */
HASH_FUNC_INLINE
void hashset_table_scan(hashset_table ht, hashset_table_scanner *);

/* Get next elt in table, storing the elt in *k and *d if k and d are
   non-NULL, respectively.  Returns TRUE if there is a next elt, FALSE
   otherwise. */
HASH_FUNC_INLINE
bool hashset_table_next(hashset_table_scanner *, hash_key *k);

// typedef struct
// {
//   region r;
//   unsigned long i;
//   unsigned long size;
//   hash_key *entries;
// } hashset_table_scanner_sorted;

// /* Begin scanning ht in sorted order according to f */
// HASH_FUNC_INLINE
// void hashset_table_scan_sorted(hashset_table ht, keycmp_fn f,
//                                hashset_table_scanner_sorted *htss);

// /* Just like hashset_table_next, but scans in sorted order */
// HASH_FUNC_INLINE
// bool hashset_table_next_sorted(hashset_table_scanner_sorted *htss, hash_key *k,
//                                hash_data *d);

HASH_FUNC_INLINE
void hashset_table_really_check_for_duplicates(hashset_table ht, hash_fn hash, keyeq_fn cmp);

#if defined HASH_EMBED_INLINE
# include "libqual/hashset.c"
#endif

#endif
