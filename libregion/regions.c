/* see License.txt for copyright and terms of use */

/* Idea: clear on page alloc rather than individual alloc
   Turns out not so good (on lcc at least, seems a wash on mudlle):
   logically should be bad for small regions (less than a few pages)
*/
#undef PRECLEAR
#undef REGION_PROFILE

// We use Valgrind's mempool macros to enable debugging with Valgrind.  To
// turn off these low-overhead helpers, #define NVALGRIND.
#define VALGRIND 1

#if defined VALGRIND
# define REDZONE 8
# undef NVALGRIND
#else
# define REDZONE 0
# define NVALGRIND 1
#endif
#include "libregion/valgrind.h"
#include "libregion/memcheck.h"

#include "libregion/cqual-stdint.h"
#include "libregion/regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

long num_regions_created = 0;
long num_regions_active = 0;

void regions_print_stats(void)
{
  printf("regions created: %lu, regions active: %lu\n",
         num_regions_created, num_regions_active);
}

// #define REGION_DEBUG_WITH_MALLOC 1
#undef REGION_DEBUG_WITH_MALLOC

// #define REGION_DEBUG_WITH_MALLOC_OPT 1
#undef REGION_DEBUG_WITH_MALLOC_OPT

// If REGION_DEBUG_WITH_MALLOC is defined, then defeat libregions and use a
// slow-and-simple malloc implementation for everything.  Useful for debugging
// with memory checkers and heap profilers such as valgrind.
#if defined REGION_DEBUG_WITH_MALLOC

struct region_ {
  struct memnode *node;
};

// If, additionally, REGION_DEBUG_WITH_MALLOC_OPT is defined, then use a more
// compact but less protective implementation.
#if defined REGION_DEBUG_WITH_MALLOC_OPT

struct memnode {
  struct memnode *next;
};

void *debug_ralloc_using_malloc(region r, size_t size)
{
#define PTRSIZE (sizeof(void*))
  void *mem = xmalloc(size+PTRSIZE);
  assert(mem);
  memset(mem+PTRSIZE, 0, size);

  struct memnode *node = (struct memnode*) mem;
  node->next = r->node;
  r->node = node;
  return mem+PTRSIZE;
}

void deleteregion(region r)
{
  struct memnode *node = r->node;
  while (node) {
    struct memnode *next = node->next;
    free(node);
    node = next;
  }
  free(r);
}

#else

struct memnode {
  void *mem;
  struct memnode *next;
};

// pass-through malloc wrapper for allocations that aren't made by the
// application, for profiling purposes.
void *debug_ralloc_heap_admin(size_t size)
{
  return xmalloc(size);
}

void *debug_ralloc_using_malloc(region r, size_t size)
{
  void *mem = xmalloc(size);
  assert(mem);
  memset(mem, 0, size);

  struct memnode *node = debug_ralloc_heap_admin(sizeof(struct memnode));
  assert(node);
  node->mem = mem;
  node->next = r->node;
  r->node = node;

  return mem;
}

void deleteregion(region r)
{
  struct memnode *node = r->node;
  while (node) {
    struct memnode *next = node->next;
    assert(node->mem);
    free(node->mem);
    free(node);
    node = next;
  }
  free(r);
}

#endif

region newregion(void)
{
  region r = xmalloc(sizeof(struct region_));
  r->node = NULL;
  return r;
}

void *__rc_ralloc_small0(region r, size_t size)
{
  return debug_ralloc_using_malloc(r, size);
}

char *__rc_rstralloc(region r, size_t size)
{
  return debug_ralloc_using_malloc(r, size);
}

char *__rc_rstrdup(region r, const char *s)
{
  char *news = __rc_rstralloc(r, strlen(s) + 1);
  strcpy(news, s);
  return news;
}

void *__rc_typed_ralloc(region r, size_t size, type_t t)
{
  return debug_ralloc_using_malloc(r, size);
}

void *__rc_typed_rarrayalloc(region r, size_t n, size_t size, type_t t)
{
  return debug_ralloc_using_malloc(r, n * size);
}

void region_init(void) {}

void region_cleanup(void) {}

nomem_handler nomem_h;
nomem_handler set_nomem_handler(nomem_handler newhandler)
{
  nomem_handler oldh = nomem_h;

  nomem_h = newhandler;

  return oldh;
}

void out_of_memory()
{
  if (nomem_h)
    nomem_h();
  write(2, "out of memory\n", 14);
  abort();
}

#else // REGION_DEBUG_WITH_MALLOC

#define RPAGESIZE (1 << RPAGELOG)
#define K 2
#define MAXPAGE (1ULL << (RMAXMEMLOG - RPAGELOG))

