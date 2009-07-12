// see License.txt for copyright and terms of use

// The word "polymorphic" is seriously overloaded in the world of
// programming languages.  Implementation of the polymorphic ("funky")
// qualifier literals syntax, such as this.
//
//     char $_1 * strchr(const char $_1 * s, int c);

#ifndef QUAL_FUNKY_H
#define QUAL_FUNKY_H

#include "qual_literal.h"
#include "sobjstack.h"

// forward
class Function;

// values need to be opaque to break the circular dependency
class Value;

// the semantics of enum static cast seem to have changed in g++
// 3.4.0, so I use an int
//  enum Value_Tag {};
typedef int Value_Tag;

class FunctionValue_Q;

// a bit-set that represents $_1_2 qualifiers
// FIX: Generalize this to a bitset.

// quarl 2006-06-23
//    Note: Although we use an int, our parent, QualAnnot, requires that only
//    the lower 29 bits are used.
class Funky {
  int bitset;
public:
  explicit Funky() : bitset(0) {}
  explicit Funky(int bitset0) : bitset(bitset0) {}
  int getRepr() const { return bitset; }

  bool empty() {return bitset==0;}
  void insert(int x);
  string toString();
  bool isSubsetOf(Funky sq2);
  bool operator == (Funky &f) {return bitset == f.bitset;}
  bool operator != (Funky &f) {return ! (*this == f);}
};

Funky parseAsFunky(char const *sqName, SourceLoc loc);
bool isFunkyQualifier(QLiterals *ql0 // as a single qualifier
                      , SourceLoc loc);
bool hasFunkyQualifiers(QLiterals *ql0 // as a set of qualifiers
                        , SourceLoc loc);
bool hasNonFunkyQualifiers(QLiterals *ql0 // as a set of qualifiers
                           , SourceLoc loc);

// An element of this class is a relative pointer from the top of a
// constructed value to an internal value node in its value tree.
class DeepValueName {
  public:
  Value_Tag tag;
  DeepValueName **meInParent;   // so we can disconnect ourselves from our parent
  DeepValueName *child;         // for function, pointer, and array
  int position;                 // for function values

  public:
  explicit DeepValueName(Value_Tag tag0,
                        DeepValueName **meInParent0,
                        DeepValueName *child0=NULL,
                        int position0=-1)
    : tag(tag0), meInParent(meInParent0), child(child0), position(position0) {
    // Don't do either of these assertions.
//      if (k == FUNCTION) xassert(position0>=0);
//      else xassert(position0==-1);
//      if (k == CVATOMIC) xassert(!child);
//      else xassert(child);
  }

  DeepValueName *deepClone();
  string toString() const;
  Value *index(Value *t) const;   // FIX: this is broken for references
};

// iterate over all deep parts of a value
class DeepValueIterator {
  struct StateLayer {
    DeepValueName *name;
    Value *value;
    StateLayer(DeepValueName *name0, Value *value0);
  };

  // we are a frozen dfs, so we need a stack for it
  SObjStack<StateLayer> stack0;
  // the current name of the iterator
  DeepValueName *name;

  private:
  DeepValueName *&topName() {return stack0.top()->name;}
  Value *&topValue() {return stack0.top()->value;}
  void pushNewChild(Value *newChild);
  // go over to the next function paramter (if there is one) and go
  // down; return true if there is a next function parameter
  bool nextFunctionParameter();

  public:
  DeepValueIterator(Value *t);
  bool isDone() {return stack0.isEmpty();}
  void adv();
  DeepValueName *currentName() {return name;}

  // NOTE: not symmetric with currentName() above
  Value *currentSubValue() {return topValue();}
};

struct DeepValueNamePair {
  DeepValueName *x;
  DeepValueName *y;

  DeepValueNamePair(DeepValueName *x0, DeepValueName *y0) : x(x0), y(y0) {}
  string toString() const;
};

class AdjList {
  ObjList<DeepValueNamePair> pairList;

  public:
  AdjList(FunctionValue_Q* ft);
  string toString();
  ObjList<DeepValueNamePair> &getPairList() {return pairList;}
  bool isEmpty() {return pairList.isEmpty();}
};

class DeepLiteral {
  public:
  DeepValueName *dtn;
  QLiterals *ql;
  explicit DeepLiteral(DeepValueName *dtn0, QLiterals *ql0) : dtn(dtn0), ql(ql0) {}
  void attach(Value *t) const;
};

class DeepLiteralsList {
  ObjList<DeepLiteral> dll;
  public:
  explicit DeepLiteralsList(Value *t);
  void attach(Value *t);
};

#endif // QUAL_FUNKY_H
