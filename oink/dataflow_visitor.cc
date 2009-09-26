// see License.txt for copyright and terms of use

#include "dataflow_visitor.h"   // this module
#include "cc_env.h"             // isCopyAssignOp, isCopyConstructor
#include "oink_global.h"        // oinkCmd
#include "dataflow_cpdinit_clbk.h" // oneAssignmentDataFlow, reportUserErrorDataFlow
#include "oink_util.h"
#include "warning_mgr.h"        // WarningManager


Variable_O *DataFlowVisitor::findGlobalDecl(string const &name, SourceLoc loc) {
  // check the local translation unit; NOTE: we omit the initial "::"
  // here, since it is a scope lookup, not a global map lookup
  return asVariable_O(tunit->globalScope->rawLookupVariable(globalStrTable(name)));
}

bool isIntegerType_or_EnumType(Type *t) {
  return t->isIntegerType() || t->isEnumType();
}

// --------------------- TopForm ---------------------
bool DataFlowVisitor::visitTopForm(TopForm *obj) {
  switch (obj->kind()) {        // roll our own virtual dispatch
  default: break;               // statement kinds we don't cover
  case TopForm::TF_ASM: return subVisitTF_asm(obj->asTF_asm());
  }
  return true;
}

bool DataFlowVisitor::subVisitTF_asm(TF_asm *) {
  // use the same WarningManager as for S_ASM
  subVisitS_asm(NULL);
  return true;
}


// --------------------- Function -----------------
// do what 'Function::ctorReceiver' used to do
Variable *getCtorReceiver(Function *f)
{
  if (f->receiver &&
      f->funcType->isConstructor()) {
    return f->receiver;
  }
  else {
    return NULL;
  }
}

bool DataFlowVisitor::visitFunction_once_initial(Function *obj) {
  return true;
}

bool DataFlowVisitor::visitFunction_once_decl(Function *obj) {
  // Unify the Function::funcType with the type of the
  // Declarator::nameAndParams; they should be isomorphic types
  xassert(obj->nameAndParams->type->isFunctionType());
  xassert(obj->funcType->isFunctionType());
  xassert(obj->funcType->equals(obj->nameAndParams->type));
  // REMOVE_FUNC_DECL_VALUE: remove this
  dfe.eDataFlow_refUnify(obj->nameAndParams->abstrValue,
                         obj->abstrValue,
                         obj->nameAndParams->getLoc());

  // also have to unify pointwise all the abstrValues of the Variables
  // in the obj->abstrValue and the obj->type since any E_variables in
  // the body of the function will refer to the parameters in the
  // obj->type; Note that we do *not* unify the obj->abstrValue and
  // the obj->type.
  FunctionValue *f0 = obj->abstrValue->asRval()->asFunctionValue();
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   FunctionValue *f0 = asVariable_O(obj->nameAndParams->var)->abstrValue()
//     ->asRval()->asFunctionValue();

  // Yes I want the TYPE here not the VALUE
  FunctionType *f1 = obj->funcType->asRval()->asFunctionType();
  // iterate over the two parameters lists in parallel
  SObjListIterNC<Variable_O> f0Params(*f0->params);
  SObjListIterNC<Variable> f1Params(f1->params);
  while(true) {
    if (f0Params.isDone()) break;
    // Now that the number of parameters in a FunctionValue that
    // hasFlag(FF_NO_PARAM_INFO) can increase as that FunctionValue
    // has function calls to it occuring, it is possible that f1Params
    // will run out before f0Params does.  If so, there is nothing
    // really to do but to stop.  I think.
//     xassert(!f1Params.isDone() && "e9d062c2-5446-4a9e-9ec3-dc41298d2b7d");
    if (f1Params.isDone()) break;

    Variable_O *f0var = f0Params.data();
    Variable_O *f1var = asVariable_O(f1Params.data());
    dfe.eDataFlow_refUnify(f0var->abstrValue(), f1var->abstrValue(), getLoc());
    f0Params.adv();
    f1Params.adv();
  }
  xassert(f1Params.isDone());

  return true;
}

bool DataFlowVisitor::visitFunction_once_body(Function *obj) {
  // Special case: "The whole is greater than the sum of the parts".
  // The problem is that operator assign for a class ought to flow
  // qualifiers from the _whole_ argument class type to the _this_
  // variable.
  //
  // FIX: Perhaps I shouldn't be doing this with the getName() method,
  // but digging down through possible layers of PQName-s to get to a
  // possible PQ_operator at the bottom just didn't seem like the
  // right thing to do here; the digging should be done by the PQName
  // classes.  Well, we already have a method like that: getName(), so
  // I'll use it.
  FunctionValue *funcValue = obj->abstrValue->asFunctionValue();
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   FunctionValue *funcValue = asVariable_O(obj->nameAndParams->var)->abstrValue()
//     ->asFunctionValue();

  SourceLoc loc = funcValue->getLoc();
  StringRef funcName = obj->nameAndParams->decl->getDeclaratorId()->getName();
  // is it an assignment operator?
  if (streq("operator=", funcName)) {
    // there should be exactly 2 parameters: _this_ and one argument
    xassert(funcValue->params->count() == 2);
    xassert(obj->funcType->getReceiver() == obj->receiver);
    Value *receiver = asVariable_O(funcValue->type->getReceiver())->abstrValue();
    xassert(receiver->isReferenceValue());
    if (isCopyAssignOp(obj->nameAndParams->var, receiver->asRval()->t()->asCompoundType())) {
      Value *otherParam = asVariable_O(funcValue->params->nth(1))->abstrValue();
      // flow the second parameter into the first; the
      // asLval() here is necessary since the parameter is
      // allowed to be by value
      dfe.eDataFlow_normal(otherParam->asLval(), receiver, loc);
    }
  }

  // is it a ctor?
  else if (getCtorReceiver(obj)) {
    // If it looks like a copy ctor, we put in the "whole" edge, as
    // above.
    Value *thisLocalVar = asVariable_O(getCtorReceiver(obj))->abstrValue();
    xassert(thisLocalVar->isReferenceValue());
    if (isCopyConstructor(obj->nameAndParams->var,
                          thisLocalVar->asRval()->t()->asCompoundType())
        ) {
      // NOTE: since ctors do not have a _this_ param, the true
      // argument is the first (0th).
      Value *otherParam = asVariable_O(funcValue->params->nth(0))->abstrValue();
      // can't pass by value into a copy ctor; it would be an infinite
      // loop!
      xassert(otherParam->isReferenceValue());
      dfe.eDataFlow_normal(otherParam, thisLocalVar, loc);
    }
  }

  // If it is a ctor put in an edge from retVar to ctorReceiver.
  // "this(retVar)" (an initialization not an assignment); FIX: when
  // 'this' goes back to being a pointer, it will semantically be like
  // "this = &retVar"
  if (getCtorReceiver(obj)) {
    Variable_O *ctorReceiver = asVariable_O(getCtorReceiver(obj));
    xassert(ctorReceiver->getType()->isReferenceType());
    Variable_O *retVar0 = funcValue->getRetVar();
    xassert(retVar0);
    Value *retVarValue = retVar0->abstrValue();
    xassert(retVarValue->isReferenceValue());
    dfe.eDataFlow_normal(retVarValue,
                         ctorReceiver->abstrValue(),
                         ctorReceiver->loc); // FIX: location could be better
  }

  return true;
}