/* Log of the number of pages */
#  define RMAXPAGELOG (RMAXMEMLOG - RPAGELOG)

#if SIZEOF_VOIDP <= 4

/* Two level page table: */

/* Size of the level 1 page table */
#  define RPAGELOG1 (RMAXPAGELOG/2)
#  define RPAGETABLESIZE1 (1ULL << RPAGELOG1)

/* Size of each level 2 page table */
#  define RPAGELOG2 (RMAXPAGELOG - RPAGELOG1)
#  define RPAGETABLESIZE2 (1ULL << RPAGELOG2)

#else

/* Three level page table: */

/* Size of the level 1 page table */
#  define RPAGELOG1 (RMAXPAGELOG/3)
#  define RPAGETABLESIZE1 (1ULL << RPAGELOG1)

/* Size of the level 2 page table */
#  define RPAGELOG2 (RMAXPAGELOG/3)
#  define RPAGETABLESIZE2 (1ULL << RPAGELOG2)

/* Size of each level 3 page table */
#  define RPAGELOG3 (RMAXPAGELOG - RPAGELOG1 - RPAGELOG2)
#  define RPAGETABLESIZE3 (1ULL << RPAGELOG3)

#endif /* SIZEOF_VOIDP <= 4 */

#define PAGENB(x) (((__rcintptr)(x) >> RPAGELOG) & \
                   ((1ULL << (RMAXMEMLOG - RPAGELOG)) - 1))

#define ALIGN(x, n) (((x) + ((n) - 1)) & ~((n) - 1))
#define PALIGN(x, n) ((void *)ALIGN((__rcintptr)(x), n))
#ifdef __GNUC__
#define RALIGNMENT __alignof(double)
#define PTRALIGNMENT __alignof(void *)
#define ALIGNMENT_LONG __alignof(unsigned long)
#else
#define RALIGNMENT 8
#define PTRALIGNMENT 4
#define ALIGNMENT_LONG 4
#endif

typedef intptr_t __rcintptr;

struct ablock {
  char *end, *allocfrom;
};

struct allocator {
  struct ablock page;
  struct ablock superpage;
  struct ablock hyperpage;
  struct page *pages;
  struct page *bigpages;
};

// Region has links to parent, children, and siblings in both directions.
// Siblings need to be doubly-linked for fast unlink_region().
struct region_ {
  struct allocator normal;
  region parent, sibling, prevsibling, children;
};

nomem_handler nomem_h;

region permanent = NULL;

static inline void clear(void *start, __rcintptr size)
{
  long *clear, *clearend;

  clear = (long *)start;
  clearend = (long *)((char *)start + size);
  while (clear < clearend) {
    *clear++ = 0;
  }
  // This is WRONG when size==0:
  //
  // do *clear++ = 0;
  // while (clear < clearend) ;
}

#ifdef PRECLEAR
#define preclear clear
#define postclear(s, e) ((void)0)
#else
#define preclear(s, e) ((void)0)
#define postclear clear
#endif

#include "libregion/pages.c"
#include "libregion/alloc.c"
#include "libregion/stats.c"

static void nochildren(region r)
{
  if (r->children) {
    fprintf(stderr, "Fatal error: trying to destroy region %p, but still has children:\n", r);
    region s = r->children;
    while (s) {
      fprintf(stderr, "  subregion %p\n", s);
      s = s->sibling;
    }
    abort();
  }
}

static void unlink_region(region r)
{
  // region *scan;

  // scan = &r->parent->children;
  // while (*scan != r)
  //   scan = &(*scan)->sibling;
  // *scan = (*scan)->sibling;

  if (r->prevsibling) {
    r->prevsibling->sibling = r->sibling;
  } else {
    assert(r->parent->children == r);
    r->parent->children = r->sibling;
  }

  if (r->sibling) {
    r->sibling->prevsibling = r->prevsibling;
  }
}

static void link_region(region r, region parent)
{
  r->sibling = parent->children;
  if (r->sibling) {
    r->sibling->prevsibling = r;
  }
  r->prevsibling = NULL;
  r->parent = parent;
  parent->children = r;
}

#define STAGGER_RSTART 1

#ifdef STAGGER_RSTART
static int rstart = 0;
#else
#define rstart 0
#endif

