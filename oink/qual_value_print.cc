// see License.txt for copyright and terms of use

#include "qual_value_print.h"   // this module
#include "qual_annot.h"         // QualAnnot
#include "qual_value_children.h" // CVAtomicValue_Q etc.
#include "qual_global.h"        // qualCmd

// QualCTypePrinter ****************

void QualCTypePrinter::leftString_extra(CVAtomicValue *value0, stringBuilder &sb) {
  CVAtomicValue_Q *value = asCVAtomicValue_Q(value0);
  sb << ::toString(qa(value)->getQl());
  if (printTransQual) {
    sb << qa(value)->inferredQualifiersToString();
  }
}

void QualCTypePrinter::leftString_extra(PointerValue *value0, stringBuilder &sb) {
  PointerValue_Q *value = asPointerValue_Q(value0);
  sb << ::toString(qa(value)->getQl());
  if (printTransQual) {
    sb << qa(value)->inferredQualifiersToString();
  }
}

void QualCTypePrinter::leftString_extra(ReferenceValue *value0, stringBuilder &sb) {
  ReferenceValue_Q *value = asReferenceValue_Q(value0);
  sb << ::toString(qa(value)->getQl());
  if (printTransQual) {
    sb << qa(value)->inferredQualifiersToString();
  }
}

void QualCTypePrinter::rightStringUpToQualifiers_extra(FunctionValue *value0, stringBuilder &sb)
{
  FunctionValue_Q *value = asFunctionValue_Q(value0);
  // sm: we now print the qualifiers on 'type', not on the function itself
  if (value->type->isMethod()) {
    // FIX: do this with an asFunctionValue_QualAnnot()
    sb << ::toString(dynamic_cast<FunctionValue_QualAnnot&>(*qa(value)).getThisQLiterals());
  }
  if (printTransQual) {
    sb << qa(value)->inferredQualifiersToString();
  }
}
