// strobjdict.h            see license.txt for copyright and terms of use
// dictionary of objects, indexed by string (case-sensitive)
// (c) Scott McPeak, 2000
// NOTE: automatically generated from xstrobjdict.h -- do not edit directly

// quarl 2006-06-08
//    created xstrobjdict.h to generate strobjdict.h, strsobjdict.h, and new
//    file strintdict.h

#ifndef STROBJDICT_H
#define STROBJDICT_H

#include "svdict.h"    // StringVoidDict

void qsortStringArray(char const **strings, int size); // strutil.h

// since the dictionary does not own the pointed-to objects,
// it has the same constness model as StringVoidDict, namely
// that const means the *mapping* is constant but not the
// pointed-to objects

template <class T>
class StringObjDict {
public:     // types
  // 'foreach' iterator functions
  typedef bool (*ForeachCFn)(string const &key, T const * value, void *extra);
  typedef bool (*ForeachFn)(string const &key, T * /*serf*/ value, void *extra);

  // external iterator
  class Iter {
  private:
    StringVoidDict::IterC iter;

  public:
    Iter(StringObjDict const &dict) : iter(dict.dict) {}
    Iter(Iter const &obj) : DMEMB(iter) {}
    Iter& operator= (Iter const &obj) { CMEMB(iter); return *this; }

    bool isDone() const { return iter.isDone(); }
    Iter& next() { iter.next(); return *this; }

    string const &key() const { return iter.key(); }
    T const * &value() const { return (T const * &)iter.value(); }

    int private_getCurrent() const { return iter.private_getCurrent(); }
  };
  friend class Iter;
  typedef Iter IterC;

  class SortedKeyIter {
  private:     // data
    // underlying iterator state
    StringObjDict<T> const &map;
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
    SortedKeyIter(StringObjDict<T> const &map0)
      : map(map0)
      , keyIndex(0)
      , numEntries(map.size())
      , sortedKeys(new char const *[numEntries])
    {
      int i = 0;
      // delegate to the other Iter class
      for(IterC iter(map); !iter.isDone(); iter.next()) {
//          xassert(i<numEntries);
        sortedKeys[i++] = iter.key().c_str();
      }
      xassert(numEntries == i);
      ::qsortStringArray(sortedKeys, numEntries);
    }
    ~SortedKeyIter() {
      delete [] sortedKeys;
    }

    bool isDone() const   { return keyIndex == numEntries; }
    SortedKeyIter &next() { ++keyIndex; return *this; }

    // return information about the currently-referenced table entry

    // dsw: I already have a char const* so I think it is a mistake to
    // wrap a string around it
    char const *key() const {
      char const *key = sortedKeys[keyIndex];
//        xassert(map.isMapped(key));
      return key;
    }
    T const * value() const {
      return (T const * )map.queryfC(key());
    }
  };

private:    // data
  // underlying dictionary functionality
  StringVoidDict dict;

public:     // funcs
  StringObjDict() : dict() {}
  ~StringObjDict() { empty(); }

private:    // funcs
  // disallowed
  StringObjDict(StringObjDict const &obj);
  StringObjDict& operator= (StringObjDict const &obj);
  bool operator== (StringObjDict const &obj) const;

  // due to similarity with StringVoidDict, see svdict.h for
  // details on these functions' interfaces

public:
  // ------- selectors ---------
  int size() const                                     { return dict.size(); }

  bool isEmpty() const                                 { return dict.isEmpty(); }
  bool isNotEmpty() const                              { return !isEmpty(); }

  bool queryC(char const *key, T const * &value) const  { return dict.query(key, (void*&)value); }
  bool query(char const *key, T * &value)               { return queryC(key, (T const * &)value); }

  T const * queryfC(char const *key) const              { return (T const *)dict.queryf(key); }
  T * /*serf*/ queryf(char const *key)                 { return (T *)dict.queryf(key); }
  T * /*serf*/ queryif(char const *key)                { return (T *)dict.queryif(key); }

  bool isMapped(char const *key) const                 { return dict.isMapped(key); }

  // -------- mutators -----------
  void add(char const *key, T * value)                  { dict.add(key, (void*)value); }
  T * /*owner*/ remove(char const *key)                { return (T *)dict.remove(key); }
  void deleteAt(char const *key)                       { deleteObject(remove(key)); }

  void empty()               { dict.emptyAndDel((StringVoidDict::DelFn)deleteObject); }

  // -------- parallel interface for 'rostring' --------
  bool query(rostring key, T * &value) const { return query(key.c_str(), value); }
  T * queryf(rostring key) const             { return queryf(key.c_str()); }
  T * queryif(rostring key) const            { return queryif(key.c_str()); }
  bool isMapped(rostring key) const         { return isMapped(key.c_str()); }
  void add(rostring key, T * value)          { dict.add(key, (void*)value); }
  T * modify(rostring key, T * newValue)      { return modify(key.c_str(), newValue); }
  T * remove(rostring key)                   { return remove(key.c_str()); }

  // --------- iters -------------
  void foreachC(ForeachCFn func, void *extra=NULL) const
    { dict.foreach((StringVoidDict::ForeachFn)func, extra); }
  void foreach(ForeachFn func, void *extra=NULL)
    { dict.foreach((StringVoidDict::ForeachFn)func, extra); }

  // ------------ misc --------------
  static void deleteObject(T * obj);
  // debugging
  int private_getTopAddr() const { return dict.private_getTopAddr(); }
};

template <class T>
STATICDEF void StringObjDict<T>::deleteObject(T * obj)
{
  delete obj;
}

#endif // STROBJDICT_H
