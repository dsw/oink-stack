
// quarl 2006-05-30
//    IdSObjDict is a data structure with the same interface as
//    StringSObjDict<void> (actually subset), but optimized for the case where
//    most (or all) ids are of the form "XY1234" (with the integer IDs densely
//    increasing).
//
//    It is implemented via a map from strings (2-character prefixes) to
//    arrays, where the integer ID is used directly as the array index.
//
//    This optimization reduces deserialization time by 60% and memory usage
//    by 16%.
//
//    Other IDs are supported (using the old hashtable algorithm), but when
//    parsing output produced by ourselves, we should never see IDs not of the
//    form "XY1234".

#ifndef IDOBJDICT_H
#define IDOBJDICT_H

#include "strobjdict.h"         // StringObjDict
#include "strsobjdict.h"        // StringSObjDict
#include "bitwise_array.h"      // BitwiseGrowArray

class IdSObjDict {
  class Prefix {
    typedef short int16_t;
    int16_t prefix;
  public:
    Prefix() : prefix(0) {}
    Prefix(char const *p) { set(p); }
    void set(char const *p) { prefix = *reinterpret_cast<int16_t const*>(p); }
    bool operator==(Prefix o) const { return prefix == o.prefix; }
    void selfCheck() { xassert(prefix); }
  };

  struct IdNode {
    void *object;
    Prefix prefix;
    IdNode *next;

    IdNode() { memset(this, 0, sizeof(*this)); }
  };

  // mini memory pool
  class IdNodePool {
    enum { PAGE_SIZE = 63 };
    struct Page {
      IdNode nodes[PAGE_SIZE];
      Page *next;
      Page() : next(0) {}
    };
    Page pages, *curPage;
    IdNode *nextInPage;

  public:
    IdNodePool() { curPage=&pages; nextInPage=curPage->nodes; }
    IdNode *alloc() {
      if (nextInPage == curPage->nodes+PAGE_SIZE) {
        curPage->next = new Page;
        nextInPage = curPage->nodes;
      }
      return nextInPage++;
    }
    ~IdNodePool() {
      Page *p = pages.next;
      while (p) {
        Page *next = p->next;
        delete p;
        p = next;
      }
    }

  } pool;

  // the data structure is an array of IdNodes.  Usually there is only 1
  // object per id, only rarely is there more than one prefix per id.  In that
  // case we use a linked list.

  BitwiseGrowArray<IdNode> objectsById;
  StringSObjDict<void> objectsOther;

public:
  IdSObjDict() : objectsById(64) {}
  void *queryif(char const *id);
  void *queryif(string const &id) { return queryif(id.c_str()); }
  void add(char const *id, void *obj);
  void add(string const &id, void *obj) { add(id.c_str(), obj); }
  bool isMapped(char const *id) { return queryif(id) != NULL; }

protected:
  static bool parseId(char const *id, Prefix &prefix, unsigned &idnum);
};

#endif
