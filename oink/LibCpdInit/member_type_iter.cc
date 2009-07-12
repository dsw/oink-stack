// see License.txt for copyright and terms of use

#include "member_type_iter.h"

// internal ****************

void MemberValueIter::set_top_cursor(int n) {
  stack0.top()->cursor = n;
  xassert(stack0.count() >= 1);
  if (stack0.count() == 1 && maxFirstFrameCursor < n) maxFirstFrameCursor = n;
}

// return the nth data member of v
Variable *MemberValueIter::nthDataMember(Value *v, int n) {
  xassert(isContainer(v));
  xassert(n>=0);
  // array
  if (v->t()->isArrayType()) {
    ArrayType *arr = v->t()->asArrayType();
    if (arr->hasSize() && n>=arr->getSize()) return NULL; // fell off the end
    return v->asArrayValue()->getAtVar();
  }
  // compound type
  xassert(v->t()->isCompoundType());
  CompoundType *ct = v->t()->asCompoundType();
  SObjList<Variable> &dataVariablesInOrder = ct->getDataVariablesInOrder();
  int numDataMembers = dataVariablesInOrder.count();
  USER_ASSERT(numDataMembers>0, loc, "Can't handle a compund type with no data members");
  if (n >= numDataMembers) return NULL; // fell off the end
  return asVariable_O(dataVariablesInOrder.nth(n));
}

// return the n such that the nth data member of t is the member named
// by id
int MemberValueIter::cursor_of_name(SourceLoc loc, Value *v, StringRef id) {
  USER_ASSERT(v->t()->isCompoundType(), loc,
              "designator .%s may only index into a compound type", id);
  CompoundType *ct = v->t()->asCompoundType();
  int cursor0 = ct->getDataMemberPosition(id);
  USER_ASSERT(cursor0>=0, loc, "No such member name %s", id);
  return cursor0;
}
  
// external utilities ****************

// is it an array of char-s or wchar_t-s?
bool MemberValueIter::isCharArray(Value *v) {
  Type *t = v->t();
  t = t->asRval();
  if (!t->isArrayType()) return false;
  return t->asArrayType()->getAtType()->isSomeKindOfCharType();
}

// is it a "point" value
bool MemberValueIter::isPoint(Value *v, bool allowCharArray) {
  Type *t = v->t();
  if (allowCharArray && isCharArray(v)) return true;
  return t->isPointerType()
    || t->isPointerToMemberType()
    || t->isEnumType()
    || t->isSimpleType();
}

// is it a "container" value
bool MemberValueIter::isContainer(Value *v) {
  Type *t = v->t();
  return t->isCompoundType() || t->isArrayType();
}

// api ****************

// take one step of the dfs; stop at nodes on the way down, but not on
// the way up
void MemberValueIter::adv() {
  xassert(!done());
//    cout << "**** adv skipAcross " << skipAcross << endl;
  if (skipAcross || !isContainer(asVariable_O(at())->abstrValue()->asRval())) {
    pop();
    skipAcross = false;
  }
  do {
    xassert(isContainer(asVariable_O(at())->abstrValue()->asRval()));
    // deal with weird union rule; essentially you only initialize one
    // member and then move on; if you assign to a union, this has the
    // effect of initializing the first member only
    if (stack0.top()->cursor>-1 &&
        asVariable_O(at())->abstrValue()->asRval()->t()->isUnionType()) {
      pop();
      continue;
    }
    set_top_cursor(stack0.top()->cursor + 1);
//      cout << "adv cursor " << stack0.top()->cursor << endl;
    if (Variable *next =
        nthDataMember(asVariable_O(at())->abstrValue()->asRval(), stack0.top()->cursor)) {
//        cout << "adv pushing next: " << next->toCString() << endl;
      push(asVariable_O(at())->abstrValue()->asRval(), next);
      return;
    }
//      cout << "adv popping" << endl;
    pop();
  } while (!done());
}

void MemberValueIter::reset(FakeList<Designator> *dl, int *range_desig_i) {
  // delete all but base of stack
//    cout << "**** reset" << endl;
  while(stack0.count()>1) pop();
  xassert(stack0.count()>=0);
  USER_ASSERT(stack0.count()==1, loc, "Too many initializers for type(3)");
  bool firstTime = true;
  // top-down index into value tree using the designator list, dl
  FAKELIST_FOREACH_NC(Designator, dl, d) {
    if (FieldDesignator *fd = d->ifFieldDesignator()) {
      // FIX: perhaps this should be computed at typechecking time, as
      // it is for array indicies
      set_top_cursor(cursor_of_name(loc, asVariable_O(at())->abstrValue()->asRval(), fd->getId()));
    } else if (SubscriptDesignator *sd = d->ifSubscriptDesignator()) {
      int index;
      if (range_desig_i && firstTime) {
        index = *range_desig_i;
        xassert(sd->getIdx_computed() <= index);
        xassert(index <= sd->getIdx_computed2());
      } else {
        USER_ASSERT(!sd->getIdx_expr2(), loc,
                    "if a range designator is used there may not be "
                    "anything else in the designator list");
        index = sd->getIdx_computed();
      }
      USER_ASSERT(asVariable_O(at())->abstrValue()->asRval()->t()->isArrayType(), loc,
                  "subscript designator may only index into an array type");
      USER_ASSERT(index >= 0, loc,
                  "array index evaluates to %d, which is < 0", index);
      int size = asVariable_O(at())->abstrValue()->asRval()->t()->asArrayType()->getSize();
      USER_ASSERT(!asVariable_O(at())->abstrValue()->asRval()->t()->asArrayType()->hasSize() ||
                  index<size, loc,
                  "array index evaluates to %d, which is >= size == %d", index, size);
      set_top_cursor(index);
    } else xfailure("illegal Designator");
//      cout << "reset cursor " << stack0.top()->cursor << endl;
    Variable *next =
      nthDataMember(asVariable_O(at())->abstrValue()->asRval(), stack0.top()->cursor);
    xassert(next);
//      cout << "reset push " << next->toCString() << endl;
    push(asVariable_O(at())->abstrValue()->asRval(), next);
    firstTime = false;
  }
}

// ensure that the cursor is at a pointer (leaf) and not an internal
// node)
Variable *MemberValueIter::ensurePoint(bool allowCharArray) {
  while(!done()) {
    if (isPoint(asVariable_O(at())->abstrValue()->asRval(), allowCharArray)) {
      return asVariable_O(at());
    } else {
      adv();
    }
  }
  return NULL;
}