bool DataFlowVisitor::visitFunction(Function *obj) {
  xassert(!obj->instButNotTchecked());
  Variable_O *var = asVariable_O(obj->nameAndParams->var);
  TemplateInfo *tinfo = var->templateInfo();
  if (tinfo) {
    xassert(tinfo->isCompleteSpecOrInstantiation());
  }

  if (var->filteredOut()) return false; // prune

  if (!visitFunction_once_initial(obj)) return false; // prune

  // ignore this Function if we should
  if (controls && controls->isIgnored(var)) {
    std::cout << sourceLocManager->getString(var->loc)
              << " ignoring function body since in ignore file: "
              << var->name
              << std::endl;
    return false;               // prune
  }

  // Run a sub traverse here on every field of Function and then
  // return false to prune the whole walk.
  obj->retspec->traverse(this->loweredVisitor);
  obj->nameAndParams->traverse(this->loweredVisitor);

  if (!visitFunction_once_decl(obj)) return false; // prune

  // Not sure that this is the right place.  Perhaps it should include
  // the initializer list and the exception handlers.
  // 7sept2003: yes, it should
  functionStack.push(obj);
  if (!visitFunction_once_body(obj)) return false; // prune

  // Visit the 'greater' function body
  FAKELIST_FOREACH_NC(MemberInit, obj->inits, iter) {
    iter->traverse(this->loweredVisitor);
  }
  obj->body->traverse(this->loweredVisitor);
  FAKELIST_FOREACH_NC(Handler, obj->handlers, iter) {
    iter->traverse(this->loweredVisitor);
  }
  // if it is a dtor, visit the implicit superclass and member dtors
  if (obj->dtorStatement) {
    obj->dtorStatement->traverse(this->loweredVisitor);
  }

  functionStack.pop();

  // PRUNE, because we handled all the children above in the sub walk
  return false;
}

bool DataFlowVisitor::visitMemberInit(MemberInit *obj) {
  if (obj->member) {
    Value *memberValue0 = asVariable_O(obj->member)->abstrValue();
    if (memberValue0->t()->isCompoundType()) {
      // dsw: Due to a strange combination of hypotheticality, we must
      // do things this way.  What can happen is that a class B can have
      // a ctor but not a no-arg ctor, which means the default no-arg
      // ctor does not get created by default.  Now, if another class A
      // has an embedded member of type B, then a no-arg ctor for A
      // would call the no-arg ctor for B, but that doesn't exist and
      // therefore the no-arg ctor for A would fail to typecheck (I
      // think).  However, if A has no user-defined ctors at all, then
      // the elsa code will make a no-arg ctor for it, since when I
      // wrote the code that does that I did not bother to honor the
      // spec exactly as it was too complex.  Therefore you can end up
      // in a situation here where there is a compound type B but no
      // no-arg ctor on B to call; this is an error, but we are down in
      // the datafow for the no-arg ctor for A, which also is not
      // supposed to exist and therefore in reality is never called in
      // the code.  Thus, it doesn't much matter what we do as long as
      // we don't fail an assertion.
      if (obj->args->count()) {
        xassert(obj->ctorStatement);
      }
    } else {
      xassert(!obj->ctorStatement);
      // MemberInit for a simple type such as an int that does not have
      // an implicit ctor.
      //
      // FIX: make this instance-specific; (did I mean to imitate this?)
      //        obj->receiver->type->asRval()->asCVAtomicValue()->getInstanceSpecificValue();
      int argCount = obj->args->count();
      USER_ASSERT(argCount<=1, getLoc(),
                  "Non-compound type must have zero-arg or one-arg MemberInit"
                  " (c5826071-1097-43c5-9388-e65f6d852db2)");
      if (argCount == 1) {
        dfe.eDataFlow_normal(obj->args->first()->expr->abstrValue, memberValue0, getLoc());
      } else {
        // otherwise, I can't think of any data to flow
      }
    }
  } else {
    // initializing a superclass??
  }
  return true;
}

