// see License.txt for copyright and terms of use

#include "qual_dataflow_visitor.h" // this module
#include "qual_annot.h"         // QualAnnot
#include "qual_global.h"        // qualCmd
#include "qual_value_children.h"

// FIX: this is questionable
#include "qual_dataflow_ty.h"   // mkLeq

#include "oink_util.h"

bool QualVisitor::visitFunction_once_initial(Function *obj) {
  // delegate to superclass
  if (!DataFlowVisitor::visitFunction_once_initial(obj)) return false;

  // this is so int $tainted $untainted f(); causes a failure; an
  // obscure feature at best
  treeEnsureQvIfUserQliterals(obj->abstrValue);
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   treeEnsureQvIfUserQliterals(asVariable_O(obj->nameAndParams->var)->abstrValue());

  return true;
}

// ****

bool QualVisitor::visitDeclarator(Declarator *obj) {
  // delegate to superclass
  if (!DataFlowVisitor::visitDeclarator(obj)) return false;

  // REMOVE_FUNC_DECL_VALUE: remove this
  Value *declValue = obj->abstrValue;
  Value *varValue = asVariable_O(obj->var)->abstrValue();

//   if (declValue->isFunctionValue() && declValue->asFunctionValue()->ellipsis &&
//       !treeContainsFunkyQliterals(declValue)) {
//     // FIX: I'm just tired of seeing this warning.
// //      userReportWarning(declValue->loc,
// //                   "vararg function does not have polymorphic type");
//   }

  // ensure that polymorphic qualifiers are attached to the Declarator
  // value
  if (declValue->isFunctionValue()) {
    qa(declValue->asFunctionValue())->ensureFunkyInternalEdges(getLoc(), dfe);
  }
  // REMOVE_FUNC_DECL_VALUE; replace with this
//   if (varValue->isFunctionValue()) {
//     qa(varValue->asFunctionValue())->ensureFunkyInternalEdges(loc, dfe);
//   }

  // this is so int $tainted $untainted x; causes a failure; an
  // obscure feature at best
  treeEnsureQvIfUserQliterals(declValue);
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   treeEnsureQvIfUserQliterals(varValue);

  // don't think I need this
//   treeEnsureQvIfUserQliterals(varValue);

  // if the declaration has qualifiers, we want those to make it onto
  // the variable
  // REMOVE_FUNC_DECL_VALUE: remove this
  if (declValue->isFunctionValue() && treeContainsQliterals(declValue)) {
    dfe.eDataFlow_refUnify(declValue, varValue, obj->var->loc);
  }

  return true;
}

// ****

bool QualVisitor::subVisitE_cast(E_cast *obj) {
  bool ret = true;
//    cout << "qualCmd->casts_preserve " << qualCmd->casts_preserve << endl;
  // NOTE: This functionality is now implemented differently.
//    if (qualCmd->casts_preserve &&
//        // Jeff says any qualifier should prevent the cast, not just
//        // non-funky ones, so don't do this:
//        // !asValue_Q(value)->containsUserQliterals()
//        !qa(obj->value)->getQl()) {
//   string locStr = sourceLocManager->getString(getLoc());
  if (!treeContainsQliterals(obj->abstrValue)) {
    // cout << "YES to cast edge at " << locStr << endl;
    // delegate to the superclass
    ret = DataFlowVisitor::subVisitE_cast(obj);
  } else {
    // cout << "NO to cast edge at " << locStr << endl;
  }
  return ret;
}

bool QualVisitor::subVisitE_keywordCast(E_keywordCast *obj) {
  bool ret = true;
  // NOTE: This functionality is now implemented differently.
//    if (qualCmd->casts_preserve &&
//        // Jeff says any qualifier should prevent the cast, not just
//        // non-funky ones, so don't do this:
//        // !asValue_Q(value)->containsUserQliterals()
//        !qa(obj->value)->getQl()) {
//   if (!qa(obj->abstrValue)->getQl()) {

  if (!treeContainsQliterals(obj->abstrValue)) {
    // delegate to the superclass
    ret = DataFlowVisitor::subVisitE_keywordCast(obj);
  }
  return ret;
}

// ****

DeepLiteralsList *QualVisitor::getGlobalDeepLiteralsList(Variable_O *func_var) {
  DeepLiteralsList *dll = qm.globalDeepLiteralsLists.get(globalStrTable(func_var->name));
  if (!dll) {
    xassert(func_var);
    // FIX: not sure this should be an abstrValue
    Value *declared_extern_func = func_var->abstrValue();
    xassert(declared_extern_func);
    dll = new DeepLiteralsList(declared_extern_func);
    qm.globalDeepLiteralsLists.add(func_var->name, dll);
  }
  return dll;
}

