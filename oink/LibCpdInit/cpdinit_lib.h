// see License.txt for copyright and terms of use

// This vacuous header file provides all the context necessary to
// build cpdinit.cc independent of the rest of the system.  It exists
// to demonstrate the API that others would need to implement and to
// test that the list is complete.  You should replace it with your
// own header file for your actual libraries.

#ifndef CPDINIT_LIB_H
#define CPDINIT_LIB_H

// forwards
class Type;
class ArrayType;
class CompoundType;
class Value;
class ArrayValue;
class Variable;
class Designator;
class Expression;
class IN_expr;
class IN_compound;
class IN_designated;
class FieldDesignator;
class SubscriptDesignator;
template<class T> class SObjList;
template<class T> class ASTList;
template<class T> class FakeList;

//  **** Utilities

// From Scott McPeak
// NULL
#ifndef NULL
#  define NULL 0
#endif // NULL

//  Indicates a source location.  Note that it is passed around by
//  value, so make it copyable and assignable (such as an int indexing
//  into a table).
typedef int SourceLoc;

//  A char* from a string table, so that strcmp() should give the same
//  answer as == (equals implies eq).  Even if I don't use that fact
//  now, I reserve the right to use it in future versions.
typedef char *StringRef;

class UserError {
  public:
  UserError(int exitCode0, SourceLoc loc, char const *message...) {}
};

//  Throws an exception unless b is true.  It is intended for internal
//  errors that the user should not be able to make happen.
void xassert(bool b); // noreturn

//  Throws an exception containing a message for the user.
void xfailure(char const *message); // noreturn

// If BOOL is false it indicates a user error.  In such case, ARGS are
// a printf-formatted message for the user and LOC is the source
// location; NOTE: the implementation is just so I don't get "unused"
// warnings

// NOTE: You MUST have your USER_ASSERT macro throw (by value) an
// instance of class UserError if the first boolean argument is false.
// See the implementation in cpdinit_lib.h.
#define USER_ASSERT(BOOL, LOC, ARGS...) do {if (!(BOOL)) throw UserError(1, LOC, ARGS);} while(0)

// See the Readme file for details on how to have the error message in
// the USER_ASSERT be preserved in the UserError() exception so that
// it gets passed to the reportUserError() function.

// a type
class Type {
  public:
  // low a reference to is rvalue
  Type *asRval();

  // can downcast ?
  bool isSimpleCharType() const; // a char
  bool isSimpleWChar_tType() const; // a wchar_t
  bool isSomeKindOfCharType() const; // a char, plain, signed, or unsigned, or wchar_t
  bool isSimpleType() const; // int, double, void, etc.
  bool isEnumType() const;
  bool isPointerType() const;
  bool isPointerToMemberType() const;
  bool isArrayType() const;
  bool isCompoundType() const; // class, struct, union
  bool isUnionType() const;

  // semantically, a downcast, though you don't have to implement it
  // that way
  ArrayType *asArrayType();
  CompoundType *asCompoundType();
};

class ArrayType /*: public Type */ {
  public:
  // does the array have any size specified? int a[3] yes, int a[] no.
  int getSize();
  bool hasSize();
  Type *getAtType();            // the type contained in the array
};

// doesn't actually inherit from Type
class CompoundType {
  public:
  // get a list of the data member variables of the compound type in
  // their syntactic order
  SObjList<Variable> &getDataVariablesInOrder();
  // get the position in syntactic order of the name of the member
  // variable specified
  int getDataMemberPosition(StringRef name) const;
};

class Value {
  public:
  Type *t();
  Value *asRval();
  ArrayValue *asArrayValue();
};

class ArrayValue {
  public:
  Value *getAtValue();
  Variable *getAtVar();
};

class Variable {
  public:
  Value *abstrValue();
};

#define asVariable_O(VAR) VAR

//  **** Parse tree

class Initializer {
  public:
  SourceLoc getLoc();          // get the location
  // semantically, a dynamic_cast down
  IN_compound *ifIN_compound(); // returns NULL if wrong type
  IN_compound *asIN_compound(); // asserts that is the right type
  bool isIN_compound();         // checks if is the right type

  IN_expr *ifIN_expr();
  IN_expr *asIN_expr();
  bool isIN_expr();

  IN_designated *ifIN_designated();
};
// one for each leaf element expression in a compound initializer
class IN_expr /*: public Initializer*/ {
  public:
  SourceLoc getLoc();           // get the location
  Expression *getE();           // get the expression
};
// one for each pair of curlies; holds a list of the Initializer-s
// inside
class IN_compound /*: public Initializer*/ {
  public:
  SourceLoc getLoc();          // get the location
  ASTList<Initializer> &getInits(); // get the contents
  // Semantically, an explicit _up_cast.  This prevents me from explicitly
  // specifying the inheritance heirarchy among the three Initializer
  // classes in case you want to do your hierarchy differently.
  Initializer *uptoInitializer();
};
// one for each "name = value" kind of leaf element in a compound
// initializer
class IN_designated /*: public Initializer*/ {
  public:
  SourceLoc getLoc();          // get the location
  FakeList<Designator> *getDesignatorList();
  Initializer *getInit();
};

class Designator {
  public:
  SourceLoc getLoc();          // get the location
  // semantically, a dynamic_cast down
  FieldDesignator *ifFieldDesignator();
  SubscriptDesignator *ifSubscriptDesignator();
};

// a field name such as ".x"
class FieldDesignator /* : public Designator*/ {
  public:
  SourceLoc getLoc();           // get the location
  StringRef getId();            // "x";
};

// a subscript, such as "[1]" or "[1 ... 3]"
class SubscriptDesignator /* : public Designator*/ {
  public:
  SourceLoc getLoc();           // get the location
  Expression *getIdx_expr();    // AST node for "1"
  Expression *getIdx_expr2();   // AST node for "3", if it exists
  int getIdx_computed();        // computed value of "1"
  int getIdx_computed2();       // computed value of "3", if it exists
};

class E_compoundLit;
class Expression {
  public:
  Value *getAbstrValue();       // abstract value of the expression
  E_compoundLit *asE_compoundLit();
  bool isE_compoundLit();
  Expression *skipGroups();
};

class E_compoundLit : Expression {
  public:
  IN_compound *getInit();
};

//  **** Containers

//  My template container classes hold pointers to the template
//  parameter class T.  See the Readme file for a pointer to Scott
//  McPeak's implementation.

// a list
template<class T> class ASTList {
  public:
  // NOTE: you don't need to have this iter subclass, but I put it in
  // so I could write the fake definition for the iterating macro
  // below; We do in fact have such as iterator class, but you could
  // implement the iterating macro some other way.
  class Iter {
    public:
    T *data();
  };
};
// supports this iterator macro; within the loop, iter.data() is a
// pointer to the current element; NOTE: the implementation I give
// won't work; it is just a fake so I can compile
#define FOREACH_ASTLIST_NC(T, list, iter) for(ASTList<T>::Iter iter;;)

// a list
template<class T> class SObjList {
  public:
  int count() const;
  T *nth(int which);
};

// a linked list
template<class T> class FakeList {
  public:
  T *first();
  FakeList<T> *butFirst();
};
// supports this iterator macro; within the loop, elt is a pointer to
// the current element; NOTE: the implementation I give won't work; it
// is just a fake so I can compile
#define FAKELIST_FOREACH_NC(T, list, elt) for(T *elt;;)

// a stack
template<class T> class SObjStack {
  public:
  T *pop();
  void push(T *item);
  T *top();
  int count() const;
  bool isEmpty() const;
  bool isNotEmpty() const;
};

#endif // CPDINIT_LIB_H
