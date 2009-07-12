// see License.txt for copyright and terms of use

// Modifications to the Type/Value pretty-printing process to
// implement the -fq-print-trans-qual functionality.  A 'type printer'
// for oink [Abstract] Values that will also print the conclusions of
// the backend as to the qualifiers that apply at each point.

#ifndef QUAL_PRINT_H
#define QUAL_PRINT_H

#include "value_print.h"        // ValueTypePrinter

// FIX: PointerToMember is not done.

class QualCTypePrinter : public ValueTypePrinter {
  public:
  // true iff we should print the transitively-concluded qualifiers on
  // the type
  bool printTransQual;

  QualCTypePrinter(bool printTransQual0)
    : printTransQual(printTransQual0)
  {}
  virtual ~QualCTypePrinter() {}

  // used by the qual analysis
  virtual void leftString_extra(CVAtomicValue *type, stringBuilder &sb);
  virtual void leftString_extra(PointerValue *type, stringBuilder &sb);
  virtual void leftString_extra(ReferenceValue *type, stringBuilder &sb);
  virtual void rightStringUpToQualifiers_extra(FunctionValue *type, stringBuilder &sb);
};

#endif // QUAL_PRINT_H
