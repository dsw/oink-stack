// see License.txt for copyright and terms of use

// Implements 1) representing the syntax and 2) providing the
// semantics of qualifier literals such as $tainted.

#ifndef QUAL_LITERAL_H
#define QUAL_LITERAL_H

#include "str.h"
#include "strtable.h"
#include "srcloc.h"

// linked list of literals: $tainted
class QLiterals {
  public:
  StringRef name;
  QLiterals *next;

  public:
  explicit QLiterals(StringRef name, QLiterals *next = NULL);

  // just tests one for equality, not a list
  bool equal(QLiterals *ql);
  string toString() const;
  QLiterals *deepClone(QLiterals *tail = NULL);
  QLiterals *deepCloneOnlyFunky(QLiterals *tail = NULL);

  private:
  void buildString(stringBuilder &s) const;
};

// non-member useful functions ****************

QLiterals *deepClone(QLiterals *ql);
QLiterals *deepCloneOnlyFunky(QLiterals *ql);
void prependQLiterals(QLiterals *&baseQl, QLiterals *ql0);
void appendQLiterals(QLiterals *&baseQl, QLiterals *ql0);
void prependLiteralString(QLiterals *&ql, StringRef s);
void appendLiteralString(QLiterals *&ql, StringRef s);
bool in_qualifiers_list
  (QLiterals *ql0               // thought of as a single element; disregard ql0->next
   ,QLiterals *ql1
   ,SourceLoc loc
   );
bool is_subset_of_qualifiers
  (QLiterals *ql0               // thought of as a set
   ,QLiterals *ql1
   ,SourceLoc loc
   );

#endif // QUAL_LITERAL_H