bool DataFlowVisitor::visitDeclarator(Declarator *obj) {
  // REMOVE_FUNC_DECL_VALUE: remove this
  Value *declValue = obj->abstrValue;

  Variable_O *var = asVariable_O(obj->var);

  if (var->filteredOut()) return false;

  Value *varValue = var->abstrValue();
  Value *varValue_rval = varValue->asRval();

  // quarl 2006-07-13
  //    Handle attribute((alias("target"))) (gnu extension) by adding dataflow
  //    from source to target as if we're calling it.
  if (D_attribute *attr = dynamic_cast<D_attribute*>(obj->decl)) {
    StringRef aliasTargetName = attr->getAlias();
    if (aliasTargetName) {
      // TODO: look up the mangled name.  But since the user wouldn't know oink
      // mangling anyway there's no point for now, just assume mangled name is
      // the same.
      Variable_O *aliasTarget = findGlobalDecl(aliasTargetName, getLoc());
      if (aliasTarget) {
        // dataflow to alias target as if we're calling it.
        dfe.eDataFlow_normal(aliasTarget->abstrValue()->asRval(),
                             varValue_rval, getLoc());
        var->setHasFuncDefn(true);
      } else {
        userReportWarning(getLoc(),
                          "can't find alias target '%s' for '%s'", aliasTargetName,
                          var->name);
      }
    }
  }

  if (declValue->isFunctionValue()) {
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   if (varValue->isFunctionValue()) {
    // NOTE: for data types we do NOT unify the declarator value with
    // the variable value; for them we now avoid using data Declarator
    // values at all.  UPDATE: I think the declarator/var unification
    // we do here is only necessary for declarators at function
    // definition sites, so that the variables in the function *body*,
    // which are the parameters in the *declarator* value get hooked
    // up with the parameters in the function *variable* value; hence
    // we add the check for the Declarator::context being DC_FUNCTION.
    //
    // REMOVE_FUNC_DECL_VALUE: remove this whole 'if'-block
    if (obj->context==DC_FUNCTION) {
      dfe.eDataFlow_refUnify(declValue, varValue, var->loc);
//     } else {
// //       USER_REPORT_COMMON
// //         (loc, "avoiding unifying declarator and var because we are not in a function definition",
// //          NULL);
    }

    // If we are virtual, assign ourselves to the vtable entires for
    // all the functions we override; NOTE: if the variable is a
    // method of a templatized class, don't put in the edge if it
    // doesn't have a funcDefn.  Unused such methods do not get
    // instantiated in C++ and so we don't want this method to look
    // like it was used in the dataflow or we will get a linking
    // error; see Test/virtual6.cc
    if (var->virtuallyOverride && !var->isUninstClassTemplMethod()) {
      xassert(var->hasFlag(DF_VIRTUAL));
      for(SObjSetIter<Variable*> iter(*var->virtuallyOverride);
          !iter.isDone();
          iter.adv()) {
        Variable_O *elt = asVariable_O(iter.data());
        if (elt->filteredOut()) continue;
        if (elt->isUninstClassTemplMethod()) continue;
        // NOTE: We do not unify, but only flow in one direction, up;
        // if we were to assign at the reference level, they would be
        // unified and that would be an assignment in both directions
        dfe.eDataFlow_normal(varValue_rval, asVariable_O(elt)->abstrValue()->asRval(), getLoc());
      }
    }
  } else {
    xassert(!varValue->isFunctionValue());
  }

  // these are not visited in a visitInitializer() because they need
  // access to obj->var (the Declarator's var)
  if (obj->init) {
    if (IN_ctor *ctor = dynamic_cast<IN_ctor*>(obj->init)) {
      if (!obj->ctorStatement) {
        // IN_ctor for a simple type that does not have an implicit ctor,
        // such as an int.
        xassert(!varValue->t()->isCompoundType());
        USER_ASSERT(ctor->args->count()==1, getLoc(),
                    "Non-compound type must have one-arg MemberInit"
                    " (c2a8e1ce-0e51-4b58-bf68-82cf652c2345)");
        dfe.eDataFlow_normal(ctor->args->first()->expr->abstrValue, varValue, getLoc());
      }
      // otherwise, the ctorStatement was handled above.
    } else if (IN_expr *expr = dynamic_cast<IN_expr*>(obj->init)) {
      if (!obj->ctorStatement) {
        // This is wrong, it could be calling a ctor.  Check for the
        // ctor and do a function call instead here if there is one.
        // 9sep2003: update: Scott isn't sure about the semantics of
        // IN_expr; keep doing this for now
        //
        // UPDATE: the ctorStatement above handles this
        //
        // UPDATE: FIX: GATED_EDGE: I need to omit this edge for the
        // special case of a string literal being used to initialize
        // an array.  The real answer is that a constructor for the
        // array should be called with the string literal that
        // *copies* it to the array; note that while a string literal
        // is an array of 'char const'-s it can be used to initialize
        // an array of (nonconst) chars.  That is, this is fine;
        // 'fmt[0]' is writable:
        //
        //   static char fmt[80] = "abort %d (%x); MSCP free pool: %x;";
        // however this is not; it is unspecified what happens if you
        // write 'fmt[0]':
        //   static char *fmt = "abort %d (%x); MSCP free pool: %x;";

        // 2006 May 26: dsw: Scott says that expr->e is the right
        // thing to use because even though sometimes the template
        // processing code will remove it, it should be put back when
        // it is used at the call site; if it is not there, that means
        // it was not used.
        if (expr->e) {
          xassert(expr->e && "3391b39c-5527-4546-a0f1-dc2cbff50f64");
//          if (! (expr->e->isE_stringLit() && varValue->isStringType()) ) {
          if (! (expr->e->isE_stringLit()) ) {
            dfe.eDataFlow_normal(expr->e->abstrValue, varValue, getLoc());
          }
        }
      }
      if (var->type->isArrayType()) {
        if (expr->e) {
          USER_ASSERT(expr->e->type->asRval()->isArrayType(), var->loc,
                      "Initializer for array must be an array.");
        }
      }
    } else if (IN_compound *ci = dynamic_cast<IN_compound*>(obj->init)) {
      // FIX: I think it is more elegant and local here to use the
      // declType.
      // Scott puts this in, though he doesn't say that he knows its
      // right, so I'll let it go through.
//        xassert(!obj->ctorStatement);

      try {
        compoundInit(&dfe, getLoc(), var, ci,
                     oneAssignmentDataFlow, reportUserErrorDataFlow);
      } catch (UserError &err) {
        // dsw: there are some compound initializers situations
        // where gcc just gives a warning instead of an error, such
        // as if you put too many elements into an array:
        // oink/Test/too_many_initializers1.c; here I turn the error
        // into a warning so we can at least process such files
        userReportWarning
          (getLoc(), "Unsoundness warning: above error turned into a warning");
      }
    } else xfailure("can't happen");
  }

  return true;
}

// ---------------------- Statement ---------------------
bool DataFlowVisitor::visitStatement(Statement *obj) {
  switch (obj->kind()) {        // roll our own virtual dispatch
  default: break;               // statement kinds we don't cover
  case Statement::S_ASM:         return subVisitS_asm(obj->asS_asm());
  case Statement::S_RETURN:      return subVisitS_return(obj->asS_return());
  case Statement::S_CHANGE_TYPE: return subVisitS_change_type(obj->asS_change_type());
  }
  return true;
}


bool DataFlowVisitor::subVisitS_asm(S_asm *) {
  static WarningManager wm("Unsoundness warning: inline assembly is not analyzed in the dataflow");
  wm.emit(getLoc());
  return true;
}


bool DataFlowVisitor::subVisitS_return(S_return *obj) {
  // NOTE: we do not insert the edge here if there is both a
  // ctorStatement and an expr; the ctorStatement takes priority
  if (!obj->ctorStatement && obj->expr) {
    Value *retValue0 = functionStack.top()->abstrValue->asFunctionValue()->retValue;
    // REMOVE_FUNC_DECL_VALUE: replace with this
//     Value *retValue0 = asVariable_O(functionStack.top()->nameAndParams->var)->abstrValue()
//       ->asFunctionValue()->retValue;

    // we should not have gotten here if the return type of the
    // function is a CompoundType
    xassert(! (retValue0->t()->isCompoundType() &&
               oinkCmd->do_elaboration &&
               lang.isCplusplus)
            );
    FullExpression *fexpr = obj->expr;
    dfe.eDataFlow_normal(fexpr->expr->abstrValue, retValue0, getLoc());
  }
  return true;   // sm: I think this should be false...
}

bool DataFlowVisitor::subVisitS_change_type(S_change_type *obj) {
  return true;
}

// -------------------------------------------------------
bool DataFlowVisitor::visitHandler(Handler *obj) {
  if (obj->globalVar) {
    Value *src0 = NULL;
    if (obj->localArg) {
      // put in the edge from the localArg "argument" to the Handler's
      // Declarators's var "parameter", except we don't have to deal
      // with the possibility of polymorphism, so forget about
      // fcId etc.
      src0 = obj->localArg->abstrValue;
    } else {
      src0 = asVariable_O(obj->globalVar)->abstrValue();
      xassert(!src0->isReferenceValue());
      src0 = src0->asLval();
    }
    xassert(src0->isReferenceValue());
    xassert(src0->asRval()->t()->isCompoundType());
    Value *tgt0 = asVariable_O(obj->typeId->decl->var)->abstrValue()
      ->asLval();    // the idempotency of asLval() is important!
    dfe.eDataFlow_normal(src0, tgt0, getLoc());
  } else {
    xassert(!obj->globalDtorStatement);
  }
  return true;
}

// ---------------------- Expression ---------------------
void DataFlowVisitor::postvisitExpression(Expression *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case Expression::E_COMPOUNDLIT: subVisitE_compoundLit(obj->asE_compoundLit()); break;
  case Expression::E_THIS:        subVisitE_this(obj->asE_this());               break;
  case Expression::E_VARIABLE:    subVisitE_variable(obj->asE_variable());       break;
  case Expression::E_FUNCALL:     subVisitE_funCall(obj->asE_funCall());         break;
  case Expression::E_CONSTRUCTOR: subVisitE_constructor(obj->asE_constructor()); break;
  case Expression::E_FIELDACC:    subVisitE_fieldAcc(obj->asE_fieldAcc());       break;
  case Expression::E_GROUPING:    subVisitE_grouping(obj->asE_grouping());       break;
  case Expression::E_ARROW:       subVisitE_arrow(obj->asE_arrow());             break;
  case Expression::E_UNARY:       subVisitE_unary(obj->asE_unary());             break;
  case Expression::E_EFFECT:      subVisitE_effect(obj->asE_effect());           break;
  case Expression::E_BINARY:      subVisitE_binary(obj->asE_binary());           break;
  case Expression::E_ADDROF:      subVisitE_addrOf(obj->asE_addrOf());           break;
  case Expression::E_DEREF:       subVisitE_deref(obj->asE_deref());             break;
  case Expression::E_CAST:        subVisitE_cast(obj->asE_cast());               break;
  case Expression::E_COND:        subVisitE_cond(obj->asE_cond());               break;
  case Expression::E_GNUCOND:     subVisitE_gnuCond(obj->asE_gnuCond());         break;
  case Expression::E_ASSIGN:      subVisitE_assign(obj->asE_assign());           break;
  case Expression::E_NEW:         subVisitE_new(obj->asE_new());                 break;
  case Expression::E_THROW:       subVisitE_throw(obj->asE_throw());             break;
  case Expression::E_KEYWORDCAST: subVisitE_keywordCast(obj->asE_keywordCast()); break;
  case Expression::E_STATEMENT:   subVisitE_statement(obj->asE_statement());     break;
  case Expression::E___BUILTIN_VA_ARG:
                                  subVisitE___builtin_va_arg(obj->asE___builtin_va_arg()); break;
  }
}

