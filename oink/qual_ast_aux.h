// see License.txt for copyright and terms of use

// Implementation of functions declared in the qual.ast file.

#ifndef QUAL_AST_AUX_H
#define QUAL_AST_AUX_H

#include "cc_ast.h"             // cc.ast.gen.h

// You may wonder at the need for a qual_ast_aux.h.  Here I define
// classes that would be defined in the verbatim section of qual.ast,
// except that that is shared by oink, and so I put them here so Oink
// can avoid seeing them.

class ASTPrinter_Q : public ASTPrinter_O {
  public:
  virtual void TypeSpecifier_printExtras
    (TypeSpecifier const *const ts, std::ostream &os, int indent) const;
  virtual string toString(QLiterals const * ql) const;
  virtual string toXml_QLiterals(QLiterals *ql);
  virtual void fromXml_QLiterals(QLiterals *&ql, string str);
};

#endif // QUAL_AST_AUX_H
