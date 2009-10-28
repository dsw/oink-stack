// see License.txt for copyright and terms of use

// quarl 2006-06-16
//    testing for alloc.h

#include <stdio.h>

#include "alloc.h"

struct S1 { char x[21]; } __attribute__((packed));

struct S2 { char x; } __attribute__((packed));

void test_Struct() {
  typedef Struct<S1,S2> S;
  S s;
  printf("s = %p\n", &s);

  S1 *t1 = &s.t1;
  S2 *t2 = &s.t2;
  printf("t1 = %p\n", t1);
  printf("t2 = %p\n", t2);

  xassert(S::getStructFrom2(t2) == &s);
  xassert(S::getStructFrom1(t1) == &s);

  printf("t1 = %p\n", &S::getStructFrom2(t2)->t1 );
  printf("t2 = %p\n", &S::getStructFrom1(t1)->t2 );
}

int na = 0;

struct A0 {
  int a, k;
  size_t sz() const { return sizeof(*this) + sizeof(int)*k; }
};

template <int K>
struct A : A0 {
  int pad[K];
  A() { a = ++na; k = K; }
};

void test_alloc1()
{
  printf("*** test_alloc1\n");
  IterableBlockAllocator alloc;
  A0 *a100 = alloc.alloc< A <100> >();
  A0 *a101 = alloc.alloc< A <101> >();
  A0 *a102 = alloc.alloc< A <102> >();

  printf("a100 = %p, a100->a = %d, a100->k = %d\n", a100, a100->a, a100->k);
  printf("a101 = %p, a101->a = %d, a101->k = %d\n", a101, a101->a, a101->k);
  printf("a102 = %p, a102->a = %d, a102->k = %d\n", a102, a102->a, a102->k);

  printf("alloc.getAllocCount() = %d\n", alloc.getAllocCount());
  xassert(alloc.getAllocCount() == 3);

  printf("Iter:\n");
  // size_t sz;
  for (IterableBlockAllocator::Iter iter(alloc); !iter.isDone(); iter.adv())
  {
    A0 *a = iter.data<A0>();
    printf("  a = %p, a->a = %d, a->k = %d, a->sz = %d\n", a, a->a, a->k, a->sz());
    // sz = a->sz();
  }
}

void test_alloc2()
{
  printf("*** test_alloc2\n");

  IterableBlockAllocator alloc;
  static const int COUNT = 10*1024*1024;

  for (int i = 0; i < COUNT; ++i) {
    char x = (char) ((unsigned char) i % 256);
    char *c = alloc.alloc<char>();
    *c = x;
  }

  int i = 0;
  for (IterableBlockAllocator::Iter iter(alloc); !iter.isDone(); iter.adv())
  {
    char x = (char) ((unsigned char) i % 256);
    char *c = iter.data<char>();
    xassert(*c == x);
    ++i;
  }
  xassert(i == COUNT);
}

int main()
{
  test_Struct();
  test_alloc1();
  test_alloc2();
}

// Local Variables:
// kc-compile-command-make: "make alloc_test && ./alloc_test"
// End:
