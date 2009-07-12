// see License.txt for copyright and terms of use

// Test code for union_find.

#include "Lib/union_find.h"
#include "xassert.h"

struct A {
  int x;
  A(int x0) : x(x0) {}
};

UnionFind<A> ufa(NULL);


A **makeData(int size) {
  A **a = new A*[size];
  for (int i=0; i<size; ++i) {
    a[i] = new A(i);
    xassert(ufa.find(a[i]) == a[i]);
  }
  return a;
}

// note: size must be a power of 2
void check(int size, bool checkAsGo) {
  A **a = makeData(size);
  // union bottom up
  for (int j=1; j<size; j*=2) {
    for (int i=0; i<size; i+=(2*j)) {
      if (checkAsGo) xassert(!ufa.equiv(a[i], a[i+j]));
      ufa.uni(a[i], a[i+j]);
      if (checkAsGo) xassert(ufa.equiv(a[i], a[i+j]));
    }
  }
  // check all were unioned
  for (int i=0; i<size; ++i) {
    xassert(ufa.find(a[i]) == a[0]);
  }

  a = makeData(size);
  // union top down
  for (int j=size/2; j>0; j/=2) {
    for (int i=0; i<size; i+=(2*j)) {
      if (checkAsGo) xassert(!ufa.equiv(a[i], a[i+j]));
      ufa.uni(a[i], a[i+j]);
      if (checkAsGo) xassert(ufa.equiv(a[i], a[i+j]));
    }
  }
  // check all were unioned
  for (int i=0; i<size; ++i) {
    xassert(ufa.find(a[i]) == a[0]);
  }
}

int main() {
  check(16, false);
  check(16, true);
  return 0;
}
