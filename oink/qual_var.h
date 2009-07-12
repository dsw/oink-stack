// see License.txt for copyright and terms of use

// Variable_Q, a Qual-aware parallel to Variable

#ifndef QUAL_VAR_H
#define QUAL_VAR_H

#include "oink_var.h"           // Variable_O
// #include "oink_type_children.h" // CompoundType_O
// #include "astlist.h"            // ASTList

class Type;                     // cc_type.h
// class Variable_Q;
// class TranslationUnit;          // AST node
// class Value;                    // value.h
// class ValueVisitor;             // value.h

class Variable_Q : public Variable_O {
  public:
  friend class XmlValueWriter;
  friend class XmlValueReader;
  friend class XmlValueWriter_Q;
  friend class XmlValueReader_Q;
  explicit Variable_Q(XmlReader &);
  Variable_Q(SourceLoc L, StringRef n, Type *t, DeclFlags f);
  virtual ~Variable_Q() {}

  virtual void nameTree();
  virtual void setGlobalTree();
  virtual bool treeContainsQvars();
};

// cast with potential for runtime check in the future
inline Variable_Q *asVariable_Q(Variable *v) {
  return static_cast<Variable_Q*>(v);
}

inline Variable_Q const *asVariable_Q(Variable const *v) {
  return static_cast<Variable_Q const*>(v);
}

#endif // QUAL_VAR_H
