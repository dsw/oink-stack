// $Id$
#include <glibc-fs/prelude.h>
#include <search.h>
#include <string.h>                                 // memcpy

// binary tree stuff

// struct qelem
// {
//     struct qelem *q_forw;
//     struct qelem *q_back;
//     char q_data[1];
// };

// NOTE: potential memcpy bug in qelem::q_data

void insque (void *elem0, void *prev0)
{
    // (fake implementation only for dataflow)
    struct qelem* elem = elem0;
    struct qelem* prev = prev0;

    prev->q_forw = elem;
    prev->q_back = elem;
    elem->q_forw = prev;
    elem->q_back = prev;
}

void remque (void *elem) {
    // (nothing needed for dataflow)
}

// typedef struct entry
// {
//     char *key;
//     void *data;
// } ENTRY;

// /* Opaque type for internal use.  */
// struct _ENTRY;

// custom _ENTRY for annotations
struct _ENTRY {
    ENTRY entry;
};

// struct hsearch_data
// {
//     struct _ENTRY *table;
//     unsigned int size;
//     unsigned int filled;
// };

struct hsearch_data the_global_hsearch_data[1];

ENTRY *hsearch (ENTRY item, ACTION action) {
    ENTRY *retval;
    hsearch_r(item, action, &retval, the_global_hsearch_data);
    return retval;
}

int hcreate (size_t nel) { return hcreate_r(nel, the_global_hsearch_data); }

void hdestroy (void) { hdestroy_r(the_global_hsearch_data); }

int hsearch_r (ENTRY item, ACTION action, ENTRY **retval,
               struct hsearch_data *htab)
{
    // if action==ENTER then the item is inserted
    htab->table->entry = item;
    *retval = &htab->table->entry;
}

int hcreate_r (size_t nel, struct hsearch_data *htab) {}

void hdestroy_r (struct hsearch_data *htab) {}

// custom
struct tree {
    const void *key;
};

void *tsearch (const void *key, void **rootp, __compar_fn_t compar)
{
    struct tree *t = *((struct tree **) rootp);
    compar(t->key, key);
    t->key = key;
    return (void*) t->key;
}

void *tfind (const void *key, void *const *rootp, __compar_fn_t compar)
{
    struct tree *t = *((struct tree **) rootp);
    compar(t->key, key);
    return (void*) t->key;
}

void *tdelete (const void * key, void ** rootp, __compar_fn_t compar)
{
    struct tree *t = *((struct tree **) rootp);
    compar(t->key, key);
    return (void*) t->key;
}

// typedef void (*__action_fn_t) (const void *nodep, VISIT value, int level);

void twalk (const void *root, __action_fn_t action) {
    struct tree *t = (struct tree *) root;
    action(t->key, preorder, 0);
}

// typedef void (*__free_fn_t) (void *nodep);

void tdestroy (void *root, __free_fn_t freefct) {
    struct tree *t = (struct tree *) root;
    freefct((void*) t->key);
}

void *lfind (const void *key, const void *base, size_t *nmemb, size_t size, __compar_fn_t compar)
{
    compar(base, key);
    return (void*) base;
}

void *lsearch (const void *key, void *base, size_t *nmemb, size_t size, __compar_fn_t compar)
{
    compar(base, key);
    memcpy((void*) ((char*)base + *nmemb++ * size), key, size);
    return base;
}
