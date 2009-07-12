// see License.txt for copyright and terms of use

// Qual subclass of dataflow_visitor; this class inherits from the
// generic dataflow and modifies its behavior for purposes of the qual
// analysis.

#ifndef QUAL_DATAFLOW_VISITOR_H
#define QUAL_DATAFLOW_VISITOR_H

#include "oink_global.h"        // globalLang
#include "dataflow_visitor.h"   // DataFlowVisitor
#include "qual.h"               // Qual


class QualVisitor : public DataFlowVisitor {
  Qual &qm;
  public:
  QualVisitor(TranslationUnit *tunit, Qual &qm0, DataFlowEx &dfe0)
    : DataFlowVisitor(tunit, dfe0, globalLang)
    , qm(qm0)
  {}

  // the functions below are grouped by the additional aspect of
  // dataflow that the qual analysis concerns itself with

  // intercept functions
  virtual bool visitFunction_once_initial(Function *obj);

  // intercept declarators
  virtual bool visitDeclarator(Declarator *obj);

  // intercept the usual cast mechanism
  bool subVisitE_cast(E_cast *obj);
  bool subVisitE_keywordCast(E_keywordCast *obj);

  // annotate operators in prelude files; FIX: this is pre-C++; remove
  // this mechanism and do it with annotated global overloaded
  // operators
  DeepLiteralsList *getGlobalDeepLiteralsList(Variable_O *func_var);
  void applyOpDeref(Value *value, Value *unrefed_ptr);
  void applyOpGt(Value *value, Value *lhsValue, Value *rhsValue);
  virtual bool subVisitE_binary(E_binary *obj);
  virtual bool subVisitE_deref(E_deref *obj);

  // annotate modified objects, such as the target of assignments,
  // with $nonconst; this is pre-C++; remove this mechanism and do it
  // with annotated global overloaded operators
  void make_nonconst(SourceLoc loc, Value *tgt);
  virtual bool subVisitS_change_type(S_change_type *obj);
  virtual bool subVisitE_effect(E_effect *obj);
  virtual bool subVisitE_assign(E_assign *obj);

  // attach qualifiers to thrown objects
  void insertAllLevelsWithQvar
    (Value *src_value, LibQual::Type_qualifier *tgt_qv, SourceLoc loc, bool reverse);
  virtual bool subVisitE_throw(E_throw *obj);
  virtual bool visitHandler(Handler *obj);
};

#endif // QUAL_DATAFLOW_VISITOR_H
