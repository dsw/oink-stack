// see License.txt for copyright and terms of use

// An iterator used by the cpdinit module to keep track of where it is
// in the compound initializer.

#ifndef MEMBER_TYPE_ITER_H
#define MEMBER_TYPE_ITER_H

// During preprocessing, set the variable CPDINIT_LIB_HEADERS to point
// to your replacement for cpdinit_lib.h.  See the "building" section
// of the Readme file for more on this.
#include CPDINIT_LIB_HEADERS

// In order to support instance-sensitivity in Oink, we need to think
// of the member of a container not just as a single thing, a Value,
// but as a container Value together with a contained Variable.  Such
// objects are already present in CompoundValue-s as the member
// variables; Arrays have been augmented to have a contained variable
// for this purpose.

// iterate over the data members of a value in a top-down recursive
// fashion; see C99 standard section 6.7.8.  Note that the spec
// includes two exceptions: 1) a character array can be initialized by
// a string literal; 2) when iterating through a union you jump out
// after the first member.
class MemberValueIter {
  protected:
  struct Frame {
    Value *container;           // the container of the variable in question
    Variable *content;          // the content variable
    int cursor;                 // ??
    Frame(Value *container0, Variable *content0, int cursor0)
      : container(container0), content(content0), cursor(cursor0)
    {}
  };

  // state ****************
  SourceLoc loc;
  MemberValueIter *parentIter;
  Variable *base;               // "current object" in C99 standard, p.125
  bool skipAcross;              // should next adv() go down (false) or across (true)
  int maxFirstFrameCursor;      // the max value ever attainted by the first frame on the stack
  SObjStack<Frame> stack0;      // frozen dfs of container values

  // tor ****************
  public:
  MemberValueIter(SourceLoc loc0, MemberValueIter *parentIter0, Variable *base0)
    : loc(loc0), parentIter(parentIter0), base(base0)
    , skipAcross(false), maxFirstFrameCursor(-1) {
    push(NULL, base);
  }
  ~MemberValueIter() {
    while(!done()) pop();       // delete stack
  }
  private:                      // don't allow copying
  MemberValueIter(MemberValueIter const &);

  // internal ****************
  protected:
  void pop() {
    delete stack0.pop();
  }
  void push(Value *container, Variable *contents) {
    stack0.push(new Frame(container, contents, -1));
  }
  void set_top_cursor(int n);
  // return the nth data member of t
  Variable *nthDataMember(Value *v, int n);
  // return the n such that the nth data member of t is the member
  // named by id
  static int cursor_of_name(SourceLoc loc, Value *t, StringRef id);

  // external utilities ****************
  public:
  static bool isCharArray(Value *t);
  static bool isPoint(Value *t, bool allowCharArray);
  static bool isContainer(Value *t);

  // api ****************
  Value *at_container() {
    USER_ASSERT(stack0.isNotEmpty(), loc,
                "Confused by something about the compound initializer nesting depth.");
    Value *v = stack0.top()->container;
    // if we are at the top, then container is NULL
//     xassert(v);
    return v;
  }
  Variable *at() {
    USER_ASSERT(stack0.isNotEmpty(), loc,
                "Confused by something about the compound initializer nesting depth."
                " 3ba6003d-d210-4211-b01b-59a3f41b5c0d");
    Variable *v = stack0.top()->content;
    xassert(v);
    return v;
  }
  bool done() {return stack0.isEmpty();}
  void adv();
  void reset(FakeList<Designator> *dl, int *range_desig_i = NULL);
  Variable *ensurePoint(bool allowCharArray);
  void setSkipAcross(bool skipAcross0) {
    // FIX: why can this assertion fail?
//      xassert(!skipAcross);
    skipAcross = skipAcross0;
  }
  int getMaxFirstFrameCursor() { return maxFirstFrameCursor; }
  // A definition is not provided for this method, and since it is not
  // virtual and not called, you don't need one.  I use it for
  // testing.
  char const *toString();
};

#endif // MEMBER_TYPE_ITER_H