bool DataFlowVisitor::subVisitE_compoundLit(E_compoundLit *obj) {
  Value *rval = obj->abstrValue->asRval();
  if (rval->isCVAtomicValue()) {
    CVAtomicValue *value0 = rval->asCVAtomicValue();
    // special case transparent unions; FIX: this should not be
    // necessary when we are computing dataflow for compound literals
    // correctly for abstract values; I think perhaps also Unions are
    // not implemented correctly in the compound literals code
    CompoundType *ct = value0->t()->ifCompoundType();
    if (ct && ct->isTransparentUnion) {
      // Scott implementes a transparent union by elaborating a
      // E_compoundLit for the union using an IN_compound to specify
      // the actual value
      ASTList<Initializer> &inits = obj->init->asIN_compound()->inits;
      xassert(inits.count() == 1);
      IN_designated *ides = inits.first()->asIN_designated();
      xassert(ides->designator_list->count() == 1);
      FieldDesignator *fd = ides->designator_list->first()->asFieldDesignator();
      StringRef name0 = fd->id;
      Variable *field = ct->rawLookupVariable(name0);
      Value *instSpecField = asVariable_O(field)->abstrValue();
      if (oinkCmd->instance_sensitive) {
        instSpecField = value0->getInstanceSpecificValue(name0, instSpecField);
      }
      dfe.eDataFlow_normal(ides->init->asIN_expr()->e->abstrValue, instSpecField, getLoc());
    }
  }

  try {
    compoundInit(&dfe, getLoc(), obj->getVarForRval(), obj->init,
                 oneAssignmentDataFlow, reportUserErrorDataFlow);
  } catch (UserError &err) {
    // dsw: there are some compound initializers situations
    // where gcc just gives a warning instead of an error, such
    // as if you put too many elements into an array:
    // oink/Test/too_many_initializers1.c; here I turn the error
    // into a warning so we can at least process such files
    userReportWarning
      (getLoc(), "Unsoundness warning: above error turned into a warning");
  }
  return true;
}

bool DataFlowVisitor::subVisitE_this(E_this *obj) {
  xassert(obj->receiver->type->isReferenceType());
  dfe.eDataFlow(asVariable_O(obj->receiver)->abstrValue(), obj->abstrValue, getLoc(),
                EQ_DataFlowKind,
                false,          // don't unref src
                false);
  return true;
}

bool DataFlowVisitor::subVisitE_variable(E_variable *obj) {
  // should not get "this" here anymore, at least not in C++
  if (globalLang.isCplusplus) {
    xassert(!streq(obj->name->getName(), "this"));
  }
  // OK, this is the name of the parameter variable that gets added.
  dfe.eDataFlow_refUnify(asVariable_O(obj->var)->abstrValue(),
                         obj->abstrValue,
                         getLoc());
  return true;
}

