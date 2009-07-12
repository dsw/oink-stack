// see License.txt for copyright and terms of use

// A class for printing Values; inherits from elsa class TypePrinter.
// It is a type printer that prints out in C syntax, but it does not
// inherit from CTypePrinter, that is why it is called
// ValueTypePrinter not ValueCTypePrinter.

#ifndef OINK_VALUE_PRINT_H
#define OINK_VALUE_PRINT_H

#include "cc_print.h"           // CTypePrinter
#include "cc_ast.h"             // C++ AST
#include "value.h"              // Value

// FIX: PointerToMemberValue is not done.

class ValueTypePrinter : public TypePrinter {
  public:
  // For printing out types that are not abstract values on the
  // occasion that we have to do this
  CTypePrinter elsaTypePrinter;

  ValueTypePrinter () {
    // turn this off so that we don't accidentally use it
    CTypePrinter::enabled = false;
  }
  virtual ~ValueTypePrinter() {
    CTypePrinter::enabled = true;
  }

  // **** **** copied from CTypePrinter

  // satisfy the interface to TypePrinter
  virtual void print(OutStream &out, TypeLike const *valueOrType0, char const *name);
  virtual TypeLike const *getTypeLike(Variable const *var);
  virtual TypeLike const *getFunctionTypeLike(Function const *func);
  virtual TypeLike const *getE_constructorTypeLike(E_constructor const *c);

  protected:
  // **** AtomicType
  virtual string print(AtomicType const *atomic);

  virtual string print(SimpleType const *);
  virtual string print(CompoundType const *);
  virtual string print(EnumType const *);
  virtual string print(TypeVariable const *);
  virtual string print(PseudoInstantiation const *);
  virtual string print(DependentQType const *);

  // **** [Compound]Type
  virtual string print(Value const *type);
  virtual string print(Value const *type, char const *name);
  virtual string printRight(Value const *type, bool innerParen = true);
  virtual string printLeft(Value const *type, bool innerParen = true);

  virtual string printLeft_orig(CVAtomicValue const *type, bool innerParen = true);
  virtual string printRight(CVAtomicValue const *type, bool innerParen = true);
  virtual string printLeft_orig(PointerValue const *type, bool innerParen = true);
  virtual string printRight(PointerValue const *type, bool innerParen = true);
  virtual string printLeft_orig(ReferenceValue const *type, bool innerParen = true);
  virtual string printRight(ReferenceValue const *type, bool innerParen = true);
  virtual string printLeft(FunctionValue const *type, bool innerParen = true);
  virtual string printRight(FunctionValue const *type, bool innerParen = true);
  virtual string printRightUpToQualifiers_orig(FunctionValue const *type, bool innerParen);
  virtual string printRightQualifiers(FunctionValue const *type, CVFlags cv);
  virtual string printRightAfterQualifiers(FunctionValue const *type);
  virtual void   printExtraRightmostSyntax(FunctionValue const *type, stringBuilder &);
  virtual string printLeft(ArrayValue const *type, bool innerParen = true);
  virtual string printRight(ArrayValue const *type, bool innerParen = true);
  virtual string printLeft(PointerToMemberValue const *type, bool innerParen = true);
  virtual string printRight(PointerToMemberValue const *type, bool innerParen = true);

  // **** Variable
  virtual string printAsParameter(Variable const *var);

  // **** **** modified for oink

  virtual string printLeft(CVAtomicValue const *type, bool innerParen = true);
  virtual string printLeft(PointerValue const *type, bool innerParen = true);
  virtual string printLeft(ReferenceValue const *type, bool innerParen = true);
  virtual string printRightUpToQualifiers(FunctionValue const *type, bool innerParen);

  // hooks used by the qual analysis
  virtual void leftString_extra(CVAtomicValue *type, stringBuilder &sb) {}
  virtual void leftString_extra(PointerValue *type, stringBuilder &sb) {}
  virtual void leftString_extra(ReferenceValue *type, stringBuilder &sb) {}
  virtual void rightStringUpToQualifiers_extra(FunctionValue *type, stringBuilder &sb) {}
};

#endif // OINK_VALUE_PRINT_H
