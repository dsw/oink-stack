// see License.txt for copyright and terms of use

#include "qual_funky.h"
#include "qual_value_children.h"
#include "value.h"
#include "qual_annot.h"         // QualAnnot
#include "oink_util.h"

// Funky ****************

// quarl 2006-06-23
//    MAX_NUM_FUNKY_BITSET_ELTS is the number of bits we allocate in
//    QualAnnot::PW_globalness_edgesAdded_funk (via a PackedWord).
#define MAX_NUM_FUNKY_BITSET_ELTS 29

void Funky::insert(int x) {
  xassert(0<=x);
  xassert(x<MAX_NUM_FUNKY_BITSET_ELTS);
  bitset |= 1<<x;
}

string Funky::toString() {
  if (empty()) return string("");
  stringBuilder sb;
  sb << "$";
  int localbitset = bitset;
  for(int i=0; i<MAX_NUM_FUNKY_BITSET_ELTS; ++i) {
    if (localbitset & 1) sb << "_" << i;
    localbitset >>= 1;
  }
  return sb;
}

bool Funky::isSubsetOf(Funky sq2) {
  return sq2.bitset == (sq2.bitset | bitset);
}

// Attempt to parse as a funky set qualifier: $_1_2; return 0 if not
// one
Funky parseAsFunky(char const *sqName, SourceLoc loc) {
  xassert(sqName);
  if (sqName[0]!='$') return Funky(); // nope
  if (sqName[0]=='\0' || sqName[1] != '_') return Funky(); // nope
  // from now on we assume is a funky set qualifier

  xassert(sqName[0]!='\0');     // check has at least one character
  int i = 1;                    // position
  Funky sq;                     // set qualifier return value
  while(1) {
  outer_loop:
    // start of a block
    switch(sqName[i]) {
    default:
      userFatalError(loc, "illegal character in funky set qualifier: %s", sqName);
    case '\0':
      if (!sq.empty()) return sq; // if there is something to return, return it
      userFatalError(loc, "funky set qualifier cannot be empty: %s", sqName);
    case '_': {
      ++i;
      // character after initial underscore
      if (sqName[i] == '\0') {
        userFatalError(loc, "can't end funky set qualifier with an '_': %s", sqName);
      }
      if (sqName[i] == '_') {
        userFatalError(loc, "can't put double-underscore in a funky set qualifier: %s", sqName);
      }
      int x = 0;                // parse the number
      while(1) {
        // in block, no initial underscore or nul
        switch(int c = sqName[i]) {
        default:
          userFatalError(loc, "illegal character in funky set qualifier: %s", sqName);
        case '\0':
          if (!(0<=x && x<MAX_NUM_FUNKY_BITSET_ELTS)) {
            userFatalError(loc, "illegal funky set qualifier member: %s", sqName);
          }
          sq.insert(x);
          return sq;            // the end
        case '_':
          if (!(0<=x && x<MAX_NUM_FUNKY_BITSET_ELTS)) {
            userFatalError(loc, "illegal funky set qualifier member: %s", sqName);
          }
          sq.insert(x);
          // NOTE: don't increment i, "push back" the underscore
          goto outer_loop;      // keep going
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          x *= 10;
          x += c-((int)'0');
          ++i;
        }
      }
    }
    }
  }
}

bool isFunkyQualifier(QLiterals *ql0 // as a single qualifier
                      , SourceLoc loc) {
  Funky f = parseAsFunky(ql0->name, loc);
  return !f.empty();
}

bool hasFunkyQualifiers(QLiterals *ql0 // as a set of qualifiers
                        , SourceLoc loc) {
  if (!ql0) return false;
  if (isFunkyQualifier(ql0, loc)) return true;
  return hasFunkyQualifiers(ql0->next, loc);
}

bool hasNonFunkyQualifiers(QLiterals *ql0 // as a set of qualifiers
                              , SourceLoc loc
                              ) {
  if (!ql0) return false;
  if (!isFunkyQualifier(ql0, loc)) return true;
  return hasNonFunkyQualifiers(ql0->next, loc);
}

