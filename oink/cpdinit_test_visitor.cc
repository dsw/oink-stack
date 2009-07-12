// see License.txt for copyright and terms of use

#include "cpdinit_test_visitor.h" // this module
#include "cpdinit_test_clbk.h"
#include "LibCpdInit/cpdinit.h"
#include "oink_global.h"        // tFac

extern char const *cpdTestGlobalCurVarName; // cpdinit_test.cc

bool CpdInitTestVisitor::visitDeclarator(Declarator *decl) {
  xassert(decl);
  xassert(decl->var);
  cpdTestGlobalCurVarName = decl->var->name;
  if (IN_compound *ci = dynamic_cast<IN_compound*>(decl->init)) {
    // FIX: ? I'm not using the global representative the way the
    // qual_walk.cc does.
    // FIX: I don't think I need getInstanceSpecificValue() here
    compoundInit(NULL, SL_UNKNOWN, decl->var, ci,
                 oneAssignmentCpdTest, reportUserErrorCpdTest);
  }
  return true;
}

bool CpdInitTestVisitor::visitExpression(Expression *expression) {
  cpdTestGlobalCurVarName = "__constr_expr_0";
  if (E_compoundLit *cl = dynamic_cast<E_compoundLit*>(expression)) {
    //  bool CpdInitTestVisitor::visitE_compoundLit(E_compoundLit *cl) {
//      cout << "**************** visitE_compoundLit" << endl;
    //    xfailure("We don't test compond lits for now since don't have the var name locally; "
    //             "Also, I wonder if CIL deals with them.");
    Value *value = cl->getAbstrValue();
    Variable_O *tmpVar =
      asVariable_O(tFac->makeVariable
                   (value->loc, "<CpdInitTestVisitor tempVar>", value->t(), DF_NONE));
    tmpVar->setReal(true);
    tmpVar->setFilteredKeep(true);
    tmpVar->setAbstrValue(value);
    compoundInit(NULL, SL_UNKNOWN, tmpVar, cl->init,
                 oneAssignmentCpdTest, reportUserErrorCpdTest);
  }
  return true;
}
