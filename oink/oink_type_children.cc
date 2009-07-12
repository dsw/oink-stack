// see License.txt for copyright and terms of use

// 1) all the subtypes of Type and 2) the TypeFactory

#include "oink_type_children.h" // this module
#include "oink_global.h"
#include "oink_var.h"
#include "oink_util.h"
#include "mangle.h"		// mangle()
#include "value.h"


// TypeFactory methods ****************

CompoundType *TypeFactory_O::makeCompoundType(CompoundType::Keyword keyword, StringRef name) {
  return new_CompoundType(keyword, name);
}


CVAtomicType *TypeFactory_O::makeCVAtomicType(AtomicType *atomic, CVFlags cv) {
  // dsw: we want to defeat any optimizations where a CVAtomicType
  // might be being re-used in the Elsa factory; therefore we do not
  // delegate to it.
//    return bTFac.makeCVAtomicType(atomic, cv);
  return new CVAtomicType(atomic, cv);
}

PointerType *TypeFactory_O::makePointerType(CVFlags cv, Type *atType) {
  return bTFac.makePointerType(cv, atType);
}

// this returns a Type* instead of a ReferenceType because I need to
// be able to return an error type
Type *TypeFactory_O::makeReferenceType(Type *atType) {
  return bTFac.makeReferenceType(atType);
}

FunctionType *TypeFactory_O::makeFunctionType(Type *retType) {
  return bTFac.makeFunctionType(retType);
}

ArrayType *TypeFactory_O::makeArrayType(Type *eltType, int size) {
  return bTFac.makeArrayType(eltType, size);
}

PointerToMemberType *TypeFactory_O::makePointerToMemberType
  (NamedAtomicType *inClassNAT, CVFlags cv, Type *atType) {
  return bTFac.makePointerToMemberType(inClassNAT, cv, atType);
}


DependentSizedArrayType *TypeFactory_O::makeDependentSizedArrayType
  (Type *eltType, Expression *sizeExpr) 
{
  return bTFac.makeDependentSizedArrayType(eltType, sizeExpr);
}


// NOTE: the body of this function used to be the the body of
// applyCVToType(); On 13-Aug-03 Scott seems to have made this new
// function in cc_type.cc and factored this out.  Therefore I follow
// that by doing the same here.  I cannot think of a test that would
// reveal that this makes a difference; computeLUB() seems to be the
// only place in elsa where this is called and might make a difference
// since in elsa it makes a shallow clone and here a deep clone.  It
// is too hard to come up with a test to show that it makes a
// difference, so I do it just for symmetry.
Type *TypeFactory_O::setQualifiers(SourceLoc loc, CVFlags cv, Type *baseType,
                                   TypeSpecifier *syntax)
{
  // jump out if input is an error the same way the default factory
  // does; FIX: I wonder if this can hit an assertion failure
  if (baseType->isError()) return baseType;

  // note: unlike Elsa we *always* do the clone
  Type *newBaseType = shallowCloneType(baseType);

  if (baseType->isCVAtomicType()) {
    newBaseType->asCVAtomicType()->cv = cv;
  } else if (baseType->isPointerType()) {
    newBaseType->asPointerType()->cv = cv;
  } else if (baseType->isPointerToMemberType()) {
    newBaseType->asPointerToMemberType()->cv = cv;
  } else {
    // this is a strange situation that you can probably only make
    // with a typedef; just drop the cv on the floor; Elsa gives an
    // error in this case
  }

  // FIX: applyCVToType_extra() should be renamed
  // setQualifiers_extra()
  applyCVToType_extra(loc, baseType, newBaseType, syntax);

  return newBaseType;
}

Type *TypeFactory_O::syntaxPointerType
  (SourceLoc loc, CVFlags cv, Type *type, D_pointer *syntax)
{
  PointerType *ptq = makePointerType(cv, type);
  syntaxPointerType_extra(syntax, ptq);
  return ptq;
}

Type *TypeFactory_O::syntaxReferenceType
  (SourceLoc loc, Type *type, D_reference *syntax)
{
  return bTFac.makeReferenceType(type)->asReferenceType();
}

// sm: I didn't override or implement syntaxPointerToMemberType
// because the concrete syntax doesn't include an opportunity to
// supply qualifier literals, and therefore it would do nothing
// more than call makePointerToMemberType, which is what the
// default implementation already does.

ReferenceType *TypeFactory_O::makeTypeOf_receiver
  (SourceLoc loc, NamedAtomicType *classType, CVFlags cv, D_func *syntax)
{
  CVAtomicType *tmpcvat = makeCVAtomicType(classType, cv);
  makeTypeOf_receiver_extra(syntax, tmpcvat);
  ReferenceType *ret = bTFac.makeReferenceType(tmpcvat)->asReferenceType();
  return ret;
}

FunctionType *TypeFactory_O::makeSimilarFunctionType
  (SourceLoc loc, Type *retType, FunctionType *similar)
{
  FunctionType *ft = makeFunctionType(retType);
  makeSimilarFunctionType_extra(similar, ft);
  return ft;
}

Variable *TypeFactory_O::makeVariable(SourceLoc L, StringRef n, Type *t, DeclFlags f)
{
  return new_Variable(L, n, t, f);
}

// TypeFactory internal methods ****************

CompoundType_O *TypeFactory_O::new_CompoundType(CompoundType::Keyword keyword, StringRef name) {
  return new CompoundType_O(keyword, name);
}

Variable_O *TypeFactory_O::new_Variable
  (SourceLoc L, StringRef n, Type *t, DeclFlags f)
{
  return new Variable_O(L, n, t, f);
}

Variable_O *TypeFactory_O::cloneVariable_O(Variable_O *src)
{
//    Type *t0 = cloneType_O(src->type, src->loc);
  Type *t0 = src->type;         // NOTE: no more cloning of the type
  Variable_O *ret =
    new_Variable(src->loc
                 , src->name // FIX: don't see a reason to clone the name
                 , t0
                 , src->flags); // an enum, so nothing to clone
  ret->setValue(src->value ? src->value->clone() : NULL);

  ret->defaultParamType = NULL;
  if (src->defaultParamType) {
//      ret->defaultParamType = cloneType_O(src->defaultParamType, src->loc);
    ret->defaultParamType = src->defaultParamType;// NOTE: no more cloning of the type
  }

  // ret->funcDefn not cloned; I don't think I need it
  // ret->overload left as NULL as set by the ctor; not cloned
  ret->scope = src->scope;      // don't see a reason to clone the scope
  // ret->intData stores three fields
  ret->setAccess(src->getAccess());
  ret->setScopeKind(src->getScopeKind());
  ret->setParameterOrdinal(src->getParameterOrdinal());
  // ret->usingAlias_or_parameterizedEntity; what is this? not cloned
  // ret->templInfo; not cloned; should be?
  // FIX: there are other members of Variable that perhaps should be
  // cloned
  return ret;
}

StringRef mangleToStringRef(Type const *t) {
  return globalStrTable(mangle(t));
}

// NOTE: that there are some difficulties as if I want to change to
// another type, I would have to actually make an object of another
// type, and I don't want to do that, so instead I implement part of
// the mangling myself here.
StringRef mangleToStringRef_TypeEquiv(Type const *t) {
  // treat all "integer types" as eqivalent
  if (t->isIntegerType()) {
    return globalStrTable("int");
  }
  if (t->isArrayType() || t->isPointerType()) {
    return globalStrTable(stringc << mangleToStringRef_TypeEquiv(t->getAtType()) << "*");
  }
  // TODO: recurse into function arguments
  return mangleToStringRef(t);
}

