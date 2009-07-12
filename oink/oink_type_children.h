// see License.txt for copyright and terms of use

#ifndef OINK_TYPE_CHILDREN_H
#define OINK_TYPE_CHILDREN_H

#include "cc_type.h"            // Type, etc.
#include "cc_ast.h"             // Initializer
#include "taillist.h"
#include "strsobjdict.h"	// StringSObjDict

class Variable_O;               // oink_var.h

// class CompoundType_O ****************

// NOTE: this class is currently just empty, but I'd rather leave it
// in existance than have to re-create it if there were something we
// would want to add later to CompoundType_O.
class CompoundType_O : public CompoundType {
  public:
  CompoundType_O(Keyword k, StringRef n)
    : CompoundType(k, n)
  {}
};


// -------------------- TypeFactory_O ------------------
// this is a factory to make objects in the *Type hierarchy, and also
// Variable_O (conceptually in that hierarchy as well)
//
// note that since TypeFactory_O is a friend of all of the *Type
// classes, it knows they inherit from Type so we don't need to mess
// around with the type() function (and more importantly, we can
// directly *down*cast from Type to a *Type)
class TypeFactory_O : public TypeFactory {
  BasicTypeFactory bTFac;

  public:
  TypeFactory_O() {}
  ~TypeFactory_O() {}

  // TypeFactory methods ****************

  virtual CompoundType *makeCompoundType(CompoundType::Keyword keyword, StringRef name);

  virtual CVAtomicType *makeCVAtomicType(AtomicType *atomic, CVFlags cv);
  virtual PointerType *makePointerType(CVFlags cv, Type *atType);
  // this returns a Type* instead of a ReferenceType because I need to
  // be able to return an error type
  virtual Type *makeReferenceType(Type *atType);
  virtual FunctionType *makeFunctionType(Type *retType);
  // this must be called after 'makeFunctionType' once all of the
  // parameters have been added
  virtual void doneParams(FunctionType *ft) {}
  virtual ArrayType *makeArrayType(Type *eltType, int size);
  virtual PointerToMemberType *makePointerToMemberType
    (NamedAtomicType *inClassNAT, CVFlags cv, Type *atType);
  virtual DependentSizedArrayType *makeDependentSizedArrayType
    (Type *eltType, Expression *sizeExpr);

  virtual Type *setQualifiers(SourceLoc loc, CVFlags cv, Type *baseType, TypeSpecifier *syntax);
  // NOTE: do NOT override applyCVToType(); don't do this:
//   virtual Type *applyCVToType(SourceLoc loc, CVFlags cv, Type *baseType, TypeSpecifier *syntax);
  virtual bool wantsQualifiedTypeReuseOptimization() { return false; }

  virtual Type *syntaxPointerType
    (SourceLoc loc, CVFlags cv, Type *underlying, D_pointer *syntax);
  virtual Type *syntaxReferenceType
    (SourceLoc loc, Type *underlying, D_reference *syntax);

  virtual ReferenceType *makeTypeOf_receiver
    (SourceLoc loc, NamedAtomicType *classType, CVFlags cv, D_func *syntax);
  virtual FunctionType *makeSimilarFunctionType
    (SourceLoc loc, Type *retType, FunctionType *similar);

  virtual Variable *makeVariable(SourceLoc L, StringRef n, Type *t, DeclFlags f);

  // internal methods ****************

  // those outside who need to call them go through the typefactory
  // functions below
  protected:

  // these just call the ctor, nothing else; they are here so you can
  // intercept the ctor calls
  virtual CompoundType_O *new_CompoundType(CompoundType::Keyword keyword, StringRef name);

  virtual Variable_O *new_Variable(SourceLoc L, StringRef n, Type *t, DeclFlags f);

  public:
  virtual Variable_O *cloneVariable_O(Variable_O *src);

  protected:
  // TYPE
  // copy QLiterals from AST to the types
  virtual void applyCVToType_extra
    (SourceLoc loc, Type *baseType, Type *newBaseType, TypeSpecifier *syntax) {}
  virtual void syntaxPointerType_extra(D_pointer *syntax, PointerType *ptq) {}
  virtual void makeTypeOf_receiver_extra(D_func *syntax, CVAtomicType *tmpcvat) {}
  virtual void makeSimilarFunctionType_extra(FunctionType *similar, FunctionType *ft) {}

  // TYPE
  virtual void finishCloningType(Type *tgt, Type *src) {}
};

// mangle a Type to StringRef name; use this instead of the raw Elsa
// mangle().
StringRef mangleToStringRef(Type const *t);

// mangle a Type to StringRef name but also mod out by a type
// equivalence-relation
StringRef mangleToStringRef_TypeEquiv(Type const *t);

#endif // OINK_TYPE_CHILDREN_H
