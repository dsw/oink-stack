// see License.txt for copyright and terms of use

// 2006-05-15 quarl
//   Memory pools: objects can be allocated and should be deallocated.
//   Deallocated objects go on a freelist for fast allocation next time.
//
//   If you have lots of instances of one data structure, and they generally
//   live forever but are sometimes individually deallocated, use this.
//
//   These mempools can't be "destroyed" like regions can.

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <string.h>             // memset
#include <assert.h>

#define MEMPOOL_ALLOC_SIZE 1024

struct mempool_node {
  struct mempool_node *next;
};

struct mempool {
#ifndef NDEBUG
  int objsize;
#endif
  struct mempool_node *freelist;
  char *data;
  char *end;
};

static inline
void mempool_init(struct mempool *mp, size_t objsize)
{
  assert (objsize >= sizeof(void*));
  // We don't align to word size for now - you can decide if you want this by
  // packing your structs or not.
#ifndef NDEBUG
  mp->objsize = objsize;
#endif
  mp->freelist = NULL;
  mp->data = xcalloc(MEMPOOL_ALLOC_SIZE, objsize);
  mp->end = mp->data + (MEMPOOL_ALLOC_SIZE * objsize);
}

static inline
void *mempool_alloc(struct mempool *mp, size_t objsize)
{
  // objsize must be the same as what you passed into mempool_init!
#ifndef NDEBUG
  assert(objsize == mp->objsize);
#endif
  if (mp->freelist) {
    // any free?
    struct mempool_node *p = mp->freelist;
    mp->freelist = p->next;
    memset(p, 0, objsize);
    return p;
  }

  assert (mp->end <= mp->data);
  if (mp->data + objsize > mp->end) {
    // since objsize is always the same, and we allocated an integral multiple
    // of it, shouldn't have any bytes left over!
    assert (mp->data == mp->end);
    // Malloc more?
    //
    // We're clobbering the old page, but we don't need it anymore.  We never
    // free() pages; we just add freed objects to the freelist.
    mp->data = xcalloc(MEMPOOL_ALLOC_SIZE, objsize);
    mp->end = mp->data + (MEMPOOL_ALLOC_SIZE * objsize);
  }

  char *p = mp->data;
  mp->data += objsize;
  return p;
}

static inline
void mempool_free(struct mempool *mp, void *obj)
{
  struct mempool_node *node = (struct mempool_node *)obj;
  node->next = mp->freelist;
  mp->freelist = node;
}

#endif