bool DataFlowVisitor::subVisitE_funCall(E_funCall *obj) {
  FunctionValue *fun = NULL;    // function being called
  Value *thisValue = NULL;      // 'this' argument, if there is one
  Expression *funcExpr = obj->func->skipGroups();

  // generally, you can 'call' any expression the type of which is a
  // Function, Reference to Function, or Pointer to Function, however
  // there are syntatic special cases for method and pointer to
  // function member calls.
  if (funcExpr->isE_fieldAcc()) {
    E_fieldAcc *e_field = funcExpr->asE_fieldAcc();
    Value *fun0 = asVariable_O(e_field->field)->abstrValue()->asRval();
    if (fun0->isFunctionValue() && fun0->asFunctionValue()->type->isMethod()) {
      // method call
      fun = fun0->asFunctionValue();
      thisValue = e_field->obj->abstrValue->asLval();
      // NOTE: if we are calling a function that is virtual, then the
      // virtualness will be handled elsewhere: when the overriding
      // function notices that it is overriding something.
      goto have_function_value;
    }
  } else if (E_binary *e_bin = funcExpr->ifE_binary()) {
    Value *e1_unref = e_bin->e1->abstrValue->asRval();
    if (e_bin->op == BIN_ARROW_STAR) {
      // pointer to function member call
      fun = e_bin->e2->abstrValue->asRval()
        ->asPointerToMemberValue()->atValue->asFunctionValue();
      thisValue = e1_unref->asPointerValue()->atValue->asLval();
      goto have_function_value;
    } else if (e_bin->op == BIN_DOT_STAR) {
      // pointer to function member call
      fun = e_bin->e2->abstrValue->asRval()
        ->asPointerToMemberValue()->atValue->asFunctionValue();
      thisValue = e1_unref->asLval();
      goto have_function_value;
    }
  }

  // no special case, just some expression that evaluates to a
  // Function Value or Pointer to Function Value
  {
    Value *fun0 = funcExpr->abstrValue->asRval();
    if (fun0->isFunctionValue()) {
      // direct call to a function
      if (fun0->t()->asFunctionType()->isMethod()) {
        xfailure("method calls have explicit 'this->' elaborated");
      }
      fun = fun0->asFunctionValue();
      thisValue = NULL;

      // Special case for varargs builtins __builtin_va_start,
      // __builtin_va_copy, and __builtin_va_end.  Note that
      // __builtin_va_arg is handled elsewhere as it is a real AST
      // node.
      E_variable *fvar = funcExpr->skipGroups()->ifE_variable();
      if (fvar) {
        // find out if it is in fact a __builtin_va_* function
        PQ_name *name0 = fvar->name->ifPQ_name();
        if (name0) {
          StringRef name1 = name0->name;
          if (name1 == strRef__builtin_va_start) {
            xassert(obj->args->count() == 2);
            Expression *arg1 = obj->args->first()->expr;
            Expression *arg2 = obj->args->butFirst()->first()->expr;
            return subVisit__builtin_va_start(arg1, arg2);
          } else if (name1 == strRef__builtin_va_copy) {
            xassert(obj->args->count() == 2);
            Expression *arg1 = obj->args->first()->expr;
            Expression *arg2 = obj->args->butFirst()->first()->expr;
            return subVisit__builtin_va_copy(arg1, arg2);
          } else if (name1 == strRef__builtin_va_arg) {
            xfailure("E_funCall should not see __builtin_va_arg");
          } else if (name1 == strRef__builtin_va_end) {
            return subVisit__builtin_va_end();
          }
        }
      }
    } else if (fun0->isPointerValue()) {
      // quarl 2006-07-26
      //    Elsa will handle this in the future.
      // xfailure("tried to call a function pointer");
      // call through a function pointer
      xassert(fun0->isPointerValue());
      fun = fun0->asPointerValue()->atValue->asFunctionValue();
      thisValue = NULL;
    }
  }

have_function_value:
  dfe.eDataFlow_call0(getLoc(), obj->retObj, obj->abstrValue,
                      thisValue, obj->args, fun);

  return true;
}

bool DataFlowVisitor::subVisitE_constructor(E_constructor *obj) {
  if (obj->ctorVar) {
    FunctionValue *funcValue0 = asVariable_O(obj->ctorVar)->abstrValue()->asFunctionValue();
    dfe.eDataFlow_call0
      (getLoc(), obj->retObj,
       NULL /* callExprValue: there is no ret for a ctor; this may change */,
       NULL /* thisArgValue: there is no "this" for a ctor */,
       obj->args,
       funcValue0);
  } else {
    // a null ctorVar means the "trivial" copy ctor; in C elsa does
    // not elaborate ctors for structs, yet when they are passed or
    // returned by value, their contents must be copied
    xassert(obj->args->count() == 1 && "e53e5e9b-b647-4ff2-a5ee-6be1a9ba5f11");
    dfe.eDataFlow_normal(obj->args->first()->expr->abstrValue, obj->retObj->abstrValue, getLoc());
  }

  if (!obj->artificial) {
    // FIX: I don't know how obj->retObj can ever be NULL, but it is
    // in /home/dsw/ball/mozilla-0.9.9-7/STDIN-10000.g.ii; I'm pretty
    // sure that's a bug; Note that the ctor is a parameter default
    // argument.
    if (obj->retObj) {
      // FIX: I change this to eDataFlow_refUnify(), but I'm not sure
      // about it.
      dfe.eDataFlow_refUnify(obj->retObj->abstrValue, obj->abstrValue, getLoc());
    }
  }
  return true;
}

bool DataFlowVisitor::subVisitE_fieldAcc(E_fieldAcc *acc) {
  if (acc->field) {
//  Note: cc_tcheck.cc: Type *makeLvalType(Env &env, Type *underlying)
//  doesn't make references for functions, but this can also be some
//  type called <dependent>!  Must be a template thing.
//  xassert(type->isReferenceType() || type->isFunctionType());
//  // field expression should be a reference or a function
    CVAtomicValue *cvat = acc->obj->abstrValue->asRval()->asCVAtomicValue();
    string name0 = acc->field->name;

    // FIX: Can I avoid creating this in the instance-sensitive case?
    Value *instSpecField = asVariable_O(acc->field)->abstrValue();
    if (oinkCmd->instance_sensitive) {
      instSpecField = cvat->getInstanceSpecificValue(name0, instSpecField);
    }
    dfe.eDataFlow_refUnify(instSpecField, acc->abstrValue, getLoc());

  } else {
    // the 'field' remains NULL if we're in a template function and
    // the 'obj' is dependent on the template arguments.. there are
    // probably a few other places lurking that will need similar
    // treatment, because typechecking of templates is very incomplete
    // and in any event when checking the template *itself* (as
    // opposed to an instantiation) we never have enough information
    // to fill in all the variable references..
  }
  return true;
}

bool DataFlowVisitor::subVisitE_grouping(E_grouping *obj) {
  dfe.eDataFlow_normal(obj->expr->abstrValue, obj->abstrValue, getLoc());
  return true;
}

bool DataFlowVisitor::subVisitE_arrow(E_arrow *obj) {
  xfailure("the typechecker should have removed E_arrow");
  return true;
}

bool DataFlowVisitor::subVisitE_unary(E_unary *obj) {
  // FIX: Like BIN_AND and BIN_OR, should probably have edges inserted
  // as the rest of the unary ops do, but an implicit cast would
  // usually break it.  Haven't done implicit casts yet.
  if (obj->op != UNY_NOT) {
    xassert(!obj->abstrValue->isReferenceValue());
    dfe.eDataFlow_normal(obj->expr->abstrValue, obj->abstrValue, getLoc());
  }
  return true;
}

bool DataFlowVisitor::subVisitE_effect(E_effect *obj) {
  xassert(obj->expr->abstrValue->isReferenceValue());
  dfe.eDataFlow_normal(obj->expr->abstrValue, obj->abstrValue, getLoc());
  return true;
}

