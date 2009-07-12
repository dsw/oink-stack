// see License.txt for copyright and terms of use

#ifndef UNION_FIND_H
#define UNION_FIND_H

// Hashable-based implementation of the union find algorithm.

#include "ptrmap.h"
#include "xassert.h"

// Union-Find algorithm
template<class T>
class UnionFind {
  public:
  // a function for choosing between two representatives; return true
  // for the first, false for the second
  typedef bool choose_t(T *a, T *b);

  private:
  choose_t *choose;
  // the invariant is that if you are not mapped, you are your own
  // canonical rep; otherwise you map to someone closer to your
  // canonical rep
  PtrMap<T, T> *canonMap; // owned: made by us in the ctor, and unmade in the dtor
  friend class XmlValueWriter;
  friend class XmlValueReader;

  public:
  explicit UnionFind(choose_t *choose0)
    : choose(choose0)
    , canonMap(new PtrMap<T, T>)
  {}

  // this one is for de-serialization
  private:
  explicit UnionFind()
    : choose(NULL)
    , canonMap(NULL)
  {}
  public:
  static UnionFind *makeUnionFind_uninitialized() {return new UnionFind();}

  public:
  ~UnionFind() {
    xassert(canonMap);
    delete(canonMap);
  }

  private:
  UnionFind(UnionFind &x);      // prohibit

  public:
  void uni(T *src, T *tgt);     // union
  T *find(T *t);                // find
  bool equiv(T *src, T *tgt);   // are two elements equivalent?
  void import(UnionFind<T> &other); // import another UnionFind into this one
};

template<class T>
void UnionFind<T>::uni(T *src, T *tgt) {
  xassert(src);
  xassert(tgt);
  T *src0 = find(src);
  xassert(src0);
  T *tgt0 = find(tgt);
  xassert(tgt0);
  if (src0 != tgt0) {
    // NOTE: in the absence of a choose function, we choose src0
    if (choose && choose(tgt0, src0)) {
      // tgt0 chosen
      canonMap->add(src0, tgt0);
    } else {
      // src0 chosen
      canonMap->add(tgt0, src0);
    }
  }
}

template<class T>
T *UnionFind<T>::find(T *t) {
  xassert(t);
  xassert(canonMap);
  T *t0 = canonMap->get(t);
  // if we are a canonical rep, return ourself as our own rep
  if (!t0) return t;
  // otherwise, transitively get our rep
  T *t1 = find(t0);
  xassert(t1);
  // optimization: close this path in the graph so future lookups are
  // fast; this is the union-find trick that makes it
  // amortized-almost-linear-time
  canonMap->add(t, t1);
  return t1;
}

template<class T>
bool UnionFind<T>::equiv(T *src, T *tgt) {
  return find(src) == find(tgt);
}

template<class T>
void UnionFind<T>::import(UnionFind<T> &other) {
  xassert(choose == other.choose);
  for(typename PtrMap<T, T>::Iter iter(*other.canonMap); !iter.isDone(); iter.adv()) {
    uni(iter.key(), iter.value());
  }
}

#endif // UNION_FIND_H
