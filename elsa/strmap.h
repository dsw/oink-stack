// strmap.h
// PtrMap using StringRef as key

#ifndef STRMAP_H
#define STRMAP_H

#include "strtable.h"     // StringRef
#include "ptrmap.h"       // PtrMap
#include "strutil.h"      // qsortStringArray()

// The 'KEY' argument to PtrMap is "char const" because it implicitly
// adds a "*", so the actual key is "char const *", i.e. StringRef.
// But note that the keys really have to be StringRef, not just any
// char*, because PtrMap relies on the unique-representative property.
template <class VALUE>
class StringRefMap : public PtrMap<char const, VALUE> {
  public:
  // Iterate over the map in sorted order by the keys
  class SortedKeyIter {
  private:     // data
    // underlying iterator state
    StringRefMap<VALUE> const &map;
    int keyIndex;
    // dsw: I think it is a mistake to use getNumEntries() repeatedly
    // instead of caching the value that it was at the time the
    // iterator was constructed.  While we are still not thread-safe
    // in the sense that an entry could be added or deleted, we could
    // catch that, but we still do not want to separate the array of
    // sortedKeys from its length as if these get out of synch this is
    // an impossible bug to catch and a very low-level error.  Note
    // that we still have a bit of a race condidition that numEntries
    // is initialized before we iterate over the keys, but we are
    // likely to catch that later.
    int const numEntries;
    char const **sortedKeys;    // array of strings

  public:      // fucs
    SortedKeyIter(StringRefMap<VALUE> const &map0)
      : map(map0)
      , keyIndex(0)
      , numEntries(map.getNumEntries())
      , sortedKeys(new char const *[numEntries])
    {
      int i = 0;
      // delegate to the other Iter class
      for(typename PtrMap<char const, VALUE>::Iter iter0(map); !iter0.isDone(); iter0.adv()) {
//          xassert(i<numEntries);
        sortedKeys[i++] = iter0.key();
      }
      xassert(numEntries == i);
      qsortStringArray(sortedKeys, numEntries);
    }
    ~SortedKeyIter() {
      delete [] sortedKeys;
    }

    bool isDone() const  { return keyIndex == numEntries; }
    void adv()           { ++keyIndex; }

    // return information about the currently-referenced table entry
    char const *key() const {
      char const *key0 = sortedKeys[keyIndex];
      xassert(map.get(key0));
      return key0;
    }
    VALUE *value() const { return map.get(key()); }
  };
  //   friend class SortedKeyIter;
};

#endif // STRMAP_H