// I can't stand the ugliness of how this wouldn't fit in with the
// other methods if I made it one
static void subVisitE_binary_ptrToMem
  (DataFlowEx &dfe, SourceLoc loc, Value *value, Value *lhsValue, Value *rhsValue)
{
  xassert(value);
  xassert(lhsValue);
  xassert(rhsValue);
  xassert(!lhsValue->isReferenceValue()); // can't have a pointer to a ref
  xassert(value->isReferenceValue() || value->isFunctionValue());
  xassert(rhsValue->isPointerToMemberValue());
  dfe.eDataFlow_deref(rhsValue->asPointerToMemberValue()->getAtValue(), value, loc);
}

bool DataFlowVisitor::subVisitE_binary(E_binary *obj) {
  xassert(obj->abstrValue != obj->e1->abstrValue);
  xassert(obj->abstrValue != obj->e2->abstrValue);

  Value *lhsValue = obj->e1->abstrValue->asRval();
  Value *rhsValue = obj->e2->abstrValue->asRval();
  Value *value = obj->abstrValue;
  BinaryOp op = obj->op;

  switch(op) {
  default: xfailure("illegal op code 8c6519af-0c92-4890-bb6c-5837bc60974e"); break;

    // the relationals come first, and in this order, to correspond
    // to RelationOp in predicate.ast
  case BIN_EQUAL:               // ==
  case BIN_NOTEQUAL:            // !=
  case BIN_LESS:                // <
  case BIN_GREATER:             // >
  case BIN_LESSEQ:              // <=
  case BIN_GREATEREQ:           // >=
    // this is a new boolean, so don't add any edges
    break;

  case BIN_PLUS:                // +
    // dsw: deal with pointer arithmetic correctly;
    // this is the case p + 1
    if (lhsValue->t()->isPointerOrArrayRValueType()
        && isIntegerType_or_EnumType(rhsValue->t())) {
      // only add an edge to the left
      dfe.eDataFlow_normal(lhsValue, value, getLoc());
      if (oinkCmd->array_index_flows) {
        dfe.eDataFlow_normal(rhsValue, value->getAtValue(), getLoc());
      }
    }
    // this is the case 1 + p
    else if (isIntegerType_or_EnumType(lhsValue->t())
             && rhsValue->t()->isPointerOrArrayRValueType()) {
      // only add an edge to the right
      dfe.eDataFlow_normal(rhsValue, value, getLoc());
      if (oinkCmd->array_index_flows) {
        dfe.eDataFlow_normal(lhsValue, value->getAtValue(), getLoc());
      }
    }
    else {                      // add both edges
      xassert(!lhsValue->t()->isPointerOrArrayRValueType());
      xassert(!rhsValue->t()->isPointerOrArrayRValueType());
      xassert(!lhsValue->isArrayValue()); // just being really sure; used to condition on this
      dfe.eDataFlow_normal(lhsValue, value, getLoc());
      dfe.eDataFlow_normal(rhsValue, value, getLoc());
    }
    // default behavior of returning the left side is close enough for now.
    break;

  case BIN_MINUS:               // -
    // NOTE: these two cases are now merged, so no need for the test
    // for pointer vs. int type.  I leave it here anyway commented out
    // in case we want to distinguish between these two situations
    // again in the future.
//      if (lhsValue->isPointerOrArrayRValueValue()
//          && rhsValue->isPointerOrArrayRValueValue() )
    dfe.eDataFlow_normal(lhsValue, value, getLoc());
    dfe.eDataFlow_normal(rhsValue, value, getLoc());
    // default behavior of returning the left side is close enough for now.
    break;

  case BIN_MULT:                // *
  case BIN_DIV:                 // /
  case BIN_MOD:                 // %
  case BIN_LSHIFT:              // <<
  case BIN_RSHIFT:              // >>
  case BIN_BITAND:              // &
  case BIN_BITXOR:              // ^
  case BIN_BITOR:               // |
    dfe.eDataFlow_normal(lhsValue, value, getLoc());
    dfe.eDataFlow_normal(rhsValue, value, getLoc());

    // FIX: These two should probably have edges inserted as above,
    // but an implicit cast would usually break it.  Haven't done
    // implicit casts yet.
  case BIN_AND:                 // &&
  case BIN_OR:                  // ||
    break;

  case BIN_COMMA:
    // link to the last one; this chains correctly
    dfe.eDataFlow_normal(rhsValue, value, getLoc());
    break;

  case BIN_BRACKETS:
    xfailure("the typechecker should have removed BIN_BRACKETS");
    break;

  case BIN_ASSIGN:              // = (used to denote simple assignments in AST, as opposed to (say) "+=")
    xfailure("why isn't this in E_assign?");
    break;

    // C++ operators
  case BIN_ARROW_STAR: {        // ->*
    Value *unrefed_ptr = lhsValue->asRval();
    xassert(unrefed_ptr->isPointerValue() && "c312d480-2c8d-43ee-9d9d-13d900e51aa3");
    // NOTE: This stripping off down to getAtValue() is NOT optional, since
    // this is a real pointer.
    subVisitE_binary_ptrToMem(dfe, getLoc(), value, unrefed_ptr->getAtValue(), rhsValue);
    break;
  }

  case BIN_DOT_STAR: {          // .*
    subVisitE_binary_ptrToMem(dfe, getLoc(), value, lhsValue, rhsValue);
    break;
  }

    // theorem prover extension
  case BIN_IMPLIES:             // ==>
    // Scott knows what to do with this
    xfailure("should not get here");
    break;

  // g++ extensions (works in C++ but not in C ?!)
  // http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Min-and-Max.html
  case BIN_MINIMUM:             // <?
  case BIN_MAXIMUM:             // >?
    dfe.eDataFlow_normal(lhsValue, value, getLoc());
    dfe.eDataFlow_normal(rhsValue, value, getLoc());
    break;
  }

  return true;
}

bool DataFlowVisitor::subVisitE_addrOf(E_addrOf *obj) {
  // NOTE: cc_tcheck.cc: Type *makeLvalType(Env &env, Type
  // *underlying) doesn't make references for functions.
  xassert(obj->expr->abstrValue->isReferenceValue() || obj->expr->abstrValue->isFunctionValue());

  xassert(!obj->abstrValue->isReferenceValue());
  if (obj->abstrValue->isPointerValue()) {
    xassert(obj->abstrValue->isPointerValue());
    dfe.eDataFlow_addrof(obj->expr->abstrValue, obj->abstrValue, getLoc());
  } else {
    xassert(obj->abstrValue->isPointerToMemberValue());
    xassert(obj->expr->isE_variable());
    Variable *var0 = obj->expr->asE_variable()->var;
    xassert(var0->scope);
    CompoundType *inClass0 = dynamic_cast<CompoundType*>(var0->scope);
    // NOTE: if we are taking the address of something that is
    // virtual, then the virtualness will be handled elsewhere: when
    // the overriding function notices that it is overriding
    // something.
    dfe.eDataFlow_addrof(obj->expr->abstrValue, obj->abstrValue, getLoc());
    xassert(inClass0);
  }
  return true;
}