void initregion(region r)
{
  char *first =
    (char *)r - rstart - PAGE_HEADER_SIZE;

  /* Start using page with region header as a pointer-containing page */
  r->normal.page.end = first + RPAGESIZE;
  r->normal.page.allocfrom = (char *)(r + 1);

  // NOTE: [r->normal.page.allocfrom, r->normal.page.end) should be
  // valgrind-marked NOACCESS from alloc_single_page()

  /* Guarantee failure for all other blocks */
  r->normal.superpage.allocfrom = (char *)(K * RPAGESIZE + 1);
  r->normal.hyperpage.allocfrom = (char *)(K * K * RPAGESIZE + 1);

  /* Remember that r owns this page. */
  r->normal.pages = (struct page *)first;

  set_region(r->normal.pages, 1, r);

  // r->normal.bigpages = NULL;
  // r->sibling = NULL;
  // r->parent = NULL;
  // r->children = NULL;
}

region newregion(void)
{
  return newsubregion(permanent);
}

region newsubregion(region parent)
{
  // fprintf(stderr, "## newsubregion\n");
  char *first;
  region r;

  first = (char *)alloc_single_page(NULL);
  preclear(first + PAGE_HEADER_SIZE, RPAGESIZE - PAGE_HEADER_SIZE);

#ifdef STAGGER_RSTART
  /* stagger regions across cache lines a bit */
  rstart += 64;
#if RPAGESIZE < 1024
#error RPAGESIZE must be at least 1024, or change the next if.
#endif
  if (rstart >= 16 * 64) rstart = 0;
#endif
  r = (region)(first + rstart + PAGE_HEADER_SIZE);
  VALGRIND_MAKE_WRITABLE(r, sizeof(*r));
  postclear(r, sizeof *r);
  initregion(r);

  if (parent)
    link_region(r, parent);

  // fprintf(stderr, "## create mempool %p\n", r);
  VALGRIND_CREATE_MEMPOOL(r, 0, 0);
  ++num_regions_active;
  ++num_regions_created;
  return r;
}

inline char *__rc_rstralloc(region r, size_t size)
{
  void *mem, *dummy;

  record_alloc(size);

  qalloc(r, &r->normal, &dummy, 0, 1, &mem, size, RALIGNMENT, 0);
  // fprintf(stderr, "## __rc_rstralloc: r=%p, mem=%p, size=%d\n", r, mem, size);
  // VALGRIND_DO_QUICK_LEAK_CHECK;
  VALGRIND_MEMPOOL_ALLOC(r, mem, size);

  return mem;
}

inline char *__rc_rstralloc0(region r, size_t size)
{
  char *mem;

  mem = __rc_rstralloc(r, size);
  clear(mem, size);

  return mem;
}

char *__rc_rstrdup(region r, const char *s)
{
  char *news = __rc_rstralloc(r, strlen(s) + 1);

  strcpy(news, s);

  return news;
}

inline static
char *internal_rstrextend(region r, const char *old, size_t newsize,
			  int needsclear)
{
  /* For now we don't attempt to extend the old storage area */
  void *newmem, *hdr;
  unsigned long *oldhdr, oldsize;

  record_alloc(newsize);

  qalloc(r, &r->normal, &hdr, sizeof(unsigned long), ALIGNMENT_LONG,
	 &newmem, newsize, RALIGNMENT, 0);

  VALGRIND_MEMPOOL_ALLOC(r, newmem, newsize);

  /* If we don't do this we can't find the header: */
  hdr = (char *)newmem - sizeof(unsigned long);

  *(unsigned long *)hdr = newsize;

  if (old)
    {
      oldhdr = (unsigned long *)(old - ALIGNMENT_LONG);
      oldsize = *oldhdr;

      if (oldsize > newsize)
	oldsize = newsize;
      else if (needsclear)
	clear((char *)newmem + oldsize, newsize - oldsize);
      memcpy(newmem, old, oldsize);
    }
  else if (needsclear)
    clear(newmem, newsize);

  return newmem;
}

inline
char *__rc_rstrextend(region r, const char *old, size_t newsize)
{
  return internal_rstrextend(r, old, newsize, 0);
}

inline
char *__rc_rstrextend0(region r, const char *old, size_t newsize)
{
  return internal_rstrextend(r, old, newsize, 1);
}

inline void *__rc_typed_ralloc(region r, size_t size, type_t t)
{
  return __rc_rstralloc0(r, size);
}

void *__rc_ralloc_small0(region r, size_t size)
{
  char *mem2;

  mem2 = PALIGN(r->normal.page.allocfrom, RALIGNMENT);
  if (mem2 + size >= r->normal.page.end)
    return __rc_typed_ralloc(r, size, 0);

  record_alloc(size);

  r->normal.page.allocfrom = mem2 + size + REDZONE;
  // VALGRIND_DO_QUICK_LEAK_CHECK;
  VALGRIND_MEMPOOL_ALLOC(r, mem2, size);
  postclear(mem2, size);

  return mem2;
}

void *__rc_typed_rarrayextend(region r, void *old, size_t n, size_t size, type_t t)
{
  return __rc_rstrextend0(r, old, n * size);
}

