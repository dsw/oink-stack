// see License.txt for copyright and terms of use

#include "qual_ast_aux.h"       // this module

void ASTPrinter_Q::TypeSpecifier_printExtras
  (TypeSpecifier const *const ts, ostream &os, int indent) const
{
  ASTPrinter_O::TypeSpecifier_printExtras(ts, os, indent);
  xassert(ts);
  if (ts->ql) {
    ind(os, indent);
    os << toString(ts->ql) << endl;
  }
}

string ASTPrinter_Q::toString(QLiterals const * ql) const
{
  if (ql) return ql->toString();
  return string("");
}

// FIX: these are dummy implementations
string ASTPrinter_Q::toXml_QLiterals(QLiterals *ql) {
  return stringc << "0";
}

void ASTPrinter_Q::fromXml_QLiterals(QLiterals *&ql, string str) {
  ql = 0;
}
