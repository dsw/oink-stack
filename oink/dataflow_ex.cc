// see License.txt for copyright and terms of use

#include "dataflow_ex.h"        // this module
#include "oink_global.h"        // oinkCmd
#include "value.h"              // makeEllipsisMirrorParam
#include "oink_util.h"

// all insertions end up here
void DataFlowEx::eDataFlow
  (Value *src, Value *tgt, SourceLoc loc,
   DataFlowKind dfk, bool mayUnrefSrc, bool cast,
   bool useMI, LibQual::polarity miPol, int fcId)
{
  // fprintf(stderr, "##          eDataFlow src=%p, tgt=%p\n", src, tgt);
  // just pass the insertion call through unchanged
  if (dft) {
    dft->tDataFlow
      (src, tgt, loc,
       dfk, mayUnrefSrc, cast,
       useMI, miPol, fcId);
  }
}

// normal ****

void DataFlowEx::eDataFlow_normal(Value *src, Value *tgt, SourceLoc loc) {
  eDataFlow(src, tgt, loc,
           // most insertions use these three defaults
           LE_DataFlowKind, true, false);
}

void DataFlowEx::eDataFlow_cast(Value *src, Value *tgt, SourceLoc loc) {
  eDataFlow(src, tgt, loc,
            LE_DataFlowKind, true, true /*a cast insertion*/);
}

// unification ****

void DataFlowEx::eDataFlow_unify(Value *src, Value *tgt, SourceLoc loc) {
  // this is off when merge_E_variable_and_var_values is on because we
  // now sometimes use the same Value annotation for multiple object
  // (Expressions, Varibles) and we also use the existance of
  // annotating qvars as evidence of dataflow; therefore there must
  // sometimes be dataflow even between something and itself just to
  // get it annotated with qvars.  Don't assign self-edges for
  // non-useMI edges.

  // optimization
  if (!oinkCmd->merge_E_variable_and_var_values && src == tgt) return;

  eDataFlow(src, tgt, loc,
            UN_DataFlowKind /*unify*/, true /*even the refs*/,
            false /*not as cast insertion*/);
}

void DataFlowEx::eDataFlow_refUnify(Value *src, Value *tgt, SourceLoc loc) {
  eDataFlow_unify(src->asLval(), tgt->asLval(), loc);
}

// crossing the lvalue/rvalue boundary ****

void DataFlowEx::eDataFlow_deref(Value *src, Value *tgt, SourceLoc loc) {
  eDataFlow(src, tgt, loc,
            EQ_DataFlowKind /*FIX:is this correct?*/, true, false);
}

void DataFlowEx::eDataFlow_addrof(Value *src, Value *tgt, SourceLoc loc) {
  // quarl 2006-07-26
  //    Normally, src is already an L-value, but you may not make references
  //    to functions in C++ so we do it here because we want the symmetry
  //    back.
  eDataFlow(src->asLval(), tgt, loc,
            LE_DataFlowKind, false /*don't unref src*/, false);
}

// function call; protected ****

// manufacture a fake parameter to assign to
Value *DataFlowEx::makeEllipsisMirrorParam
  (SourceLoc loc, Value *argValue, Value */*ellipsisParamValue*/, FunctionValue */*ft*/)
{
  // this is not used anymore?
  return vFac->buildValue(argValue->t(), loc);
}

void DataFlowEx::eDataFlow_argToParam
  (Value *src, Value *tgt, SourceLoc loc,
   DataFlowKind dfk, bool mayUnrefSrc, int fcId)
{
  eDataFlow(src, tgt, loc,
            dfk, mayUnrefSrc, false /*not a cast*/,
            true /*useMI*/, LibQual::p_neg /*fun call goes DOWN the stack*/, fcId);
}

