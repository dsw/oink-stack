m4_dnl // xstrobjdict.h            see license.txt for copyright and terms of use
m4_dnl // template file to be processed with m4 to generate one
m4_dnl // of the wrappers around VoidList
m4_dnl
m4_changequote([, ])m4_dnl      // for this section
m4_changecom[]m4_dnl            // no m4 "comments"
m4_ifelse(m4_output, strsobjdict.h, [m4_dnl
// strsobjdict.h            see license.txt for copyright and terms of use
// dictionary of *serf* pointers to objects, indexed by string (case-sensitive)
m4_define(makeName, S[$1])m4_dnl
m4_define(outputCond, [$1])m4_dnl       // select 1st arg
m4_define(SPC, [])m4_dnl
m4_define(XSTROBJDICT, [StringSObjDict])m4_dnl
m4_define(includeLatch, [STRSOBJLIST_H])m4_dnl
m4_define(TPTR, [T *])m4_dnl
m4_define(TCPTR, [T const *])m4_dnl
m4_define(outputCondTemplate, [$1])m4_dnl
], [m4_dnl
m4_ifelse(m4_output, strintdict.h, [m4_dnl
// strintlist.h            see license.txt for copyright and terms of use
// dictionary of longs (integers that fit into void*), indexed by string
// (case-sensitive)
m4_define(makeName, S[$1])m4_dnl
m4_define(outputCond, [$1])m4_dnl       // select 1st arg
m4_define(SPC, [ ])m4_dnl
m4_define(XSTROBJDICT, [StringIntDict])m4_dnl
m4_define(includeLatch, [STRINTDICT_H])m4_dnl
m4_define(TPTR, [long])m4_dnl
m4_define(TCPTR, [long])m4_dnl
m4_define(outputCondTemplate, [$2])m4_dnl
], [m4_dnl
// strobjdict.h            see license.txt for copyright and terms of use
// dictionary of objects, indexed by string (case-sensitive)
m4_define(makeName, [$1])m4_dnl
m4_define(outputCond, [$2])m4_dnl       // select 2nd arg
m4_define(SPC, [ ])m4_dnl               // for balancing lined-up comments
m4_define(XSTROBJDICT, [StringObjDict])m4_dnl
m4_define(includeLatch, [STROBJDICT_H])m4_dnl
m4_define(TPTR, [T *])m4_dnl
m4_define(TCPTR, [T const *])m4_dnl
m4_define(outputCondTemplate, [$1])m4_dnl
])m4_dnl
])m4_dnl
m4_dnl m4_define(XSTROBJDICT_type, XSTROBJDICT[]_type)m4_dnl
m4_define(XSTROBJDICT_type, outputCondTemplate(XSTROBJDICT<T>, XSTROBJDICT))m4_dnl
m4_changequote(, )m4_dnl              // so quotes are not quoted..
m4_changequote([[[, ]]])m4_dnl        // reduce likelihood of confusion
// (c) Scott McPeak, 2000
// NOTE: automatically generated from xstrobjdict.h -- do not edit directly

// quarl 2006-06-08
//    created xstrobjdict.h to generate strobjdict.h, strsobjdict.h, and new
//    file strintdict.h

#ifndef includeLatch
#define includeLatch

#include "svdict.h"    // StringVoidDict

void qsortStringArray(char const **strings, int size); // strutil.h

outputCond([[[m4_dnl
// the dictionary object is considered to own all of the things
// contained, so constness means constness of the contained objects
// as well as the mapping from strings to them
]]],[[[m4_dnl
// since the dictionary does not own the pointed-to objects,
// it has the same constness model as StringVoidDict, namely
// that const means the *mapping* is constant but not the
// pointed-to objects
]]])m4_dnl

outputCondTemplate([[[template <class T>]]])
class XSTROBJDICT {
public:     // types
m4_dnl  typedef outputCondTemplate([[[XSTROBJDICT<T>]]], [[[XSTROBJDICT]]]) [[[XSTROBJDICT_type]]];
  // 'foreach' iterator functions
outputCond([[[m4_dnl
  typedef bool (*ForeachFn)(string const &key, TPTR value, void *extra);
]]],[[[m4_dnl
  typedef bool (*ForeachCFn)(string const &key, TCPTR value, void *extra);
  typedef bool (*ForeachFn)(string const &key, TPTR /*serf*/ value, void *extra);
]]])m4_dnl

outputCond([[[m4_dnl
  // external iterator
  class Iter {
  private:
    StringVoidDict::Iter iter;

  public:
    Iter(XSTROBJDICT &dict) : iter(dict.dict) {}
    Iter(Iter const &obj) : DMEMB(iter) {}
    Iter& operator= (Iter const &obj) { CMEMB(iter); return *this; }

    bool isDone() const { return iter.isDone(); }
    Iter& next() { iter.next(); return *this; }

    string const &key() const { return iter.key(); }
    TPTR &value() const { return (TPTR &)iter.value(); }

    int private_getCurrent() const { return iter.private_getCurrent(); }
  };
  friend class Iter;

  class IterC {
  private:
    StringVoidDict::IterC iter;

  public:
    IterC(XSTROBJDICT const &dict) : iter(dict.dict) {}
    IterC(IterC const &obj) : DMEMB(iter) {}
    IterC& operator= (IterC const &obj) { CMEMB(iter); return *this; }

    bool isDone() const { return iter.isDone(); }
    IterC& next() { iter.next(); return *this; }

    string const &key() const { return iter.key(); }
    TPTR value() const { return (TPTR)iter.value(); }

    int private_getCurrent() const { return iter.private_getCurrent(); }
  };
  friend class IterC;
]]],[[[m4_dnl
  // external iterator
  class Iter {
  private:
    StringVoidDict::IterC iter;

  public:
    Iter(XSTROBJDICT const &dict) : iter(dict.dict) {}
    Iter(Iter const &obj) : DMEMB(iter) {}
    Iter& operator= (Iter const &obj) { CMEMB(iter); return *this; }

    bool isDone() const { return iter.isDone(); }
    Iter& next() { iter.next(); return *this; }

    string const &key() const { return iter.key(); }
    TCPTR &value() const { return (TCPTR &)iter.value(); }

    int private_getCurrent() const { return iter.private_getCurrent(); }
  };
  friend class Iter;
  typedef Iter IterC;
]]])m4_dnl