bool DataFlowVisitor::subVisitE_deref(E_deref *obj) {
  Value *unrefed_ptr = obj->ptr->abstrValue->asRval();
  // FIX: this fails in
  // /home/dsw/ball/./mozilla-0.9.9-7/STDIN-11119.g.ii ; A class is
  // derefed; we must be missing an operator deref or an conversion
  // operator to a de-refable thing; I'll skip it for now
//    xassert(unrefed_ptr->isPointerValue() ||
//            unrefed_ptr->isArrayValue() ||
//            unrefed_ptr->isFunctionValue() );
  // Punt on anything that is not one of these.
  if ( !(unrefed_ptr->isPointerValue() ||
         unrefed_ptr->isArrayValue()   ||
         unrefed_ptr->isFunctionValue()) ) {
    return true;
  }
  // NOTE: This stripping off down to getValue() is NOT optional, since this
  // is a real pointer.
  Value *derefed_unrefed_ptr = NULL;
  if (unrefed_ptr->isFunctionValue()) {
    // NOTE: idempotent for functions
    derefed_unrefed_ptr = unrefed_ptr;
  } else {
    derefed_unrefed_ptr = unrefed_ptr->getAtValue();
  }
  xassert(derefed_unrefed_ptr);
  xassert(!derefed_unrefed_ptr->isReferenceValue()); // can't have a pointer to a ref
  xassert(obj->abstrValue->isReferenceValue() || obj->abstrValue->isFunctionValue());

  dfe.eDataFlow_deref(derefed_unrefed_ptr, obj->abstrValue, getLoc());
  // moved to QualVisitor2
//    applyOpDeref(obj->abstrValue, unrefed_ptr);
  return true;
}

// C-style cast
bool DataFlowVisitor::subVisitE_cast(E_cast *obj) {
  dfe.eDataFlow_cast(obj->expr->abstrValue, obj->abstrValue, getLoc());
  return true;
}

// ? : syntax
bool DataFlowVisitor::subVisitE_cond(E_cond *obj) {
  // FIX: should rename the type of the expression to be "lub", per
  // cqual.  NOTE: If the qvar has already been created, this is a
  // problem.  Probably need a seperate renaming pass before the qvar
  // pass.

  // link to both

  dfe.eDataFlow_normal(obj->th->abstrValue, obj->abstrValue, getLoc());
  dfe.eDataFlow_normal(obj->el->abstrValue, obj->abstrValue, getLoc());
  return true;
}

bool DataFlowVisitor::subVisitE_gnuCond(E_gnuCond *obj) {
  // In gcc it is legal to omit the 'then' part;
  // http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Conditionals.html#Conditionals
  dfe.eDataFlow_normal(obj->cond->abstrValue, obj->abstrValue, getLoc());
  dfe.eDataFlow_normal(obj->el->abstrValue, obj->abstrValue, getLoc());
  return true;
}


bool DataFlowVisitor::subVisitE_assign(E_assign *obj) {
  // should not get targets here which are compound types or
  // references to such, since it should have been resolved into a
  // call to an operator assign, which always exists implicitly.
  if (lang.isCplusplus) {
    // t0148.cc t0149.cc
    // 53f8abcb-a6e1-40a9-9b95-53adb8aa3b86
    xassert(!obj->target->type->asRval()->isCompoundType()
            && "53f8abcb-a6e1-40a9-9b95-53adb8aa3b86");
  }

  Value *tgt = obj->target->abstrValue;
  // remove l-value layer of indirection for ASSIGNMENT (not for
  // INITIALIZATION).
  USER_ASSERT(tgt->isReferenceValue(), getLoc(), "Left side of assignment must be an lvalue");
  Value *at = tgt->asRval();
  // FIX: There may be an implied cast here; see Test.incl.mk for
  // Scott's comments on implementing that.

  dfe.eDataFlow_normal(obj->src->abstrValue, at, getLoc());

  // note that E_assign is also an expression the value of which can
  // be used; it is even an L-value according to gcc
  dfe.eDataFlow_normal(tgt, obj->abstrValue, getLoc());

  return true;
}

bool DataFlowVisitor::subVisitE_new(E_new *obj) {
  // FIX: Maybe link to the return value of the constructor?
  // UPDATE: I think the above comment is obsolete.

//    if (obj->ctorArgs) {
//      xassert(obj->ctorVar);
  // this could probably be omitted: the ast type id of the E_new
  // flows to the type of the E_new
  Variable_O *astTypeIDDeclVar = asVariable_O(obj->atype->decl->var);
  xassert(astTypeIDDeclVar);
  dfe.eDataFlow_addrof(astTypeIDDeclVar->abstrValue()->asLval(), obj->abstrValue, getLoc());
  // the heap-allocated var flows to the type of the E_new; there is
  // no ctor call for non-CompoundType-s so there is no heapVar
  if (obj->heapVar) {
    dfe.eDataFlow_addrof
      (asVariable_O(obj->heapVar)->abstrValue()->asLval(), obj->abstrValue, getLoc());
  }
  return true;
}

bool DataFlowVisitor::subVisitE_throw(E_throw *obj) {
  // FIX: There are implicit copy ctors going on here
  //
  // NOTE: yes, we do not insert the edge here if there is both a
  // globalCtorStatement and an expr; the globalCtorStatement takes
  // priority
  if (!obj->globalCtorStatement) {
    if (obj->expr) {
//    } else {                      // re-throw
//      // For now there is nothing to do here since all throws go to all
//      // catches, this adds nothing to the analysis.
    }
  }
  return true;
}

// C++-style cast
bool DataFlowVisitor::subVisitE_keywordCast(E_keywordCast *obj) {
  dfe.eDataFlow_cast(obj->expr->abstrValue, obj->abstrValue, getLoc());
  return true;
}

bool DataFlowVisitor::subVisitE_statement(E_statement *obj) {
  // if this fails, it is a user error, but the typechecking pass is
  // supposed to catch it; UPDATE: FIX: elsa should be catching this I
  // suppose, but for now I will just skip it.
//   xassert(obj->s->stmts.count() >= 1);
  if (obj->s->stmts.count() < 1) {
    // userReportWarning(loc, "Empty statement expression so omitting any dataflow edge");
  } else {
    Statement *last = obj->s->stmts.last();
    if (!last->isS_expr()) {
      // quarl 2006-07-12
      //    For now I'm commenting out this warning and the "empty statement
      //    expr" one above because:
      //      1) this wouldn't compile with gcc so such code wouldn't pass the
      //         gcc_1 stage: "int x = ({ });" or "int x = ({if (1) 1;});"
      //      2) we don't yet catch this, which is similar but worse: "int x =
      //         (void) 0;" or "void foo(); int x = foo();"
      //      3) with valid code (that compiles with gcc) there shouldn't be
      //         any "void value not ignored as it ought to be"
      //      4) it is an annoying and common false positive with "({ });"
      //    Ideally, this should be an error if the result is assigned to
      //    something; else no error or warning.

      // static WarningManager wm("Last statement of a statement expression is not an S_expr, "
      //                          "so omitting any dataflow edge");
      // wm.emit(loc);
    } else {
      FullExpression *fexpr = last->asS_expr()->expr;
      dfe.eDataFlow_normal(fexpr->expr->abstrValue, obj->abstrValue, getLoc());
    }
  }
  return true;
}