#undef MAX_NUM_FUNKY_BITSET_ELTS

// DeepValueName ****************

DeepValueName *DeepValueName::deepClone() {
  DeepValueName *n = new DeepValueName(tag,
                                     NULL, // fill in below
                                     child ? child->deepClone() : (DeepValueName*)NULL,
                                     position);
  if (n->child) n->child->meInParent = &(n->child); // not really necessary, considering usage
  return n;
}

string DeepValueName::toString() const {
  stringBuilder sb;
  switch(tag) {
  default: xassert(0);
  case Value::V_ATOMIC:
    sb << "T"; break;
  case Value::V_FUNCTION:
    sb << "F" << position; break;
  case Value::V_POINTER: case Value::V_REFERENCE:
    sb << "P"; break;
  case Value::V_ARRAY:
    sb << "A"; break;
  }
  if (child) sb << child->toString();
  return sb;
}

Value *DeepValueName::index(Value *t) const {
  // FIX: Need to add ref-conciousness to DeepValueNames
  t = t->asRval();
  switch(tag) {
  default: xassert(0); return NULL;
  case Value::V_ATOMIC: {
    CVAtomicValue *at = t->asCVAtomicValue();
    if (!at) return NULL;       // name and value don't match
    return t;
  }
  case Value::V_FUNCTION: {
    FunctionValue *ft = t->asFunctionValue();
    if (!ft) return NULL;       // name and value don't match
    if (position==-1) return t; // the end
    xassert(child);             // if have an index, can't stop here
    // NOTE: this works for the ellipsis param also
    return child->index(ft->nthParamAbstrValue(position));
  }
  // NOTE: we allow array and pointer to be interchangeable.
  case Value::V_POINTER:
  case Value::V_REFERENCE:
  case Value::V_ARRAY: {
    if (t->isPointerValue()) {
      PointerValue *t2 = t->asPointerValue();
      if (!child) return t;     // the end
      Value *t3 = t2->getAtValue();
      return child->index(t3);
    } else if (t->isReferenceValue()) {
      ReferenceValue *t2 = t->asReferenceValue();
      if (!child) return t;     // the end
      Value *t3 = t2->getAtValue();
      return child->index(t3);
    } else if (t->isArrayValue()) {
      ArrayValue *t2 = t->asArrayValue();
      if (!child) return t;     // the end
      Value *t3 = t2->atValue;
      return child->index(t3);
    } else return NULL;         // name and value don't match
  }
  }
}

// DeepValueIterator ****************

// private
DeepValueIterator::StateLayer::StateLayer
  (DeepValueName *name0, Value *value0) : name(name0), value(value0) {
  xassert(name->tag == (Value_Tag)(value->getTag()));
}

void DeepValueIterator::pushNewChild(Value *newChild) {
  xassert(newChild);
  DeepValueName *new_name =
    new DeepValueName((Value_Tag)(newChild->getTag()), &(topName()->child));
  topName()->child = new_name;
  stack0.push(new StateLayer(new_name, newChild));
}

// private
bool DeepValueIterator::nextFunctionParameter() {
  xassert(topName()->tag==(Value_Tag)(Value::V_FUNCTION));
  FunctionValue *ft = topValue()->asFunctionValue();
  // recall that the retValue is position 0, so I want
  // less-than-or-equals here
  ++(topName()->position);
  if (topName()->position <= ft->maxNthParamIndex()) {
    // shift over one parameter and go down; NOTE: this works for the
    // ellipsis param also
    pushNewChild(ft->nthParamAbstrValue(topName()->position));
    return true;
  } else {
    stack0.pop();
    return false;
  }
}

DeepValueIterator::DeepValueIterator(Value *t)
{
  t = t->asRval();
  name = new DeepValueName((Value_Tag)(t->getTag()), NULL);
  stack0.push(new StateLayer(name, t));
}

