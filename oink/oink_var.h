// see License.txt for copyright and terms of use

// Variable_O, an Oink-aware parallel to Variable

#ifndef OINK_VAR_H
#define OINK_VAR_H

#include "variable.h"           // Variable
#include "oink_type_children.h" // CompoundType_O
#include "astlist.h"            // ASTList

class Type;                     // cc_type.h
class Variable_O;
class TranslationUnit;          // AST node
class Value;                    // value.h
class ValueVisitor;             // value.h

// if this is true, we do not insist that [abstract] Value-s be
// annotated only on Variable-s for which getReal() returns true.
// This is necessary to break a circular depedency: the pass that
// annotates setReal(true) on real (non-template) Variable-s happens
// after typechecking, but during typechecking compound initializers
// code needs values to be annotated onto Variables
extern bool omitCheckAbstrValueOnlyOnRealVars;

class Variable_O : public Variable {
  private:
  mutable Value *abstrValue0; // (owner) can't call it value, as that is a Variable field

  public:
  friend class XmlValueWriter;
  friend class XmlValueReader;
  explicit Variable_O(XmlReader &);
  Variable_O(SourceLoc L, StringRef n, Type *t, DeclFlags f);
  virtual ~Variable_O();

  public:
  // does this Variable have a non-NULL funcDefn?
  //
  // NOTE: the reason this boolean is needed may be non-obivous; why
  // not just look at the funcDefn field?  Because when we serialize,
  // we sometimes do not serialize the AST and so when de-serialized,
  // the funcDefn field will always be NULL whether or not it was
  // present before.
  //
  // NOTE: Elsa serializes this field as 'user1'; FIX: perhaps it
  // would be more intuitive for the user if we serialized it as
  // 'hasFuncDefn'.
  bool getHasFuncDefn() const { return getUser1(); }
  void setHasFuncDefn(bool b) { setUser1(b);       }

  // should this variable be kept?  NOTE: there is no need to
  // serialize this field and indeed any Variables that have this flag
  // set should not be serialized
  bool getFilteredKeep() const { return getUser2(); }
  void setFilteredKeep(bool b) { setUser2(b);       }
  // was this variable filtered out?  this is not just the inverse of
  // getFilteredKeep() as it takes into account that any non-real,
  // non-function variable is also not filtered out
  bool filteredOut() const;

  void checkOkForDataflow() const;
  Value *abstrValue() const;
  void setAbstrValue(Value *);
  bool hasAbstrValue() const { return abstrValue0 != NULL; }

  StringRef fullyQualifiedName_StrRef() /*const*/;
  StringRef mangledName_StrRef() /*const*/;
  StringRef fullyQualifiedMangledName_StrRef() /*const*/;
  bool canBeInherited() const;

  void traverse(ValueVisitor &vis);
};

// cast with potential for runtime check in the future
inline Variable_O *asVariable_O(Variable *v) {
  return static_cast<Variable_O*>(v);
}

inline Variable_O const *asVariable_O(Variable const *v) {
  return static_cast<Variable_O const*>(v);
}

bool serializeVar_O(Variable *var0);

#endif // OINK_VAR_H
