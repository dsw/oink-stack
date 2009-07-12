// see License.txt for copyright and terms of use

// Qual versions of the Value classes.

#ifndef QUAL_VALUE_CHILDREN_H
#define QUAL_VALUE_CHILDREN_H

#include "value.h"

class CVAtomicValue_QualAnnot;  // qual_annot.h
class PointerValue_QualAnnot;
class ReferenceValue_QualAnnot;
class FunctionValue_QualAnnot;
class ArrayValue_QualAnnot;
class PointerToMemberValue_QualAnnot;


#define FOREACH_CLASS_Value_Q(D)                                  \
  D(CVAtomicValue_Q)                                              \
  D(PointerValue_Q)                                               \
  D(ReferenceValue_Q)                                             \
  D(FunctionValue_Q)                                              \
  D(ArrayValue_Q)                                                 \
  D(PointerToMemberValue_Q)


// "Value_Q" methods ****************

// these methods really should go on a hypothenical Value_Q class that
// would be a superclass of all of the qual value children and a
// subclass of Value; we have not materialized such a class

// does the tree of abstract values rooted here, including descending
// into the parameters of the function values, contain:

// bool treeContainsQualAnnots_Value_Q(Value *v);
bool treeContainsQvars_Value_Q(Value const *v);
bool treeContainsExternQvars(Value const *v);

bool treeContainsQliterals(Value *v);
bool treeContainsFunkyQliterals(Value *v);
bool treeContainsUserQliterals(Value *v);
void treeEnsureQvIfUserQliterals(Value *v);


// *Value_Q classes ****************

// a leaf of a constructed type tree
class CVAtomicValue_Q : public CVAtomicValue {
public:
  friend class ValueFactory_Q;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;

  typedef CVAtomicValue_Q MyValue;
  typedef CVAtomicValue_QualAnnot MyQualAnnot;
  typedef Struct<MyValue,MyQualAnnot> MyStruct;

  MyQualAnnot *getAnnotation();
  virtual void *getAnnotation0() { return getAnnotation(); }
  virtual size_t getWholeSize();
};

inline CVAtomicValue_Q *asCVAtomicValue_Q(CVAtomicValue *t)
  { return dynamic_cast<CVAtomicValue_Q*>(t); }

// pointer to something
class PointerValue_Q : public PointerValue {
public:
  friend class ValueFactory_Q;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;
  typedef PointerValue_Q MyValue;
  typedef PointerValue_QualAnnot MyQualAnnot;
  typedef Struct<MyValue,MyQualAnnot> MyStruct;

  MyQualAnnot *getAnnotation();
  virtual void *getAnnotation0() { return getAnnotation(); }
  virtual size_t getWholeSize();
};

inline PointerValue_Q *asPointerValue_Q(PointerValue *t)
  { return dynamic_cast<PointerValue_Q*>(t); }

// reference to something
class ReferenceValue_Q : public ReferenceValue {
public:
  friend class ValueFactory_Q;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;
  typedef ReferenceValue_Q MyValue;
  typedef ReferenceValue_QualAnnot MyQualAnnot;
  typedef Struct<MyValue,MyQualAnnot> MyStruct;

  MyQualAnnot *getAnnotation();
  virtual void *getAnnotation0() { return getAnnotation(); }
  virtual size_t getWholeSize();
};

inline ReferenceValue_Q *asReferenceValue_Q(ReferenceValue *t)
  { return dynamic_cast<ReferenceValue_Q*>(t); }

// function
class FunctionValue_Q : public FunctionValue {
public:
  friend class ValueFactory_Q;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;
  typedef FunctionValue_Q MyValue;
  typedef FunctionValue_QualAnnot MyQualAnnot;
  typedef Struct<MyValue,MyQualAnnot> MyStruct;

  MyQualAnnot *getAnnotation();
  virtual void *getAnnotation0() { return getAnnotation(); }
  virtual size_t getWholeSize();
};

inline FunctionValue_Q *asFunctionValue_Q(FunctionValue *t)
  { return dynamic_cast<FunctionValue_Q*>(t); }

// array
class ArrayValue_Q : public ArrayValue {
public:
  friend class ValueFactory_Q;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;

  typedef ArrayValue_Q MyValue;
  typedef ArrayValue_QualAnnot MyQualAnnot;
  typedef Struct<MyValue,MyQualAnnot> MyStruct;

  MyQualAnnot *getAnnotation();
  virtual void *getAnnotation0() { return getAnnotation(); }
  virtual size_t getWholeSize();
};

inline ArrayValue_Q *asArrayValue_Q(ArrayValue *t)
  { return dynamic_cast<ArrayValue_Q*>(t); }

// pointer to member
class PointerToMemberValue_Q : public PointerToMemberValue {
public:
  friend class ValueFactory_Q;
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;

  typedef PointerToMemberValue_Q MyValue;
  typedef PointerToMemberValue_QualAnnot MyQualAnnot;
  typedef Struct<MyValue,MyQualAnnot> MyStruct;

  MyQualAnnot *getAnnotation();
  virtual void *getAnnotation0() { return getAnnotation(); }
  virtual size_t getWholeSize();
};

inline PointerToMemberValue_Q *asPointerToMemberValue_Q(PointerToMemberValue *t)
  { return dynamic_cast<PointerToMemberValue_Q*>(t); }


// -------------------- TypeFactory_Q ------------------
// this is a factory to make objects in the Value_Q hierarchy, and also
// Variable_O (conceptually in that hierarchy as well)
class TypeFactory_Q : public TypeFactory_O {
  virtual Variable_O *new_Variable(SourceLoc L, StringRef n, Type *t, DeclFlags f);

  // TYPE
  virtual void applyCVToType_extra
    (SourceLoc loc, Type *baseType, Type *newBaseType, TypeSpecifier *syntax);
  virtual void syntaxPointerType_extra(D_pointer *syntax, PointerType *ptq);
  virtual void makeTypeOf_receiver_extra(D_func *syntax, CVAtomicType *tmpcvat);
  virtual void makeSimilarFunctionType_extra(FunctionType *similar, FunctionType *ft);

  // TYPE
  virtual void finishCloningType(Type *tgt, Type *src);
};

class ValueFactory_Q : public ValueFactory {

  // quarl 2006-06-20
  //    Allocation functions
  virtual CVAtomicValue        *alloc_CVAtomicValue();
  virtual PointerValue         *alloc_PointerValue();
  virtual ReferenceValue       *alloc_ReferenceValue();
  virtual FunctionValue        *alloc_FunctionValue();
  virtual ArrayValue           *alloc_ArrayValue();
  virtual PointerToMemberValue *alloc_PointerToMemberValue();

  // VALUE
  virtual CVAtomicValue *makeAtVoid(CVAtomicValue const *t) const;
  virtual void finishJustClonedMember(CVAtomicValue *t, string memberName, Value *memberValue);
};

#endif // QUAL_VALUE_CHILDREN_H