void DataFlowEx::eDataFlow_call0
  (SourceLoc loc, Expression *retObj, Value *callExprValue,
   Value *thisArgValue, FakeList<ArgExpression> *args, FunctionValue *fv)
{
  if (thisArgValue) xassert(fv->type->isMethod());
  if (fv->type->isMethod()) xassert(thisArgValue);

  int fcId = globalNextFunCallId++;

  // if it is return by value of a CompoundType, we need to put in the
  // edge from the call site retObj to the function retVar
  if (retObj) {
    Value *retObjValue = retObj->abstrValue;
    xassert(retObjValue);
    // NOTE: unlike for function return values, this is an edge that
    // is FROM and argument (retObj) DOWN TO a REFERENCE TO a
    // CompoundType parameter (retVar)
    xassert(retObjValue->isReferenceValue());
    xassert(fv->getRetVar()->getType()->asRval()->isCompoundType());
    eDataFlow_argToParam
      (retObjValue,
       // NOTE: the idempotency of asLval() is important!
       fv->getRetVar()->abstrValue()->asLval(),
       loc,
       LE_DataFlowKind,
       // NOTE: this shouldn't matter, since tgt is a ref
       false, // mayUnrefSrc
       fcId);

    // hook it up to the return value of the function call
    if (callExprValue) {         // a ctor has no callExprValue
      // FIX: should assert that callExprValue equals retObjValue
      // unify them
      eDataFlow_refUnify(callExprValue, retObjValue, loc);
    }
  }
  // otherwise, add edge for function return; for ctors I allow a NULL
  // callExprValue since there is no first class return value
  else if (callExprValue) {
    eDataFlow
      (fv->retValue, callExprValue, loc,
       LE_DataFlowKind, true, false, // cast
       true,                    // useMI
       LibQual::p_pos,          // we are coming UP the stack when returning
       fcId);
  }


//   // Careful: gcc and cqual both do not do type checking at a function
//   // call site if the declaration that was seen was K&R:
//   //
//   //    If you declare a function \texttt{f} using the K\&R style, then no
//   //    type checking is done to arguments at a call to \texttt{f}.  This
//   //    matches the behavior of C, but it can lead to unexpected results.  If
//   //    wish to run \cqual{} on a program written in the K\&R style, you can
//   //    use the GNU package \texttt{protoize} to ANSIfy the function
//   //    definitions and declarations.  \cqual{} will warn about some, but not
//   //    all, uses of old-style functions.
//   //
//   //    Basically, when you use old-style function in C, gcc does no type
//   //    checking on them whatsoever, so I decided to do the same with cqual and
//   //    add some warnings.
//   //
//   //    Jeff
//   if (fv->isKandR()) return;

  // add edge for function arguments
  ArgExpression *arg0 = args->first();
  ParamIter paramIter(fv);

  // sm: now that member functions have their 'this' parameter, we
  // need to skip over it to retain the previous behavior of this
  // code.  obviously that raises the question of when exactly does
  // the 'this' parameter get connected to the receiver object at the
  // call site, but I'll leave that question unanswered.
  if (fv->type->isMethod()) {
    xassert(thisArgValue);
    // they should be pointers or references; FIX: why can it be
    // either?
    xassert(thisArgValue->isPointerValue()
            || thisArgValue->isReferenceValue());
    xassert(paramIter.data()->type->isPointerType()
            || paramIter.data()->type->isReferenceType());

    eDataFlow_argToParam(thisArgValue, paramIter.data()->abstrValue(), loc,
                         LE_DataFlowKind, false /*mayUnrefSrc*/, fcId);
    paramIter.advanceButNotPastEllipsis();
    USER_ASSERT(paramIter.isDone() || !paramIter.data()->type->isEllipsis(),
                loc, "First argument after 'this' can't be ellipsis.");
  }

  // NOTE: there is no need to special case a function that has
  // exactly one void parameter, as Scott handles that in the type
  // checker.
  for (;
       arg0 != NULL;
       arg0 = arg0->next, paramIter.advanceButNotPastEllipsis()) {

    // check if the "official" parameters are done but there are still
    // arguments
    if (paramIter.isDone()) {
      // this should be safe because paramIter.isDone() will not
      // return true otherwise, even if FunctionValue *fv got an
      // ellipsis added to it somehow at the last minute after the
      // ParamIter was created during all the arg->param dataflow that
      // has been going on
      xassert(!fv->hasEllipsis());
      if (fv->t()->asFunctionType()->hasFlag(FF_NO_PARAM_INFO)) {
        // this is a C function that has no parameter info; just match
        // up the args with phantom parameters that are made as we go
        // along
        ParamIter_NO_PARAM_INFO paramIter(fv);
        for (;
             arg0 != NULL;
             arg0 = arg0->next, paramIter.adv()) {
          Value *arg0Value = arg0->expr->abstrValue;
          DataFlowKind dfk = LE_DataFlowKind;
          Value *paramTarget = paramIter.data(arg0Value->t())->abstrValue();
          // FIX: GATED_EDGE omit the edge for a string literal argument as
          // it will cause const inference failure.  When we get gated
          // edges, put this edge back in and gate-off the const.
          if (!arg0->expr->isE_stringLit()) {
            eDataFlow_argToParam(arg0Value, paramTarget, loc,
                                 dfk, true /*mayUnrefSrc*/, fcId);
          }
        }
        // perhaps I should just trust the outer loop to terminate at
        // this point, but instead I will do it explicitly
        goto done;

      } else {
        // If it's an old-style declaration, it's legal to have too many
        // arguments.  It probably indicates a bug in the code though...
        //
        // UPDATE: dsw: it is legal to call a function with more
        // arugments than it needs, so just let it.
        goto done;

//         if (fv->isKandR()) {
//           xfailure("TODO: handle too many arguments to old-style function"
//                    " (c77e9b0c-8b23-46f4-b0cc-64da19719a05)");
//         }

//         // FIX: should this call report_qerror?
//         userFatalError(loc,
//                        "Function call has more arguments than the function has parameters");
      }
    }

    Value *arg0Value = arg0->expr->abstrValue;
    Value *paramTarget = NULL;

    DataFlowKind dfk = UNDEF_DataFlowKind; // undefined DataFlowKind
    if (paramIter.data()->type->isEllipsis()) {
      xassert(paramIter.data() == fv->getEllipsis() && "4ef4c3e9-7047-42ce-a305-4b4e19b2867a");
      dfk = EQ_DataFlowKind;
      // FIX: where the heck does the paramTarget get filed?  I think
      // nowhere.  This reflects cqual's behaivor, but it isn't very
      // satisfying as a data-flow analysis
      paramTarget = makeEllipsisMirrorParam
        (loc, arg0Value, paramIter.data()->abstrValue(), fv);
      // also put in the edge from the argument to the ellipsis
      // parameter directly
      eDataFlow_argToParam(arg0Value, paramIter.data()->abstrValue(), loc,
                           LE_DataFlowKind, true /*mayUnrefSrc*/, fcId);
    } else {
      dfk = LE_DataFlowKind;
      paramTarget = paramIter.data()->abstrValue();
    }
    // this could only happen if the *argument* type were ellipsis,
    // which should be impossible
    xassert(!paramTarget->t()->isEllipsis());

    if (paramTarget->t()->isCompoundType()) {
      // FIX: I don't know if it should be possible for this to not be
      // a reference type, but if it is not, make it one; right now
      // anyway, to see a situation where it may not be a reference
      // type, put the assertion back and run:
      // /home/dsw/ball/./kernel-2.4.18-3/STDIN-10232.g.i
      //        xassert(arg0Value->isReferenceValue());
      if (!arg0Value->isReferenceValue()) {
        arg0Value = arg0Value->asLval();
      }
      // by value argument passing is done by reference from the
      // call-site temporary
      paramTarget = paramTarget->asLval();
    }

    // FIX: GATED_EDGE omit the edge for a string literal argument as
    // it will cause const inference failure.  When we get gated
    // edges, put this edge back in and gate-off the const.
    if (!arg0->expr->isE_stringLit()) {
      eDataFlow_argToParam(arg0Value, paramTarget, loc,
                           dfk, true /*mayUnrefSrc*/, fcId);
    }
  }

  // FIX: this doesn't seem to be an error in K&R C, at least for
  // functions with no prototype
  if (!(paramIter.isDone()     // no more parameters
        || paramIter.data()->value // remaining parameters have defaults
        || paramIter.data()->type->isEllipsis()))
    {
      // FIX: should this call report_qerror?
      // FIX: This fails for /home/dsw/ball/./mozilla-0.9.9-7/STDIN-10000.g.ii
//      userFatalError(loc,
//                    "Function call has fewer arguments than the function has parameters");
    }

  done:;
}