void *__rc_typed_rarrayalloc(region r, size_t n, size_t size, type_t t)
{
  return __rc_typed_ralloc(r, n * size, t);
}

void typed_rarraycopy(void *to, void *from, size_t n, size_t size, type_t type)
{
  memcpy(to, from, n * size);
}

static void delregion(region r)
{
  nochildren(r);
  VALGRIND_DESTROY_MEMPOOL(r);
  VALGRIND_MAKE_WRITABLE(r, sizeof(struct page));
  free_all_pages(r, &r->normal);
  // VALGRIND_DO_QUICK_LEAK_CHECK;
  // VALGRIND_FREELIKE_BLOCK(r, 0);
  // fprintf(stderr, "##delregion: r->normal.page.end = %p\n", r->normal.page.end);
  // fprintf(stderr, "##           r+1 = %p\n", r+1);
  if (r->normal.page.end) {
    // VALGRIND_MAKE_NOACCESS((char*)(r+1), r->normal.page.end - (char*)(r+1));
  }
  // VALGRIND_MAKE_NOACCESS(r, sizeof(*r));
  --num_regions_active;
}

void deleteregion(region r)
{
  unlink_region(r);
  delregion(r);
}

void deleteregion_ptr(region *r)
{
  region tmp = *r;

  *r = NULL;
  deleteregion(tmp);
}

void deleteregion_array(int n, region *regions)
{
  int i;

  for (i = 0; i < n; i++)
    unlink_region(regions[i]);

  for (i = 0; i < n; i++)
    {
      delregion(regions[i]);
      regions[i] = NULL;
    }
}

region regionof(void *ptr)
{
  return page_region((__rcintptr)ptr >> RPAGELOG);
}

void region_init(void)
{
  // VALGRIND_DO_QUICK_LEAK_CHECK;
  // fprintf(stderr, "## region_init\n");
  static int initialized = 0;

  if ( initialized )
    return;

  else
    {
#ifdef STAGGER_RSTART
      rstart = -64; /* Save 64 bytes of memory! (sometimes ;-)) */
#endif
      init_pages();
      permanent = newregion();
      if (getenv("REGIONSTATS"))
	benchmark_init();
#ifdef DEBUG_RALLOC
      atexit(memusage);
#endif
    }
  initialized = 1;
  // VALGRIND_DO_QUICK_LEAK_CHECK;
}

void region_cleanup(void)
{
  // fprintf(stderr, "## region_cleanup\n");
  // VALGRIND_DO_QUICK_LEAK_CHECK;
  if (permanent) {
    // Call delregion() rather than deleteregion() because we don't have
    // siblings to unlink from.
    delregion(permanent);
    permanent = NULL;
  }
}

nomem_handler set_nomem_handler(nomem_handler newhandler)
{
  nomem_handler oldh = nomem_h;

  nomem_h = newhandler;

  return oldh;
}

void out_of_memory()
{
  if (nomem_h)
    nomem_h();
  if (write(2, "out of memory\n", 14) == -1) {
    /* Do nothing, about to abort anyway */
  }
  abort();
}

/*
int region_main(int argc, char **argv, char **envp);

int main(int argc, char **argv, char **envp)
{
  region_init();
  return region_main(argc, argv, envp);
}
*/

/* Debugging support */

static FILE *out;

static void printref(void *x)
{
  if (x >= (void *)__rcregionmap && x < (void *)&__rcregionmap[MAXPAGE])
    return;

#ifdef RCPAIRS
  if (x >= (void *)__rcregions && x < (void *)&__rcregions[MAXREGIONS])
    return;

#endif

  fprintf(out, "info symbol 0x%p\n", x);
}

void findrefs(region r, void *from, void *to)
{
  char *f;

  if (!out)
    out = fopen("/dev/tty", "w");

  for (f = PALIGN(from, PTRALIGNMENT); f < (char *)to; f += PTRALIGNMENT)
    if (regionof(*(void **)f) == r)
      printref(f);

  fflush(out);
}

#if defined(__GNUC__) && defined(sparc)
/* This code breaks some version of sun's cc at least */
extern void _DYNAMIC, _end;

void findgrefs(region r)
{
  findrefs(r, &_DYNAMIC, &_end);
}
#endif

void findrrefs(region r, region from)
{
  struct page *p;

  for (p = from->normal.pages; p; p = p->next)
    findrefs(r, (char *)&p->previous, (char *)p + RPAGESIZE);

  for (p = r->normal.bigpages; p; p = p->next)
    findrefs(r, (char *)&p->previous, (char *)p + p->pagecount * RPAGESIZE);
}

#endif //REGION_DEBUG_WITH_MALLOC
