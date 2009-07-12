/* see License.txt for copyright and terms of use */

/* TBD: recover unusued portions of pages for use as individual pages */

#include "libregion/regions.h"
#include <stddef.h>

static
void alloc_block(region r, struct allocator *a, struct ablock *blk,
		 void **p1, int s1, int a1, void **p2, int s2, int a2,
		 size_t blksize, int needsclear)
{
  struct page *newp;
  char *mem1, *mem2;

  mem1 = PALIGN(blk->allocfrom, a1);
  mem2 = PALIGN(mem1 + s1, a2);

  /* Can't use last byte of page (pointers to the byte after an object are
     valid) */
  if (mem2 + s2 >= blk->end)
    {
      if (blksize == RPAGESIZE)
	{
	  newp = alloc_single_page(a->pages);
	  a->pages = newp;
	  blk->allocfrom = (char *)newp + offsetof(struct page, previous);
	  set_region(newp, 1, r);
	}
      else
	{
	  newp = alloc_pages(blksize >> RPAGELOG, a->bigpages);
	  a->bigpages = newp;
	  blk->allocfrom = (char *)newp + offsetof(struct page, previous);
	  set_region(newp, blksize >> RPAGELOG, r);
	}
      blk->end = (char *)newp + blksize;

      if (needsclear)
	preclear(blk->allocfrom, blksize - (blk->allocfrom - (char *)newp));
      mem1 = PALIGN(blk->allocfrom, a1);
      mem2 = PALIGN(mem1 + s1, a2);
    }

  ASSERT_INUSE(blk->end - blksize, r);
  blk->allocfrom = mem2 + s2;

  *p1 = mem1;
  *p2 = mem2;
}

static inline
void qalloc(region r, struct allocator *a, void **p1, int s1, int a1,
	    void **p2, int s2, int a2, int needsclear)
{
  struct page *p;
  char *mem;
  int npages;
  int n = ALIGN(s1, a2) + s2; /* Yes, this is correct (see alloc_block) */

  /* We optimise the check for the first block */
  {
    char *mem1, *mem2;

    mem1 = PALIGN(a->page.allocfrom, a1);
    mem2 = PALIGN(mem1 + s1, a2);

    /* Can't use last byte of page (pointers to the byte after an object are
       valid) */
    if (mem2 + s2 < a->page.end)
      {
	ASSERT_INUSE(blk->end - blksize, r);
	a->page.allocfrom = mem2 + s2;

	*p1 = mem1;
	*p2 = mem2;
	return;
      }
  }

  if (n <= RPAGESIZE / K)
    {
      alloc_block(r, a, &a->page, p1, s1, a1, p2, s2, a2, RPAGESIZE,
		  needsclear);
      return;
    }
#if K >= 2
  if (n <= RPAGESIZE)
    {
      alloc_block(r, a, &a->superpage, p1, s1, a1, p2, s2, a2,
		  K * RPAGESIZE, needsclear);
      return;
    }
#endif
#if K >= 4
  if (n <= RPAGESIZE * K)
    {
      alloc_block(r, a, &a->hyperpage, p1, s1, a1, p2, s2, a2,
		  K * K * RPAGESIZE, needsclear);
      return;
    }
#endif

  /* We would have - 1 after RPAGESIZE, but we need to add 1 to make the
     last byte of the object live in the same region */
  npages = (n + ALIGN(offsetof(struct page, previous), a1) + RPAGESIZE)
    >> RPAGELOG;
  p = alloc_pages(npages, a->bigpages);
  a->bigpages = p;
  set_region(p, npages, r);

  mem = (char *)p + offsetof(struct page, previous);
  *p1 = PALIGN(mem, a1);
  *p2 = PALIGN((char *)*p1 + s1, a2);
  if (needsclear)
    preclear(*p2, s2);
}

void free_all_pages(region r, struct allocator *a)
/* Assumes freepages_lock held */
{
  struct page *p, *next;

  // save a->bigpages, since after coalesce(p) we shouldn't use p
  struct page *bigpages = a->bigpages;

  for (p = a->pages; p; p = next)
    {
      next = p->next;
      free_single_page(r, p);
    }
  for (p = bigpages; p; p = next)
    {
      next = p->next;
      free_pages(r, p);
    }
}
