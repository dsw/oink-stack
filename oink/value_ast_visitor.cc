// see License.txt for copyright and terms of use

#include "value_ast_visitor.h"  // ValueASTVisitor
#include "oink_global.h"        // ValueFactory *valueFactory

// make values for every Expression
bool ValueASTVisitor::visitExpression(Expression *expr) {
  // FIX: why can this happen?
//    xassert(!expr->abstrValue);

  bool isE_variable0 = expr->isE_variable();
  // make sure that if this is a variable that it is a non-template
  // one
  if (isE_variable0) {
    xassert(expr->asE_variable()->var->getReal());
  }

  // FIX: There are Expressions without a Type.  18 april 2005 Scott
  // said he was fixing this but he didn't get them all.
  if (expr->type && !expr->abstrValue) {
    // optimization
    if (oinkCmd->merge_E_variable_and_var_values && isE_variable0) {
      // re-use the Variable's [abstract] Value rather than create our
      // own and merge it later in the dataflow
      Variable_O *var = asVariable_O(expr->asE_variable()->var);
      Value *varValue = var->abstrValue();
      bool const varIsRef = var->type->isReferenceType();
      bool const exprIsRef = expr->type->isReferenceType();
      if (exprIsRef == varIsRef) {
        expr->abstrValue = varValue;
      } else if (exprIsRef && !varIsRef) {
        expr->abstrValue = varValue->asLval();
      } else if (!exprIsRef && varIsRef) {
        xfailure("not sure this can happen");
      } else {
        xfailure("can't happen");
      }
    } else {
      expr->abstrValue = vFac->buildValue(expr->type, loc);
    }
  }
  return true;
}

bool ValueASTVisitor::visitFunction(Function *func) {
  xassert(func->funcType);
  Variable_O *var = asVariable_O(func->nameAndParams->var);
  xassert(var->getReal());
  // REMOVE_FUNC_DECL_VALUE: remove this line
  xassert(!func->abstrValue);
  if (var->filteredOut()) return false;
  // annotate with Value
  // REMOVE_FUNC_DECL_VALUE: remove this line
  func->abstrValue = vFac->buildValue(func->funcType, loc);
  // This is taken from ElabVisitor::elaborateFunctionStart()

  FunctionValue *ft = func->abstrValue->asFunctionValue();
  if (ft->retValue->t()->isCompoundType()) {
    ft->registerRetVar(func->retVar);
  }
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   // FIX: This was changed from doing the registerRetVar() on the
//   // Function->value which no longer exists, therefore this might be
//   // better done for all variables rather than just for those that
//   // that have a function definition; FIX: why is this not done for
//   // declarators!  If it should be done I think it should be done for
//   // declarators since Functions have Declarators but not the reverse.
//   FunctionValue *fv = asVariable_O(func->nameAndParams->var)->abstrValue()->asFunctionValue();
//   if (fv->retValue->t()->isCompoundType()) {
//     fv->registerRetVar(func->retVar);
//   }
  return true;
}

bool ValueASTVisitor::visitDeclarator(Declarator *decltor) {
  xassert(decltor->type);
  Variable_O *var = asVariable_O(decltor->var);
  xassert(var->getReal());
  // REMOVE_FUNC_DECL_VALUE: remove this line
  xassert(!decltor->abstrValue);
  if (var->filteredOut()) return false;
  // annotate with Value
  // REMOVE_FUNC_DECL_VALUE: remove this line
  decltor->abstrValue = vFac->buildValue(decltor->type, loc);
  return true;
}

bool ValueASTVisitor::visitPQName(PQName *n) {
  // SGM 2007-08-25: To correspond with the change I made to
  // RealVarAndTypeASTVisitor to not descend inside PQ_template, this
  // must also not look inside PQ_template.
  if (n->isPQ_template()) {
    return false;
  }
  
  return true;
}