// insert any inference edges for _op_deref
void QualVisitor::applyOpDeref(Value *value, Value *unrefed_ptr) {
  string _op_deref = "_op_deref";
  if (Variable_O *f_deref_var = findGlobalDecl(_op_deref, getLoc())) {
    // get _op_deref and check it
    xassert(f_deref_var);
    xassert(f_deref_var->abstrValue()->isFunctionValue());
    FunctionValue *f_deref = f_deref_var->abstrValue()->asFunctionValue();
    xassert(f_deref);
    xassert(f_deref->params->count() == 1);
    xassert(!f_deref->type->isMethod());
    xassert(!f_deref->type->acceptsVarargs());
    xassert(streq(f_deref_var->name, _op_deref));
    DeepLiteralsList *deref_dll = getGlobalDeepLiteralsList(f_deref_var); //_op_deref, loc);
    xassert(deref_dll);

    // make a dummy function to which to attach
    xassert(f_deref->type->isFunctionType());
    FunctionValue *f_deref_clone = vFac->buildValue(f_deref->type, getLoc())->asFunctionValue();
    // FunctionValue *f_deref_clone = vFac->buildFunctionValue(f_deref->type, loc);
    xassert(f_deref_clone->params->count() == 1);

    // do surgery on the function
    // FIX: This should be fixed for references.
    f_deref_clone->retValue = value->asRval();// FIX: old value becomes garbage
    asVariable_O(f_deref_clone->params->nth(0))->
      setAbstrValue(unrefed_ptr); // FIX: old value becomes garbage

    // attach the literals
    deref_dll->attach(f_deref_clone->asFunctionValue());
    // actually, I don't know if I can do this; FIX: garbage.
    //      delete f_deref_clone;       // FIX: not exn safe
  }
}

// insert any edges for _op_gt
void QualVisitor::applyOpGt(Value *value, Value *lhsValue, Value *rhsValue) {
  string _op_gt = "_op_gt";
  if (Variable_O *f_gt_var = findGlobalDecl(_op_gt, getLoc())) {
    xassert(f_gt_var);
    xassert(f_gt_var->abstrValue()->isFunctionValue());
    FunctionValue *f_gt = f_gt_var->abstrValue()->asFunctionValue();
    xassert(f_gt);
    xassert(f_gt->params->count() == 2);
    xassert(!f_gt->type->isMethod());
    xassert(!f_gt->type->acceptsVarargs());
    xassert(streq(f_gt_var->name, _op_gt));
    DeepLiteralsList *gt_dll = getGlobalDeepLiteralsList(f_gt_var);//_op_gt, loc);
    xassert(gt_dll);
    // make a dummy function to which to attach
    xassert(f_gt->type->isFunctionType());
    // FunctionValue *f_gt_clone = vFac->buildFunctionValue(f_gt->type, loc);
    FunctionValue *f_gt_clone = vFac->buildValue(f_gt->type, getLoc())->asFunctionValue();
    xassert(f_gt_clone->params->count() == 2);
    // FIX: This should be fixed for references.
    f_gt_clone->retValue = value; // FIX: old value becomes garbage
    asVariable_O(f_gt_clone->params->nth(0))->
      setAbstrValue(lhsValue); // FIX: old value becomes garbage
    asVariable_O(f_gt_clone->params->nth(1))->
      setAbstrValue(rhsValue); // FIX: old value becomes garbage
    gt_dll->attach(f_gt_clone->asFunctionValue());
    // actually, I don't know if I can do this; FIX: garbage.
//      delete f_gt_clone;       // FIX: not exn safe
  }
}

bool QualVisitor::subVisitE_binary(E_binary *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::subVisitE_binary(obj);

  xassert(obj->abstrValue != obj->e1->abstrValue);
  xassert(obj->abstrValue != obj->e2->abstrValue);
  Value *lhsValue = obj->e1->abstrValue->asRval();
  Value *rhsValue = obj->e2->abstrValue->asRval();

  if (obj->op == BIN_GREATER) applyOpGt(obj->abstrValue, lhsValue, rhsValue);
  return ret;
}

bool QualVisitor::subVisitE_deref(E_deref *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::subVisitE_deref(obj);

  Value *unrefed_ptr = obj->ptr->abstrValue->asRval();
  applyOpDeref(obj->abstrValue, unrefed_ptr);
  return ret;
}

