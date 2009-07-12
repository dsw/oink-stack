// ptrintmap.h            see license.txt for copyright and terms of use
// map from KEY* to int for arbitrary type KEY
// (key is not owned by the table)

// dsw: modified from ptrmap.h by replacing 'VALUE*' with 'DATAVALUE'.
// this is an abuse of VoidPtrMap as it only works with types
// DATAVALUE that are the same size as a 'void*', such as 'int'; I
// think VoidPtrMap should be generalized to allow this usage.

// for const purposes, I regard the mapping itself as the only
// thing that cannot be modified in a "const" map

#ifndef PTRINTMAP_H
#define PTRINTMAP_H

#include "vptrmap.h"       // VoidPtrMap
#include "typ.h"           // NULL


template <class KEY, class DATAVALUE>
class PtrIntMap {
private:     // data
  // underlying map implementation, around which this class
  // is a type-safe wrapper
  VoidPtrMap map;

public:      // funcs
  PtrIntMap()                         : map() {}
  ~PtrIntMap()                        {}

  // query # of mapped entries
  int getNumEntries() const        { return map.getNumEntries(); }
  bool isEmpty() const             { return getNumEntries() == 0; }
  bool isNotEmpty() const          { return !isEmpty(); }

  // if this key has a mapping, return it; otherwise, return NULL
  DATAVALUE get(KEY const *key) const { return reinterpret_cast<DATAVALUE>(map.get((void const*)key)); }

  // add a mapping from 'key' to 'value'; replaces existing
  // mapping, if any
  void add(KEY *key, DATAVALUE value) { map.add((void*)key, (void*)value); }

  // remove all mappings
  void empty()                     { map.empty(); }


public:      // iterators
  class Iter {
  private:     // data
    // underlying iterator state
    VoidPtrMap::Iter iter;

  public:      // fucs
    Iter(PtrIntMap<KEY,DATAVALUE> const &map)   : iter(map.map) {}
    ~Iter()                              {}

    bool isDone() const            { return iter.isDone(); }
    void adv()                     { return iter.adv(); }

    // return information about the currently-referenced table entry
    KEY *key() const               { return (KEY*)iter.key(); }
    DATAVALUE value() const        { return reinterpret_cast<DATAVALUE>(iter.value()); }
  };
  friend class Iter;
};


#endif // PTRINTMAP_H
