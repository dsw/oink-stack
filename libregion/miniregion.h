// see License.txt for copyright and terms of use

// 2006-05-15 quarl
//   Fast and simple implementation of regions for small allocations.  Pages
//   are 120 bytes in size.  Everything is inlined.  Pages are allocated using
//   malloc(), but that is the only thing that is allocated; everything else
//   is static.

#ifndef MINIREGION_H
#define MINIREGION_H

#include <string.h>             // memset

// The bigger this number, the faster we run.  If it's too small, there's high
// overhead in the linked list; if it's too large, we waste too much unused
// space.
#define MINIREGION_SIZE (128-2*sizeof(void*))

struct miniregion_page {
  char data[MINIREGION_SIZE];
  char *end;
  struct miniregion_page *next;
};

// Using our own freelist only buys a fractional performance improvement.
// However, if we don't use a freelist, memory debugging/profiling tools work
// out-of-the-box.

// static struct miniregion_page *miniregion_freelist = NULL;

struct miniregion {
  struct miniregion_page *node;
};

static inline
void miniregion_addfree(struct miniregion_page *pool)
{
  // pool->next = miniregion_freelist;
  // miniregion_freelist = pool;
  free(pool);
}

static inline
struct miniregion_page *miniregion_getpage()
{
  // if (miniregion_freelist) {
  //   struct miniregion_page *p = miniregion_freelist;
  //   miniregion_freelist = p->next;
  //   return p;
  // } else {
    return xmalloc(sizeof(struct miniregion_page));
  // }
}

static inline
void miniregion_init(struct miniregion *pool)
{
  pool->node = miniregion_getpage();
  memset(pool->node->data, 0, MINIREGION_SIZE);
  pool->node->end = pool->node->data;
  pool->node->next = NULL;
}

static inline
void miniregion_delete(struct miniregion *pool)
{
  struct miniregion_page *p = pool->node, *next;
  while (p != NULL) {
    next = p->next;
    miniregion_addfree(p);
    p = next;
  }
#ifndef NDEBUG
  // risky optimization: pool is about to be freed (or re-initialized), so
  // don't need this.
  pool->node = NULL;
#endif
}

// equivalent to miniregion_delete(pool); miniregion_init(pool), with one less
// roundtrip to the freelist.
static inline
void miniregion_reset(struct miniregion *pool)
{
  struct miniregion_page *p = pool->node->next, *next;
  while (p != NULL) {
    next = p->next;
    miniregion_addfree(p);
    p = next;
  }
  memset(pool->node->data, 0, MINIREGION_SIZE);
  pool->node->end = pool->node->data;
  pool->node->next = NULL;
}

static inline
void *miniregion_alloc(struct miniregion *pool, size_t size)
{
  // align to word size
#define ALIGN(x, n) (((x) + ((n) - 1)) & ~((n) - 1))
  size = ALIGN(size, (sizeof(void*)));
#undef ALIGN
  assert (size < MINIREGION_SIZE);
  if (pool->node->end + size > pool->node->data + MINIREGION_SIZE)
  {
    // allocate new page
    struct miniregion_page *p = miniregion_getpage();
    memset(p->data, 0, MINIREGION_SIZE);
    p->end = p->data + size;
    p->next = pool->node;
    pool->node = p;
    return p->data;
  } else {
    char *ret = pool->node->end;
    pool->node->end += size;
    return ret;
  }
}

#endif