// ****

void QualVisitor::make_nonconst(SourceLoc loc, Value *tgt) {
  Value *t = tgt->asRval();
  qa(t)->attachOneLiteralIfInLattice(loc, "$nonconst");
  // if we are a compound value, mark all of our member variables as
  // $nonconst
  if (t->isCVAtomicValue()) {
    CVAtomicValue *cvat = t->asCVAtomicValue();
    xassert(cvat);
    if (cvat->type->atomic->isCompoundType()) {
      CompoundType *compound = cvat->type->atomic->asCompoundType();
      SObjListIter<Variable> varIter(compound->getDataVariablesInOrder());
      for(; !varIter.isDone(); varIter.adv()) {
        Variable *v0 = const_cast<Variable*>(varIter.data());
        make_nonconst(loc, asVariable_O(v0)->abstrValue());
      }
    }
  }
}

bool QualVisitor::subVisitS_change_type(S_change_type *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::subVisitS_change_type(obj);

  make_nonconst(getLoc(), obj->e->expr->abstrValue);
  return ret;
}

bool QualVisitor::subVisitE_effect(E_effect *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::subVisitE_effect(obj);

  xassert(obj->expr->abstrValue->isReferenceValue());
  make_nonconst(getLoc(), obj->expr->abstrValue);
  return ret;
}

bool QualVisitor::subVisitE_assign(E_assign *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::subVisitE_assign(obj);

  // should not get targets here which are compound types or
  // references to such, since it should have been resolved into a
  // call to an operator assign, which always exists implicitly.
  if (lang.isCplusplus) {
    xassert(!obj->target->abstrValue->asRval()->t()->isCompoundType());
  }

  Value *tgt = obj->target->abstrValue;
  // remove l-value layer of indirection for ASSIGNMENT (not for
  // INITIALIZATION).
  xassert(tgt->isReferenceValue());
  Value *at = tgt->asRval();
  make_nonconst(getLoc(), at);

  return ret;
}

// ****

// special insertion for pairing up all levels of a value with a qvar
void QualVisitor::insertAllLevelsWithQvar
  (Value *src_value, LibQual::Type_qualifier *tgt_qv, SourceLoc loc,
   bool reverse) {
  Value *t0 = src_value;
  while(t0) {
    if (reverse) mkLeq(tgt_qv, qa(t0)->getQVar(), loc);
    else mkLeq(qa(t0)->getQVar(), tgt_qv, loc);
    switch(t0->getTag()) {
    default: xfailure("illegal tag"); break;
    case Value::V_ATOMIC: case Value::V_FUNCTION:
      t0 = NULL;
      break;
    case Value::V_POINTER:
    case Value::V_REFERENCE:
    case Value::V_ARRAY:
    case Value::V_POINTERTOMEMBER:
      t0 = t0->getAtValue();
      break;
    }
  }
}

bool QualVisitor::visitHandler(Handler *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::visitHandler(obj);

  if (obj->globalVar) {
    insertAllLevelsWithQvar(asVariable_O(obj->globalVar)->abstrValue(),
                            qm.thrownQv, getLoc(),
                            true // reverse
                            );
  } else {
    xassert(!obj->globalDtorStatement);
    insertAllLevelsWithQvar(asVariable_O(obj->typeId->decl->var)->abstrValue(), qm.thrownQv,
                            getLoc(),
                            true // reverse
                            );
  }
  return ret;
}

bool QualVisitor::subVisitE_throw(E_throw *obj) {
  // delegate to superclass
  bool ret = DataFlowVisitor::subVisitE_throw(obj);

  // FIX: There are implicit copy ctors going on here
  if (obj->globalCtorStatement) {
    // insert edge from globalVar->value to qm.thrownQv.
    insertAllLevelsWithQvar(asVariable_O(obj->globalVar)->abstrValue(),
                            qm.thrownQv, getLoc(),
                            false // not reverse
                            );
  }
  // NOTE: yes, we do not insert the edge here if there is both a
  // globalCtorStatement and an expr; the globalCtorStatement takes
  // priority
  else if (obj->expr) {
    insertAllLevelsWithQvar(obj->expr->abstrValue, qm.thrownQv, getLoc(),
                            false // not reverse
                            );
//    } else {                      // re-throw
//      // For now there is nothing to do here since all throws go to all
//      // catches, this adds nothing to the analysis.
  }
  return ret;
}