  class SortedKeyIter {
  private:     // data
    // underlying iterator state
    XSTROBJDICT_type const &map;
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
    SortedKeyIter(XSTROBJDICT_type const &map0)
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
    TCPTR value() const {
      return (TCPTR )map.queryfC(key());
    }
  };

private:    // data
  // underlying dictionary functionality
  StringVoidDict dict;

public:     // funcs
  XSTROBJDICT[[[]]]() : dict() {}
outputCond([[[m4_dnl
  ~XSTROBJDICT[[[]]]() {}
]]],[[[m4_dnl
  ~XSTROBJDICT[[[]]]() { empty(); }
]]])m4_dnl

outputCond([[[m4_dnl
public:    // funcs
  XSTROBJDICT[[[]]](XSTROBJDICT const &obj) : dict(obj.dict) {}

  // comparison and assignment use *pointer* comparison/assignment

  XSTROBJDICT& operator= (XSTROBJDICT const &obj)    { dict = obj.dict; return *this; }

  bool operator== (XSTROBJDICT const &obj) const        { return dict == obj.dict; }
  NOTEQUAL_OPERATOR(XSTROBJDICT)
]]],[[[m4_dnl
private:    // funcs
  // disallowed
  XSTROBJDICT[[[]]](XSTROBJDICT const &obj);
  XSTROBJDICT& operator= (XSTROBJDICT const &obj);
  bool operator== (XSTROBJDICT const &obj) const;
]]])m4_dnl

  // due to similarity with StringVoidDict, see svdict.h for
  // details on these functions' interfaces

public:
  // ------- selectors ---------
  int size() const                                     { return dict.size(); }

  bool isEmpty() const                                 { return dict.isEmpty(); }
  bool isNotEmpty() const                              { return !isEmpty(); }

outputCond([[[m4_dnl
  bool query(char const *key, TPTR &value) const         { return dict.query(key, (void*&)value); }
  TPTR queryf(char const *key) const                     { return (TPTR)dict.queryf(key); }
  TPTR queryif(char const *key) const                    { return (TPTR)dict.queryif(key); }
  
  // parallel functions for API consistency
  bool queryC(char const *key, TPTR &value) const { return query(key, value); }
  TPTR queryfC(char const *key) const { return queryf(key); }
]]],[[[m4_dnl
  bool queryC(char const *key, TCPTR &value) const  { return dict.query(key, (void*&)value); }
  bool query(char const *key, TPTR &value)               { return queryC(key, (TCPTR &)value); }

  TCPTR queryfC(char const *key) const              { return (TCPTR)dict.queryf(key); }
  TPTR /*serf*/ queryf(char const *key)                 { return (TPTR)dict.queryf(key); }
  TPTR /*serf*/ queryif(char const *key)                { return (TPTR)dict.queryif(key); }
]]])m4_dnl

  bool isMapped(char const *key) const                 { return dict.isMapped(key); }

  // -------- mutators -----------
  void add(char const *key, TPTR value)                  { dict.add(key, (void*)value); }
  TPTR /*owner*/ remove(char const *key)                { return (TPTR)dict.remove(key); }
outputCond([[[m4_dnl
  TPTR modify(char const *key, TPTR newValue)              { return (TPTR)dict.modify(key, (void*)newValue); }

  void empty()                                         { dict.empty(); }
]]],[[[m4_dnl
  void deleteAt(char const *key)                       { deleteObject(remove(key)); }

  void empty()               { dict.emptyAndDel((StringVoidDict::DelFn)deleteObject); }
]]])m4_dnl

  // -------- parallel interface for 'rostring' --------
  bool query(rostring key, TPTR &value) const { return query(key.c_str(), value); }
  TPTR queryf(rostring key) const             { return queryf(key.c_str()); }
  TPTR queryif(rostring key) const            { return queryif(key.c_str()); }
  bool isMapped(rostring key) const         { return isMapped(key.c_str()); }
  void add(rostring key, TPTR value)          { dict.add(key, (void*)value); }
  TPTR modify(rostring key, TPTR newValue)      { return modify(key.c_str(), newValue); }
  TPTR remove(rostring key)                   { return remove(key.c_str()); }

  // --------- iters -------------
outputCond([[[m4_dnl
  void foreach(ForeachFn func, void *extra=NULL) const
    { dict.foreach((StringVoidDict::ForeachFn)func, extra); }
]]],[[[m4_dnl
  void foreachC(ForeachCFn func, void *extra=NULL) const
    { dict.foreach((StringVoidDict::ForeachFn)func, extra); }
  void foreach(ForeachFn func, void *extra=NULL)
    { dict.foreach((StringVoidDict::ForeachFn)func, extra); }
]]])m4_dnl

  // ------------ misc --------------
outputCond([[[m4_dnl
]]],[[[m4_dnl
  static void deleteObject(TPTR obj);
]]])m4_dnl
  // debugging
  int private_getTopAddr() const { return dict.private_getTopAddr(); }
};

outputCond([[[m4_dnl
]]],[[[m4_dnl
template <class T>
STATICDEF void XSTROBJDICT_type::deleteObject(TPTR obj)
{
  delete obj;
}
]]])m4_dnl

#endif // includeLatch
