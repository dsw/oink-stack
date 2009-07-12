/* see License.txt for copyright and terms of use */

#ifndef HASH_H
#define HASH_H

#include "libqual/bool.h"
#include "libregion/regions.h"

// This allows us to #include the entire hash.c file and inline all of its
// functions.  hash_table_next() especially benefits from inlining.
#if defined HASH_EMBED_INLINE
# define HASH_FUNC_INLINE static inline
#else
# define HASH_FUNC_INLINE /*nothing*/
#endif

typedef void *hash_key;
typedef void *hash_data;

typedef unsigned long (*hash_fn)(hash_key k);    /* Function to hash a key */
typedef bool (*keyeq_fn)(hash_key k1, hash_key k2);
                                         /* Function returning true iff
					     k1 and k2 are equal */
typedef void (*hash_apply_fn)(hash_key k, hash_data d, void *arg);
                                          /* Function applied to
					     elts in the hash table */
typedef hash_data (*hash_map_fn)(hash_key k, hash_data d, void *arg);
                                          /* Function mapped to
					     elts in the hash table */

typedef struct Hash_table *hash_table;

/* Make a new hash table, with size buckets initially.  Hash table
   elements are allocated in region rhash. */
HASH_FUNC_INLINE
hash_table make_hash_table(region rhash, unsigned long size
                           /*,hash_fn hash, keyeq_fn cmp*/);

// /* Make a hash table for strings. */
// HASH_FUNC_INLINE
// hash_table make_string_hash_table(region rhash, unsigned long size);

/* Zero out ht.  Doesn't reclaim bucket space. */
HASH_FUNC_INLINE
void hash_table_reset(hash_table ht);

HASH_FUNC_INLINE
void destroy_hash_table(hash_table ht);

/* Return the number of entries in ht */
HASH_FUNC_INLINE
unsigned long hash_table_size(hash_table ht);

/* Given an equality predicate function which agrees with our
   hash_function, search for the given element. */
HASH_FUNC_INLINE
bool hash_table_hash_search(hash_table ht, hash_fn hash, keyeq_fn cmp,
			    hash_key k, hash_data *d);

/* brute-force search entire table (for debugging purposes) */
HASH_FUNC_INLINE
bool hash_table_hash_search_hard(hash_table ht, hash_fn hash, keyeq_fn cmp,
                                 hash_key k, hash_data *d);

// /* Lookup k in ht.  If d is not NULL, returns corresponding data in *d.
//    Function result is TRUE if the k was in ht, false otherwise. */
// HASH_FUNC_INLINE
// bool hash_table_lookup(hash_table ht, hash_key k, hash_data *d);

/* Add k:d to ht.  If k was already in ht, replace old entry by k:d.
   Rehash if necessary.  Returns TRUE if k was not already in ht. */
HASH_FUNC_INLINE
bool hash_table_insert(hash_table ht, hash_fn hash, keyeq_fn cmp,
                       hash_key k, hash_data d);

/* Remove mapping for k in ht.  Returns TRUE if k was in ht. */
HASH_FUNC_INLINE
bool hash_table_remove(hash_table ht, hash_fn hash, keyeq_fn cmp, hash_key k);

/* Return a copy of ht, allocated in rhash */
HASH_FUNC_INLINE
hash_table hash_table_copy(region rhash, hash_table ht);

/* Map f to all elements on ht, creating a new hash table */
HASH_FUNC_INLINE
hash_table hash_table_map(region r, hash_table ht,
                          hash_map_fn f, void *arg);

/* Re-arrange hash-table in place after some element's may have their
   hash value change. */
HASH_FUNC_INLINE
void hash_table_update_hashes(hash_table ht, hash_fn hash, keyeq_fn cmp);

typedef struct Bucket *bucket;
typedef struct
{
  hash_table ht;
  unsigned long i;
  bucket cur;
} hash_table_scanner; /* Opaque type!  Do not modify fields. */

/* Begin scanning ht */
HASH_FUNC_INLINE
void hash_table_scan(hash_table ht, hash_table_scanner *);

/* Get next elt in table, storing the elt in *k and *d if k and d are
   non-NULL, respectively.  Returns TRUE if there is a next elt, FALSE
   otherwise. */
HASH_FUNC_INLINE
bool hash_table_next(hash_table_scanner *, hash_key *k, hash_data *d);

/* Total order on hash table keys, only uesd for hash_table_scan_sorted */
typedef int (*keycmp_fn)(hash_key k1, hash_key k2);

struct sorted_entry
{
  hash_key k;
  hash_data d;
};

typedef struct
{
  region r;
  unsigned long i;
  unsigned long size;
  struct sorted_entry *entries;
} hash_table_scanner_sorted;

/* Begin scanning ht in sorted order according to f */
HASH_FUNC_INLINE
void hash_table_scan_sorted(hash_table ht, keycmp_fn f,
			    hash_table_scanner_sorted *htss);

/* Just like hash_table_next, but scans in sorted order */
HASH_FUNC_INLINE
bool hash_table_next_sorted(hash_table_scanner_sorted *htss, hash_key *k,
			    hash_data *d);

HASH_FUNC_INLINE
void hash_table_really_check_for_duplicates(hash_table ht, hash_fn hash, keyeq_fn cmp);

#if defined HASH_EMBED_INLINE
# include "libqual/hash.c"
#endif

#endif
