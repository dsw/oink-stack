// see License.txt for copyright and terms of use

#include "qual_literal.h"       // this module
#include "qual_funky.h"
#include <cstring>              // strcmp, streq

QLiterals::QLiterals(StringRef name, QLiterals *next)
  : name(name), next(next)
{
//    cout << "Qliterals " << count0 << ":" << name << endl;
}

bool QLiterals::equal(QLiterals *ql) {
  if (!ql) return false;
  // FIX: StringRef-s should be compared with ==.
  return streq(name, ql->name);
}

string QLiterals::toString() const {
  stringBuilder s;
  if (this != NULL) {
    buildString(s);
    s << " ";                   // dsw: FIX: should be able to get rid of this.
  }
  return s;
}

QLiterals *QLiterals::deepClone(QLiterals *tail) {
  return new QLiterals(name, (next? next->deepClone(tail) : tail));
}

QLiterals *QLiterals::deepCloneOnlyFunky(QLiterals *tail) {
  QLiterals *tail0 = next? next->deepCloneOnlyFunky(tail) : tail;
  if (isFunkyQualifier(this, SL_UNKNOWN)) {
    return new QLiterals(name, tail0);
  }
  return tail0;
}

void QLiterals::buildString(stringBuilder &s) const {
  s << " " << name;
  if (next) next->buildString(s);
}

// non-member useful functions ****************

QLiterals *deepClone(QLiterals *ql) {
  if (!ql) return NULL;
  return ql->deepClone(NULL);
}

QLiterals *deepCloneOnlyFunky(QLiterals *ql) {
  if (!ql) return NULL;
  return ql->deepCloneOnlyFunky(NULL);
}

void prependQLiterals(QLiterals * /*nullable*/ &baseQl, QLiterals *ql0) {
  if (!ql0) return;             // NOTE: required for CORRECTNESS
  QLiterals **qlp;              // find end of ql0
  for(qlp = &ql0; *qlp; qlp = &((*qlp)->next)) {}
  *qlp = baseQl;
  baseQl = ql0;
}

void appendQLiterals(QLiterals *&baseQl, QLiterals *ql0) {
  if (!ql0) return;             // NOTE: optimization ONLY
  QLiterals **baseQlp;          // find end of baseQl
  for(baseQlp = &baseQl; *baseQlp; baseQlp = &((*baseQlp)->next)) {}
  *baseQlp = ql0;
}

void prependLiteralString(QLiterals *&ql, StringRef s) {
  prependQLiterals(ql, new QLiterals(s));
}

void appendLiteralString(QLiterals *&ql, StringRef s) {
  appendQLiterals(ql, new QLiterals(s));
}

// Answer this question: is ql0 (a single element) an element of ql1
// (as a set)?
bool in_qualifiers_list
  (QLiterals *ql0               // thought of as a single element; disregard ql0->next
   ,QLiterals *ql1
   ,SourceLoc loc
   ) {
  xassert(ql0);
  if (!ql1) return false;
  // skip the funky qualifiers
  if (isFunkyQualifier(ql1, loc)) return in_qualifiers_list(ql0, ql1->next, loc);
  if (ql0->equal(ql1)) return true;
  return in_qualifiers_list(ql0, ql1->next, loc);
}

// Answer this quesetion: is ql0 (as a set) a subset of ql1 (as a
// set)?  Note that the double-induction implementation is quadratic,
// but should never grow large.
bool is_subset_of_qualifiers
  (QLiterals *ql0               // thought of as a set
   ,QLiterals *ql1
   ,SourceLoc loc
   ) {
  if (!ql0) return true;
  // skip the funky qualifiers
  if (isFunkyQualifier(ql0, loc)) return is_subset_of_qualifiers(ql0->next, ql1, loc);
  if (is_subset_of_qualifiers(ql0->next, ql1, loc) && in_qualifiers_list(ql0, ql1, loc)) {
    return true;
  }
  return false;
}