// bool DataFlowVisitor::subVisitE___builtin_va_start(E___builtin_va_start *bvs) {
bool DataFlowVisitor::subVisit__builtin_va_start(Expression *vaIterExpr, Expression */*preEllipsisParam*/) {
  // Note that we don't need the second parameter for dataflow, we
  // just get the ellipsis parameter out of the FunctionValue; we
  // could use it as an assertion I suppose, since it should be an
  // expression of the second parameter.
  FunctionValue *fv = functionStack.top()->abstrValue->asFunctionValue();
  // REMOVE_FUNC_DECL_VALUE: replace with this
//   FunctionValue *fv = asVariable_O(functionStack.top()->nameAndParams->var)->abstrValue()
//     ->asFunctionValue();

  if (!fv->t()->asFunctionType()->hasFlag(FF_VARARGS)) {
    userFatalError(getLoc(), "__builtin_va_start in function that does not have an ellipsis");
  }
  Value *ellipsisValue0 = fv->getEllipsis()->abstrValue()->asRval();
  xassert(ellipsisValue0->t()->isEllipsis());
  Value *vaIterValue = vaIterExpr->abstrValue->asRval();
  PointerValue *vaIterValuePtr = vaIterValue->asPointerValue();
  xassert(vaIterValuePtr->atValue->t()->isEllipsis());
  dfe.eDataFlow_normal(ellipsisValue0, vaIterValuePtr->atValue, getLoc());
  return true; // perhaps this should be false as we don't need the sub-expression
}

// bool DataFlowVisitor::subVisit__builtin_va_copy(E___builtin_va_copy *bvc) {
bool DataFlowVisitor::subVisit__builtin_va_copy(Expression *tgtVaIterExpr, Expression *srcVaIterExpr) {
  dfe.eDataFlow_normal(srcVaIterExpr->abstrValue->asRval(),
                       tgtVaIterExpr->abstrValue->asRval(), getLoc());
  return true; // perhaps this should be false as we don't need the sub-expression
}

bool DataFlowVisitor::subVisitE___builtin_va_arg(E___builtin_va_arg *bva) {
  Value *vaIterValue = bva->expr->abstrValue->asRval();
  PointerValue *vaIterValuePtr = vaIterValue->asPointerValue();
  xassert(vaIterValuePtr->atValue->t()->isEllipsis());
  // there is an implicit cast here so tell the dataflow backend that
  dfe.eDataFlow_cast(vaIterValuePtr->atValue, bva->abstrValue->asRval(), getLoc());
  return true; // perhaps this should be false as we don't need the sub-expression
}

// bool DataFlowVisitor::subVisitE___builtin_va_end(E___builtin_va_end *) {
bool DataFlowVisitor::subVisit__builtin_va_end() {
  return true; // perhaps this should be false as we don't need the sub-expression
}

bool DataFlowVisitor::visitTemplateArgument(TemplateArgument *obj) {
  // the template arguments for partial specializations contain type
  // variables which have not really had their types set correctly,
  // but more importantly, I can't imagine how a template argument can
  // really participate in qualifier inference
  return false;                 // prune
}

#ifdef GNU_EXTENSION
bool DataFlowVisitor::visitASTTypeof(ASTTypeof *obj) {
  // we do not want to do dataflow down inside a typeof expression
  // because that code isn't actually run and so it makes it look like
  // those variables were used when in fact they were not:
  // Test/typeof_undefined1.c; therefore prune the tree.
  if (obj->isTS_typeof_expr()) return false;
  return true;        // otherwise, just keep going with the traversal
}
#endif // GNU_EXTENSION


// FindValueASTVisitor ****************

bool FindValueASTVisitor::visitExpression(Expression *expr) {
  if (expr->hasAbstrValue()) {
    expr->getAbstrValue()->traverse(valueVisitor);
  }
  return true;
}

bool FindValueASTVisitor::visitFunction(Function *func) {
  xassert(func->abstrValue);
  func->abstrValue->traverse(valueVisitor);
  return true;
}

bool FindValueASTVisitor::visitDeclarator(Declarator *decltor) {
  xassert(decltor->abstrValue);
  decltor->abstrValue->traverse(valueVisitor);
  return true;
}

// StructuralFlow_ValueVisitor ****************

bool StructuralFlow_ValueVisitor::preVisitValue(Value *obj) {
  // idempotency
  if (visitedValue.contains(obj)) return true;
  visitedValue.add(obj);
  // put in the edges to/from the struct/class/union container
  if (obj->t()->isCompoundType()) {
    SObjList<Variable> &dataVars = obj->t()->asCVAtomicType()->atomic->asCompoundType()->
      getDataVariablesInOrder();
    SFOREACH_OBJLIST_NC(Variable, dataVars, iter) {
      Variable_O *var = asVariable_O(iter.data());
      if (compoundUp) {
        dfe.eDataFlow_normal(var->abstrValue(), obj, var->loc);
      }
      if (compoundDown) {
        dfe.eDataFlow_normal(obj, var->abstrValue(), var->loc);
      }
    }
  }
  // put in the edges to/from the pointer
  if (obj->ptrToMe) {
    if (pointUp) {
      dfe.eDataFlow_normal(obj, obj->ptrToMe, obj->loc);
    }
    if (pointDown) {
      dfe.eDataFlow_normal(obj->ptrToMe, obj, obj->loc);
    }
  }
  // put in the edges to/from the reference
  if (obj->refToMe) {
    if (refUp) {
      dfe.eDataFlow_normal(obj, obj->refToMe, obj->loc);
    }
    if (refDown) {
      dfe.eDataFlow_normal(obj->refToMe, obj, obj->loc);
    }
  }

  return true;
}

// FuncDeclVar_ASTVisitor ****************

bool FuncDeclVar_ASTVisitor::visitDeclarator(Declarator *decltor) {
  // optimization: avoid calling var->abstrValue() unless this is a
  // function declarator
  if (!decltor->type->isFunctionType()) return true;
  // NOTE: do NOT use decltor->abstrValue
  Value *v0 = asVariable_O(decltor->var)->abstrValue();
  xassert(v0);
  xassert(v0->isFunctionValue());
  v0->traverse(valueVisitor);
  return true;
}