void DeepValueIterator::adv() {
  xassert(!isDone());
//    printf("DeepValueIterator::adv() %s\n", topName()->toString().pcharc());
  switch(topName()->tag) {
  default: xassert(0);
  case Value::V_ATOMIC: {        // go up
    while(1) {
      while(!stack0.isEmpty() && topName()->tag!=(Value_Tag)(Value::V_FUNCTION)) {
        if(topName()->meInParent) *(topName()->meInParent) = NULL;
        delete topName();
        stack0.pop();
      }
      if(stack0.isEmpty()) return; // we are done
      if (nextFunctionParameter()) return;
      // otherwise, keep going up
    }
    break;
  }
  case Value::V_FUNCTION: {  // go down
    bool success = nextFunctionParameter();
    xassert(success);           // always has at least the return value
    break;
  }
  case Value::V_POINTER:        // go down
    pushNewChild(topValue()->asPointerValue()->getAtValue());
    break;
  case Value::V_REFERENCE:      // go down
    pushNewChild(topValue()->asReferenceValue()->getAtValue());
    break;
  case Value::V_ARRAY:          // go down
    pushNewChild(topValue()->asArrayValue()->atValue);
    break;
  }
}

// DeepValueNamePair ****************

string DeepValueNamePair::toString() const {
  stringBuilder sb;
  sb << "(" << x->toString() << "," << y->toString() << ")";
  return sb;
}

// AdjList ****************

static bool funkyIsNontrivialSubsetOf(Value *t0, Value *t1) {
  if (t0 == t1) {return false;}
  if (qa(t0)->getFunk().empty()) {return false;}
  if (qa(t1)->getFunk().empty()) {return false;}
  if (qa(t0)->getFunk().isSubsetOf(qa(t1)->getFunk())) {return true;}
  else {return false;}
}

AdjList::AdjList(FunctionValue_Q* ft) {
  for(DeepValueIterator iter0(ft); !iter0.isDone(); iter0.adv()) {
    // optimization: check if this value has any funky annotations;
    // skip if not
    if (qa(iter0.currentSubValue())->getFunk().empty()) continue;
    for(DeepValueIterator iter1(ft); !iter1.isDone(); iter1.adv()) {
      if (funkyIsNontrivialSubsetOf(iter0.currentSubValue(), iter1.currentSubValue())) {
        // The order that they are on the list doesn't matter, so do
        // it the O(1)-time way.
        pairList.prepend(new DeepValueNamePair(iter0.currentName()->deepClone(),
                                               iter1.currentName()->deepClone()));
      }
    }
  }
}

string AdjList::toString() {
  stringBuilder sb;
  sb << "AdjList:";
  for(ObjListIter<DeepValueNamePair> iter(pairList); !iter.isDone(); iter.adv()) {
    sb << const_cast<DeepValueNamePair*>(iter.data())->toString();
  }
  return sb;
}

// DeepLiteralsList ****************

void DeepLiteral::attach(Value *t) const {
  qa(dtn->index(t))->attachOneLiteralIfInLattice(SL_UNKNOWN, ql->name);
}

// DeepLiteralsList ****************

DeepLiteralsList::DeepLiteralsList(Value *t) {
//    cout << "DeepLiteralsList::DeepLiteralsList " << t->toMLString() << endl;
  for(DeepValueIterator iter0(t); !iter0.isDone(); iter0.adv()) {
    // FIX: should seperate qualifier literals and funky literals
    // earlier on in the Type ctor
    for(QLiterals *ql0=qa(iter0.currentSubValue())->getQl(); ql0; ql0=ql0->next) {
      if (isFunkyQualifier(ql0, t->loc)) continue; // skip funky qualifiers
      dll.prepend(new DeepLiteral(iter0.currentName()->deepClone(), ql0->deepClone()));
    }
  }
}

void DeepLiteralsList::attach(Value *t) {
  for(ObjListIter<DeepLiteral> iter0(dll); !iter0.isDone(); iter0.adv()) {
    iter0.data()->attach(t);
  }
}
