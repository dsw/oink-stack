// see License.txt for copyright and terms of use

#include "dataflow_ty.h"        // this module
#include "dataflow_visitor.h"   // DataFlowVisitor
#include "oink_global.h"        // oinkCmd
#include "oink_util.h"

bool isLegalDataFlowKind(DataFlowKind dfk) {
  switch(dfk) {
  default:                      // includes UNDEF_DataFlowKind
    return false;
  case LE_DataFlowKind:
  case EQ_DataFlowKind:
  case UN_DataFlowKind:
    return true;
  }
}

// a walk down the value tree changes the DataFlowKind as it goes down;
// the subIK is what DataFlowKind the walk would change it to under
// normal, non-unify, circumstances; this is what you get unless the
// currentIK trumps it with a unify (UN_DataFlowKind).
static inline DataFlowKind subDataFlowKind(DataFlowKind currentIK, DataFlowKind subIK)
{
  xassert(isLegalDataFlowKind(currentIK));
  xassert(isLegalDataFlowKind(subIK));
  if (currentIK == UN_DataFlowKind) return UN_DataFlowKind;
  else return subIK;
}


// DataFlowTy ****************

void DataFlowTy::tDataFlow
  (Value *src, Value *tgt, SourceLoc loc,
   DataFlowKind dfk, bool mayUnrefSrc, bool cast,
   bool useMI, LibQual::polarity miPol, int fcId)
{
  StableArgs stable(loc, cast, useMI, miPol, fcId);
  tDataFlow_dispatch(stable, src, tgt, dfk, mayUnrefSrc);
}

// This is basically eDataFlow_refUnify(); FIX: merge dataflow_ex and
// dataflow_ty so I don't have to do this
void DataFlowTy::tDataFlow_refUnify(Value *src, Value *tgt, SourceLoc loc) {
  StableArgs stable0(loc, /*cast*/false,
                     /*useMI*/false, /*miPol*/LibQual::p_non, /*fcId*/-1);
  tDataFlow_dispatch(stable0, src, tgt, UN_DataFlowKind, true);
}

// FIX: this should probably somehow be factored out into a
// non-dataflow specific place, but I'm not sure how to do it well
void DataFlowTy::tDataFlow_CVAtomicUnifyFields
  (StableArgs &stable, CVAtomicValue *src, CVAtomicValue *tgt)
{
  xassert(oinkCmd->instance_sensitive);
   // cout << "DataFlowTy::tDataFlow_CVAtomicUnifyFields(), loc:" << toString(stable.loc)
   //      << ", src->serialNumber:" << src->serialNumber
   //      << ", tgt->serialNumber:" << tgt->serialNumber << endl;
  BucketOValues *discardedBucket = src->unifyBuckets(tgt);
  xassert(src->getBucket() != NULL && src->getBucket() == tgt->getBucket());

  if (discardedBucket) {
    // union the two buckets
//      cout << "\tunion the two buckets" << endl;

    // which bucket was kept?
    BucketOValues *keptBucket = src->getBucket();
    // quarl 2006-05-19
    //     I don't see how this could ever be true, esp. given assertion
    //     above.

    // if (keptBucket == discardedBucket) {
    //   keptBucket = tgt->getBucket();
    // }
    xassert(keptBucket);
    xassert(keptBucket != discardedBucket);

    // Note that doing recrusive calls into the dataflow *before* we
    // are completely finished only works because it doesn't matter
    // what order dataflow edges are inserted into the graph.  Other
    // things that *do* have to be done first are making sure that 1)
    // the union-find has discareded the discarded bucket, 2) the
    // discarded bucket has been disabled, and 3) all the elements of
    // the discarded bucket that are going to get created in the kept
    // bucket are already in there.

    // copy over the fields that discardedBucket has and keptBucket
    // doesn't have; this must be done before the loop below; it only
    // works because the loop below doesn't insert edges between two
    // fields that are the same object
    xassert(keptBucket->isEnabled());
    keptBucket->getOthersExtraFields(*discardedBucket);

    // initialization of loop below; it is factored out so that we can
    // disable the discardedBucket after creating the iterator
    BucketOValues::PairIter iter(*keptBucket, *discardedBucket);
    // disable discardedBucket
    discardedBucket->disable();

    // unify the fields they share
    for(/*init done above*/; !iter.isDone(); iter.adv()) {
      // recurse
      string key = iter.data();
//        cout << "\t\tunifying key " << key << endl;
      // NOTE: this is the ONLY place that getEvenThoughDisabled()
      // should be used; I have to break the cyclic dependency
      // somewhere; I am able to narrow it down to this one place.
      // UPDATE: see the note below.
      Value *srcField = discardedBucket->getEvenThoughDisabled(key);
      xassert(srcField);
      // UPDATE: due to recursive types you can get a situation here
      // where the unification in this loop below caused this function
      // to be called recursively, the consequence of which is that
      // keptBucket may suddenly be disabled during the iteration of
      // this loop.  I have concluded that this is in fact ok because
      // of an invariant that really *must* hold which is that 1) no
      // fields are being ADDED to a disabled bucket, and 2) above, we
      // ATOMICALLY unify in the union-find two buckets before
      // iterating through their members.
//       Value *tgtField = keptBucket->get(key);
      Value *tgtField = keptBucket->getEvenThoughDisabled(key);
      xassert(tgtField);
      if (srcField != tgtField) {
        tDataFlow_refUnify(srcField, tgtField, stable.loc);
      }
    }
    // TODO: delete discardedBucket and set the pointer to it to NULL.
  }
}

void DataFlowTy::tDataFlow_CVAtomicTgt
  (StableArgs &stable, Value *src, CVAtomicValue *tgt, DataFlowKind dfk)
{
  xassert(!src->isReferenceValue());

  // FIX: VOID ALSO
  if (isAutoUnionValue(tgt) && oinkCmd->instance_sensitive) {
    xfailure("autounion types should have been eliminated already");
  }

  tDataFlow_one(stable, src, tgt, dfk);

  // NOTE: both of these branches below will be taken for an
  // isAutoUnionValue()
  if (tgt->hasAtValue() && src->hasAtValue()) {
    // int-as-void*; note: no matter the dfk at this level, we go to
    // EQ_DataFlowKind.
    tDataFlow_dispatch
      (stable, src->getAtValue(), tgt->getAtValue(),
       subDataFlowKind(dfk, EQ_DataFlowKind),
       true);
  }

  if (src->isCVAtomicValue()) {
    if (src->t()->isCompoundType() && tgt->t()->isCompoundType()) {
      if (oinkCmd->instance_sensitive) {
        tDataFlow_CVAtomicUnifyFields(stable, src->asCVAtomicValue(), tgt);
      }
    }
    // FIX: VOID ALSO
    else if (isAutoUnionValue(src) && oinkCmd->instance_sensitive) {
      xfailure("autounion types should have been eliminated already");
    }
  } else {
    // TODO: need a warning here?
    //
    // possible unsoundness
  }
}

void unifyEllipsisPtr0(DataFlowTy &dft, FunctionValue *a, FunctionValue *b) {
  if (a->ellipsisHolder) {
    if (b->ellipsisHolder) {
      // fallthrough
    } else {
      b->ellipsisHolder = a->ellipsisHolder;
      return;
    }
  } else {
    if (b->ellipsisHolder) {
      a->ellipsisHolder = b->ellipsisHolder;
      return;
    } else {
      a->setEllipsis(NULL);     // create a->ellipsisHolder
      b->ellipsisHolder = a->ellipsisHolder;
      return;
    }
  }

  // only get here if both already have ellipsisHolder
  xassert(a->ellipsisHolder && b->ellipsisHolder);
  bool a_hasEllipsis = a->hasEllipsis();
  bool b_hasEllipsis = b->hasEllipsis();
  if (a_hasEllipsis && b_hasEllipsis) {
    if (a->getEllipsis() != b->getEllipsis()) {
      dft.tDataFlow_refUnify(a->getEllipsis()->abstrValue(),
                             b->getEllipsis()->abstrValue(),
                             a->loc);
    } else {
      // nothing to do
    }
  } else if (a_hasEllipsis && !b_hasEllipsis) {
    b->setEllipsis(a->getEllipsis());
  } else if (!a_hasEllipsis && b_hasEllipsis) {
    a->setEllipsis(b->getEllipsis());
  } else if (!a_hasEllipsis && !b_hasEllipsis) {
    // neither has an ellipsis, but both will share the same
    // ellipsisHolder (see below), which is enough for now
  }

  a->ellipsisHolder = b->ellipsisHolder;
}

void unifyEllipsisPtr(DataFlowTy &dft, FunctionValue *a, FunctionValue *b) {
  unifyEllipsisPtr0(dft, a, b);
  xassert(a->ellipsisHolder);
  xassert(a->ellipsisHolder == b->ellipsisHolder);
  // note that we do not assert that a->getEllipsis() == b->getEllipsis()
}

void unifyRetVar(DataFlowTy &dft, FunctionValue *a, FunctionValue *b) {
  if (b->retVar) {
    xassert(a->retVar);      // a mis-match should not be possible
    dft.tDataFlow_refUnify(b->retVar->abstrValue(),
                           a->retVar->abstrValue(),
                           a->loc);
  } else {
    xassert(!a->retVar);     // a mis-match should not be possible
  }
}

// the implementation of this flow is potentially confusing.  If
// function f flows into function g that means that calling g means
// calling f, so f.ret -> g.ret (covariant rule) and g.args -> f.args
// (contravariant rule); the result of this is that much of the
// dataflow is the contravariant rule and ends up looking reversed.
// Note that the one-directional flow here, instead of straight
// function unification (that cqual does), is needed for dealing with
// virtual method overriding
void DataFlowTy::tDataFlow_FuncTgtFuncSrc
  (StableArgs &stable, Value *src, Value *tgt, bool cast, SourceLoc loc)
{
  bool useMI = true;            // Rob and I have decided to always have this on
  int fcId = globalNextFunCallId++;

  xassert(src->isFunctionValue());
  FunctionValue *src0 = src->asFunctionValue();
  // will fail if one is a function and the other a pointer, say; 6
  // apr 2003 Really?  Write a test for it.
  xassert(src0);

  xassert(tgt->isFunctionValue());
  FunctionValue *tgt0 = tgt->asFunctionValue();
  xassert(tgt0);

  // when either function is FF_NO_PARAM_INFO, we just do straight
  // unification
  bool src0_noParam = src0->t()->asFunctionType()->hasFlag(FF_NO_PARAM_INFO);
  bool tgt0_noParam = tgt0->t()->asFunctionType()->hasFlag(FF_NO_PARAM_INFO);
  bool unify = false;
  if (src0_noParam || tgt0_noParam) {
    xassert(!src0->t()->isMethod());
    xassert(!tgt0->t()->isMethod());
    unify = true;
  }

  // FIX: I suppose I should be sure can not assign to a defined
  // function.  12 feb 03 defined functions should now be const, so
  // const inference should catch that.

  Value *srcRetValue = src0->retValue;
  Value *tgtRetValue = tgt0->retValue;
  // put the edge between the retVar-s
  if (srcRetValue->t()->isCompoundType()) {
    // NOTE: Contravariant rule ****
    //
    // This is the contravariant rule because retVar is a parameter
    //
    // this is a normal parameter edge; note that we are unifying the
    // two variables, so we unify them by reference

    // Note: 8efc322b-1a2b-42c6-a387-ba23f9ce30ee is known to occur in two
    // Debian packages and both of them are bugs in the code-under-analysis.
    // dsw: ok, so I'll make it a user error.
    USER_ASSERT(tgtRetValue->t()->isCompoundType(), loc,
                "Cannot flow between two functions where one returns a compound type"
                " and the other does not; 8efc322b-1a2b-42c6-a387-ba23f9ce30ee");
    xassert(tgt0->getRetVar()->getType()->asRval()->isCompoundType());
    xassert(src0->getRetVar()->getType()->asRval()->isCompoundType());
    if (unify) {
      tDataFlow_refUnify
        (tgt0->getRetVar()->abstrValue()->asLval(),
         src0->getRetVar()->abstrValue()->asLval(),
         loc);
    } else {
      tDataFlow
        (tgt0->getRetVar()->abstrValue()->asLval(),
         src0->getRetVar()->abstrValue()->asLval(),
         loc,
         // don't do this: 'dfk'; do this instead (recalling that
         // functions have no state):
         LE_DataFlowKind, true, cast,
         useMI, LibQual::p_neg, fcId);
    }
  } else {
    // NOTE: Covariant rule ****

    // As above in the reverse situation (src/tgt reversed), I really
    // can't think of any other interpretation of this other than as a
    // user error.
    USER_ASSERT(!tgt0->retValue->t()->isCompoundType(), loc,
                "Cannot flow between two functions where one returns a compound type"
                " and the other does not f66eefd9-fad0-4b47-b57f-558b5f877f67");
    xassert(!src0->retVar);
    xassert(!tgt0->retVar);
    if (unify) {
      tDataFlow_refUnify(srcRetValue, tgtRetValue, loc);
    } else {
      tDataFlow
        (srcRetValue, tgtRetValue, loc,
         // NOTE: we don't use insertion kind here, but just add
         // them the right way.  That is, don't do this: dfk, but
         // instead do this (recall that functions have no
         // state):
         LE_DataFlowKind, true, cast,
         useMI, LibQual::p_pos, fcId);
    }
  }

  // parameters of one asssigned (as if they were arguments ?) to
  // parameters of the other
  ParamIter srcParamsIter(src0);
  ParamIter tgtParamsIter(tgt0);
  while (!(srcParamsIter.isDone() || tgtParamsIter.isDone())) {
    // NOTE: Contravariant rule ****
    // parameters that are passed by value have a temporary for them
    // copy constructed at the call site and a reference to it is
    // passed in
    Value *tgtParamValue = tgtParamsIter.data()->abstrValue();
    Value *srcParamValue = srcParamsIter.data()->abstrValue();
    if (tgtParamValue->t()->isCompoundType()) tgtParamValue = tgtParamValue->asLval();
    if (srcParamValue->t()->isCompoundType()) srcParamValue = srcParamValue->asLval();

    if (unify) {
      tDataFlow_refUnify(tgtParamValue, srcParamValue, loc);
    } else {
      tDataFlow
        (tgtParamValue, srcParamValue, loc,
         // don't do this: 'dfk'; do this instead (recalling that
         // functions have no state):
         LE_DataFlowKind, true, cast,
         useMI, LibQual::p_neg, fcId);
    }

    // NOTE: This must be done AFTER the insertion, or you might never
    // do it at all!  DO NOT put this into the while clause.
    if (srcParamsIter.data()->type->isEllipsis()) {
      if (tgtParamsIter.data()->type->isEllipsis()) {
        // this is the primary way to get out of the loop when both
        // function end in an ellipsis
        break;
      } else {
        if (tgt0_noParam) break; // jump out, the rest of the params are handled below
        // After long consultation with Scott, we decided that the
        // best thing to do here is 1) ensure that both functions have
        // an ellipsis and 2) flow all of the remaining parameters of
        // the longer parameter list into the ellipsis of the shorter
        // parameter list; that means just keep going around the loop.
        tgt0->ensureEllipsis();
//         userReportError(loc,
//                         "Source function has fewer parameters before "
//                         "the ellipsis than target function "
//                         "(4798ba56-1da1-48ce-8371-1cfddccfd17c).");
//         userFatalError(src->loc,
//                        "This is the location of the source function call.");
      }
    } else {
      if (tgtParamsIter.data()->type->isEllipsis()) {
        if (src0_noParam) break; // jump out, the rest of the params are handled below
        // as above, we just ensure src has an ellipsis and keep going
        // around the loop
        src0->ensureEllipsis();
//         USER_ASSERT(!tgtParamsIter.data()->type->isEllipsis(), loc,
//                     "Target function has fewer parameters before "
//                     "the ellipsis than source function "
//                     " (09706d00-1974-4571-9282-63b8eef6a833).");
//         userFatalError(tgt->loc,
//                        "This is the location of the target function call.");
      } else {
        // this means that they were both just normal parameters and
        // so we just keep going
      }
    }

    srcParamsIter.advanceButNotPastEllipsis();
    tgtParamsIter.advanceButNotPastEllipsis();
  }

  // deal with FF_NO_PARAM_INFO functions
  if (srcParamsIter.isDone()
      // have to remove this condition because of stupid examples like
      // Test/inconsistent_func_type_param2.c that gcc is still
      // willing to compile because it is a complete slut
//       && src0_noParam
      && !tgtParamsIter.isDone()) {
    // src is a C function that has no parameter info; just match up
    // the args with phantom parameters that are made as we go along
    ParamIter_NO_PARAM_INFO srcParamsIter(src0);
    for(;
        !tgtParamsIter.isDone();
        tgtParamsIter.advanceButNotPastEllipsis(), srcParamsIter.adv()) {
      Value *tgtParamValue = tgtParamsIter.data()->abstrValue();
      // there is no point in making phantom parameters to match an
      // ellipsis
      if (tgtParamValue->t()->isEllipsis()) break;
      Value *srcParamValue = srcParamsIter.data(tgtParamsIter.data()->type)->abstrValue();
      xassert(!srcParamValue->t()->isEllipsis());
      // changed this to a warning to avoid failing outright in some
      // situations that I don't know how to handle right now such as
      // oink/Test/noparam_compound1.c
//       xassert(!tgtParamValue->t()->isCompoundType() &&
//               "a42702dd-37d7-41b7-ba95-74e555e3e98e");
//       xassert(!srcParamValue->t()->isCompoundType() &&
//               "64cec768-cabc-494d-ba75-d83041125941");
//       if (tgtParamValue->t()->isCompoundType() ||
//           srcParamValue->t()->isCompoundType()) {
//         // don't know how to deal with this right now
//         userReportWarning
//           (stable.loc,
//            "Unsoundness warning: TODO: FF_NO_PARAM_INFO function meets another function with"
//            " an extra param that is a CompoundType passed by value, "
//            "a situation hard to match up with an 'int' phantom parameter, "
//            "so I just drop the edge for now; "
//            "a42702dd-37d7-41b7-ba95-74e555e3e98e, "
//            "64cec768-cabc-494d-ba75-d83041125941");
//         continue;
//       }
      if (unify) {
        tDataFlow_refUnify(tgtParamValue, srcParamValue, loc);
      } else {
        tDataFlow
          (tgtParamValue, srcParamValue, loc,
           // don't do this: 'dfk'; do this instead (recalling that
           // functions have no state):
           LE_DataFlowKind, true, cast,
           useMI, LibQual::p_neg, fcId);
      }
    }
  }

  // note: src and tgt are revesed here with respect to the above in
  // the if and loop preamble, but in the body of the loop they are
  // not reversed; UPDATE: not sure about that anymore
  if (tgtParamsIter.isDone()
      // have to remove this condition because of stupid examples like
      // Test/inconsistent_func_type_param2.c that gcc is still
      // willing to compile because it is a complete slut
//       && tgt0_noParam
      && !srcParamsIter.isDone()) {
    // tgt is a C function that has no parameter info; just match up
    // the args with phantom parameters that are made as we go along
    ParamIter_NO_PARAM_INFO tgtParamsIter(tgt0);
    for(;
        !srcParamsIter.isDone();
        srcParamsIter.advanceButNotPastEllipsis(), tgtParamsIter.adv()) {
      // exactly the same body as above
      Value *srcParamValue = srcParamsIter.data()->abstrValue();
      // there is no point in making phantom parameters to match an
      // ellipsis
      if (srcParamValue->t()->isEllipsis()) break;
      Value *tgtParamValue = tgtParamsIter.data(srcParamsIter.data()->type)->abstrValue();
      xassert(!tgtParamValue->t()->isEllipsis());
//       xassert(!tgtParamValue->t()->isCompoundType() &&
//               "794cd149-c450-47ab-9f0a-e489e5d90110");
//       xassert(!srcParamValue->t()->isCompoundType() &&
//               "0c09b0c3-344d-400a-be48-53961099883a");
//       if (tgtParamValue->t()->isCompoundType() ||
//           srcParamValue->t()->isCompoundType()) {
//         // don't know how to deal with this right now
//         userReportWarning
//           (stable.loc,
//            "Unsoundness warning: TODO: FF_NO_PARAM_INFO function meets another function with"
//            " an extra param that is a CompoundType passed by value, "
//            "a situation hard to match up with an 'int' phantom parameter, "
//            "so I just drop the edge for now; "
//            "794cd149-c450-47ab-9f0a-e489e5d90110,"
//            "0c09b0c3-344d-400a-be48-53961099883a");
//         continue;
//       }
      if (unify) {
        tDataFlow_refUnify(tgtParamValue, srcParamValue, loc);
      } else {
        tDataFlow
          (tgtParamValue, srcParamValue, loc,
           // don't do this: 'dfk'; do this instead (recalling that
           // functions have no state):
           LE_DataFlowKind, true, cast,
           useMI, LibQual::p_neg, fcId);
      }
    }
  }

  // We share one params list for C functions with omitted parameter
  // info.  This is necessary for correctness as such functions have
  // parameters created in their params list as they encounter other
  // 1) functions or 2) function calls; the creation of these params
  // depends on the order in which edges are added to the dataflow
  // graph so in certain circumstances both sides will later get
  // another param but these will never meet as the two functions have
  // already met.  The only way to handle this is that when the two
  // functions meet their parameter lists are paired up and then only
  // one is used from now on.  This problem and its solution is
  // parallel to that of the BucketOValues problem.
  //
  // After the parameter lists have been unified, we now throw one
  // away and share the other from now on; we are unifying so it
  // doesn't matter which one we keep if the lengths are the same;
  // however if not, keep the longer list.  FIX: we could delete the
  // discarded ones.
  //
  // dsw: I find it very strange that it is possible for a
  // FF_NO_PARAM_INFO function to have a retVar, but they do:
  // Test/return_struct_fs.c (the filter-good vesion at least); I will
  // assume that if one has a retVar that all copies have to have one,
  // rather than allowing the ability to introduce one later.
  if (src0_noParam) {
    if (tgt0_noParam) {
      xassert(unify);
      if (src0->params->count() > tgt0->params->count()) {
        // keep src0 params
        unifyEllipsisPtr(*this, tgt0, src0);
        unifyRetVar(*this, tgt0, src0);
        tgt0->params = src0->params;
      } else {
        // keep tgt0 params
        unifyEllipsisPtr(*this, tgt0, src0);
        unifyRetVar(*this, tgt0, src0);
        src0->params = tgt0->params;
      }
    } else {
      xassert(unify);
      // have to unify the extra arguments of src0 with those of tgt0
      // before we throw them away
      unifyEllipsisPtr(*this, tgt0, src0);
      if (tgt0->params->count() < src0->params->count()) {
        tgt0->ensureEllipsis();
        for (int i = tgt0->params->count(); i<src0->params->count(); ++i) {
          Variable_O *srcParam = src0->params->nth(i);
          tDataFlow_refUnify(asVariable_O(tgt0->getEllipsis())->abstrValue(),
                             srcParam->abstrValue(),
                             loc);
        }
      }
      // keep tgt0 params
      unifyRetVar(*this, tgt0, src0);
      src0->params = tgt0->params;
    }
  } else {
    if (tgt0_noParam) {
      xassert(unify);
      // have to unify the extra arguments of tgt0 with those of src0
      // before we throw them away
      unifyEllipsisPtr(*this, tgt0, src0);
      if (src0->params->count() < tgt0->params->count()) {
        src0->ensureEllipsis();
        for (int i = src0->params->count(); i<tgt0->params->count(); ++i) {
          Variable_O *tgtParam = tgt0->params->nth(i);
          tDataFlow_refUnify(src0->getEllipsis()->abstrValue(), tgtParam->abstrValue(), loc);
        }
      }
      // keep src0 params
      unifyRetVar(*this, tgt0, src0);
      tgt0->params = src0->params;
    } else {
      // nothing to do; fallthrough
    }
  }

  // deal with ellipsis; for now, if either one has an ellipsis, both
  // get one; FIX: this is possibly a soundness hole, but we are
  // encountering some strange situations and it is better for the
  // code to go through than to bork due to an obscure soundness hole
  if (src0->hasEllipsis() || tgt0->hasEllipsis()) {
    // NOTE: Contravariant rule ****
    src0->ensureEllipsis();
    tgt0->ensureEllipsis();
//     USER_ASSERT(tgt0->hasEllipsis(), loc,
//                 "Source function has ellipsis but target function does not"
//                 " (2679bf5a-87cb-40ca-891f-28e060d30cf3).");
//     // TODO: print location of both source and dest
    if (unify) {
      tDataFlow_refUnify(tgt0->getEllipsis()->abstrValue(),
                         src0->getEllipsis()->abstrValue(),
                         loc);
    } else {
      tDataFlow(tgt0->getEllipsis()->abstrValue(), src0->getEllipsis()->abstrValue(), loc,
                UN_DataFlowKind /*unify*/, true /*even the refs*/, false /*not as cast insertion*/,
                /*useMI*/ false, /*miPol*/ LibQual::p_non, /*fcId*/ -1);
    }
//   } else {
//     USER_ASSERT(!tgt0->hasEllipsis(), loc,
//                 "Target function has ellipsis but source function does not.");
  }

}

void DataFlowTy::tDataFlow_FuncTgt
  (StableArgs &stable, Value *src, FunctionValue *tgt, DataFlowKind dfk)
{
  if (src->isFunctionValue()) {
    tDataFlow_one(stable, src, tgt, dfk);
    return;
  }
  if (src->isCVAtomicValue()) {
    // be sure to allow this: virtual int f(int x) = 0;
    AtomicType *atom = src->asCVAtomicValue()->type->atomic;
    if (atom->isSimpleType() &&
        (atom->asSimpleTypeC()->type == ST_INT ||
         // and assigning a ((void*)0) to a function pointer
         atom->asSimpleTypeC()->type == ST_VOID)
        ) {
      return;
    }
    // can't think of a way to check that it is the constant '0'.

    if (atom->isSimpleType() && atom->asSimpleTypeC()->type == ST_CHAR) {
      // char-as-void

      userReportWarning(stable.loc, "TODO: char-as-void (2de5d12e-4f37-4bfd-9138-9ae764e8afbb)");
      // fall-through to end
    }

    // Don't return - fall through to bottom error message.

    // userFatalError(stable.loc,
    //            stringc
    //            << "Can't insert edge from a non-int atomic "
    //            << src->t()->toString()
    //            << " to function "
    //            << tgt->t()->toString()
    //            << " (9fe4294a-ac9e-4ae4-b592-86c5c9cae54f)");
    // return;
  }
  if (src->isPointerValue()) {
    Value *at = src->asPointerValue()->getAtValue();
    if (at->t()->isSimpleType() &&
        (at->t()->asSimpleTypeC()->type == ST_INT ||
         // and assigning a ((void*)0) to a function
         at->t()->asSimpleTypeC()->type == ST_VOID)
        ) {
      // allow edges from int* or void* to functions; the mis-matched
      // layer can happen when a ?: is evaluated and the 'then' clause
      // is a function (a function literal has function type, not
      // pointer to function type) but the 'else' clause is a
      // ((void*)0); the type of the whole conditional is inferred from
      // the 'then' clause and then the void* flows to it, a function
      // type
      return;
    } else if (at->isFunctionValue()) {
      xassert("can this happen? 7524d880-c018-4a70-a33a-2fafa0f03a85");
      tDataFlow_one(stable, at->asFunctionValue(), tgt, dfk);
      return;
    }
  }
  if (src->t()->isUnionType()) {
    userReportWarning(stable.loc, "TODO: automatic cast-as-union-construction");
  }

  // 9fe4294a-ac9e-4ae4-b592-86c5c9cae54f
  userReportWarning(stable.loc,
                    stringc
                    << "Unsoundness warning: TODO: Not inserting edge from a non-function '"
                    << src->t()->toString()
                    << "' to function '"
                    << tgt->t()->toString()
                    << "'"
                    // << " (9fe4294a-ac9e-4ae4-b592-86c5c9cae54f)"
    );
}

void DataFlowTy::tDataFlow_PtrOrRefTgt
  (StableArgs &stable, Value *src, Value *tgt, DataFlowKind dfk)
{
  // NOTE: includes references also
  xassert(tgt->isPointerValue() || tgt->isReferenceValue());
  switch(src->getTag()) {
  default: xfailure("illegal tag"); break;
  case Value::V_ATOMIC:
    // you get this when you make an edge from a void* to a struct foo**
    tDataFlow_one(stable, src, tgt, dfk);
//      if (!(src->isIntegerType() || src->isVoid())) {
//        // FIX: This fails in /home/dsw/ball/mozilla-0.9.9-7/STDIN-10000.g.ii
//        userFatalError(stable.loc, "Can't insert edge from a non-integer atomic to a pointer.");
//      }
    if (src->hasAtValue()) {
      // int-as-void*; no matter the dfk at this level, we go to
      // EQ_DataFlowKind.
      tDataFlow_dispatch
        (stable, src->getAtValue(), tgt->getAtValue(),
         subDataFlowKind(dfk, EQ_DataFlowKind),
         true);
    }
    break;
  case Value::V_FUNCTION: {    // assignment to pointer to function from function
    // quarl 2006-07-26
    //    Elsa will handle this eventually.
    // xfailure("flowing function to pointer to function (9cc54383-8e24-4a23-8df2-a802fe9abd03)");

    tDataFlow_one(stable, src, tgt, dfk);
    FunctionValue *src0 = src->asFunctionValue();
    xassert(src0);
    // FIX: hmm, this should probably just check right here that tgt
    // points to a function
    tDataFlow_dispatch
      (stable, src0, tgt->getAtValue(), // remove layer of indirection
       subDataFlowKind
       (dfk,
        // FIX: why LE? because functions have no state???
        LE_DataFlowKind),
       true);
  }
  break;
  case Value::V_POINTER:
  case Value::V_REFERENCE:
  case Value::V_ARRAY:
    tDataFlow_one(stable, src, tgt, dfk);
    // no matter the dfk at this level, we go to EQ_DataFlowKind.
    tDataFlow_dispatch
      (stable, src->getAtValue(), tgt->getAtValue(),
       subDataFlowKind(dfk, EQ_DataFlowKind),
       true);
    break;
  case Value::V_POINTERTOMEMBER:
    xfailure("Can't insert edge from pointer-to-member to pointer.");
    break;
  }
}

void DataFlowTy::tDataFlow_ArrayTgt
  (StableArgs &stable, Value *src, ArrayValue *tgt, DataFlowKind dfk)
{
  switch(src->getTag()) {
  default: xfailure("illegal tag"); break;
  case Value::V_ATOMIC: {
    CVAtomicValue *src0 = src->asCVAtomicValue();
    tDataFlow_one(stable, src0, tgt, dfk);
    if (src0->hasAtValue()) {
      // int-as-void*; no matter the dfk at this level, we go to
      // EQ_DataFlowKind.
      tDataFlow_dispatch
        (stable, src0->getAtValue(), tgt->getAtValue(),
         subDataFlowKind(dfk, EQ_DataFlowKind),
         true);
    }
    break; }
  case Value::V_FUNCTION:
    xfailure("attempt to assign a function type to an array type");
    break;
  case Value::V_POINTER:
  case Value::V_REFERENCE:
  case Value::V_ARRAY:
    tDataFlow_one(stable, src, tgt, dfk);
    // no matter the dfk at this level, we go to EQ_DataFlowKind.
    tDataFlow_dispatch
      (stable, src->getAtValue(), tgt->atValue,
       subDataFlowKind(dfk, EQ_DataFlowKind),
       true);
    break;
  case Value::V_POINTERTOMEMBER:
    xfailure("Can't insert edge from pointer-to-member to array.");
    break;
  }
}

void DataFlowTy::tDataFlow_PtrToMemTgt
  (StableArgs &stable, Value *src, PointerToMemberValue *tgt, DataFlowKind dfk)
{
  switch(src->getTag()) {
  default: xfailure("illegal tag"); break;
  case Value::V_ATOMIC:
    // quarl 2006-05-28 perhaps this should accept "0" (NULL) pointers?
//     userFatalError(stable.loc, "Can't insert edge from an atomic to a pointer-to-member (18cca471-ba67-40dd-865a-c787b1c65ade).");
    // dsw: whatever; I don't care what this means I'll just put it in.
    // quarl TODO: check to make sure it is indeed 0 ?
    tDataFlow_one(stable, src, tgt, dfk);
    break;
  case Value::V_FUNCTION: {
//      userFatalError(loc, "Can't insert edge from a function to a pointer-to-member.");
    tDataFlow_one(stable, src, tgt, dfk);
    FunctionValue *src0 = src->asFunctionValue();
    xassert(src0);
    tDataFlow_dispatch
      (stable, src0, tgt->getAtValue(), // remove layer of indirection
       subDataFlowKind
       (dfk,
        // FIX: why LE? because functions have no state???
        LE_DataFlowKind),
       true);
  }
  break;
  case Value::V_REFERENCE: {
    tDataFlow_one(stable, src, tgt, dfk);
    // no matter the dfk at this level, we go to EQ_DataFlowKind.
    tDataFlow_dispatch
      (stable, src->getAtValue(), tgt->getAtValue(),
       subDataFlowKind(dfk, EQ_DataFlowKind),
       true);
  }
  break;
  case Value::V_POINTER:
    userFatalError(stable.loc, "Can't insert edge from a pointer to a pointer-to-member.");
    break;
  case Value::V_ARRAY:
    userFatalError(stable.loc, "Can't insert edge from an array to a pointer-to-member.");
    break;
  case Value::V_POINTERTOMEMBER:
    // FIX: need to check that the inClass members have the correct
    // inheritance relationship.
    tDataFlow_one(stable, src, tgt, dfk);
    // no matter the dfk at this level, we go to EQ_DataFlowKind.
    tDataFlow_dispatch
      (stable, src->getAtValue(), tgt->getAtValue(),
       subDataFlowKind(dfk, EQ_DataFlowKind),
       true);
    break;
  }
}

void DataFlowTy::tDataFlow_dispatch
  (StableArgs &stable, Value *src, Value *tgt,
   DataFlowKind dfk,
   bool mayUnrefSrc)
{
  xassert(src);
  xassert(tgt);

#if DEBUG_INSTANCE_SPECIFIC_VALUES
  // check have not been marked as instance-specific;
  // dataDeclaratorValues is a subset of instanceSpecificValues; if it
  // is one of those we want to find out first.
  xassert(!dataDeclaratorValues.contains(src));
  xassert(!dataDeclaratorValues.contains(tgt));
  // FIX: turn this off for now for C++
  if (oinkCmd->instance_sensitive) {
    xassert(!instanceSpecificValues.contains(src));
    xassert(!instanceSpecificValues.contains(tgt));
  }
#endif

  // make reference-ness of src match that of tgt
  if (tgt->isReferenceValue()) src = src->asLval();
  else if (mayUnrefSrc) src = src->asRval();

  if (src == tgt) {
    // this is off when merge_E_variable_and_var_values is on because
    // we now sometimes use the same Value annotation for multiple
    // object (Expressions, Varibles) and we also use the existance of
    // annotating qvars as evidence of dataflow; therefore there must
    // sometimes be dataflow even between something and itself just to
    // get it annotated with qvars Don't assign self-edges for
    // non-useMI edges.
    if (!oinkCmd->merge_E_variable_and_var_values) {
      // this can now happen because we unify the parameter lists of
      // function values when one of them is FF_NO_PARAM_INFO
      //     if (stable.useMI) xfailure("we thought this can't happen; tell Rob.");
      //     else return;
      return;
    }
  }

  // We simply don't insert edges between the ellipsis of function values
//    // There is no way to properly insert edges between functions with
//    // an ellipsis.  Not sure what I will do if this ever happens.
//    if (src->isFunctionValue()) {
//      FunctionValue *src_func = src->asFunctionValue();
//      USER_ASSERT
//        (!src_func->ellipsis,
//         src->loc,
//         "Functions with an ellipsis cannot participate in edge insertion");
//    }
//    if (src->isFunctionValue()) {
//      FunctionValue *tgt_func = tgt->asFunctionValue();
//      USER_ASSERT
//        (!tgt_func->ellipsis,
//         tgt->loc,
//         "Functions with an ellipsis cannot participate in edge insertion");
//    }

  // insert the edges even if one is a void
  // FIX: VOID ALSO
//    tDataFlow_doubleRecurse(stable, src, tgt, dfk);

  // insert any additional edges due to autounion-polymorphism
  if (oinkCmd->instance_sensitive) {
    // fprintf(stderr, "##            tDataFlow_dispatch: isAutoUnionValue(%p)=%d isAutoUnionValue(%p)=%d\n",
    //         src, isAutoUnionValue(src), tgt, isAutoUnionValue(tgt));

    if (isAutoUnionValue(src)) {
      if (isAutoUnionValue(tgt)) {
        // both are autounion
        tDataFlow_CVAtomicUnifyFields(stable, src->asCVAtomicValue(), tgt->asCVAtomicValue());
        return;                   // we're done
      } else {
        // src is autounion, tgt is not
        src = src->asCVAtomicValue()->
          getInstanceSpecificValue(mangleToStringRef_TypeEquiv(tgt->t()), tgt);
      }
    } else {
      if (isAutoUnionValue(tgt)) {
        // tgt is autounion, src is not
        xassert(!isAutoUnionValue(src));
        tgt = tgt->asCVAtomicValue()->
          getInstanceSpecificValue(mangleToStringRef_TypeEquiv(src->t()), src);
      } else {
        // neither are autounion; there is nothing to do
        // FIX: VOID ALSO
        //        return;
      }
    }
    xassert(!isAutoUnionValue(src));
    xassert(!isAutoUnionValue(tgt));
    // this can easily happen with void-polymorphism
    if (src == tgt) return;
  }
  if (!oinkCmd->merge_E_variable_and_var_values) {
    xassert(src != tgt);
  }
  tDataFlow_doubleRecurse(stable, src, tgt, dfk);
}

void DataFlowTy::tDataFlow_doubleRecurse
  (StableArgs &stable, Value *src, Value *tgt,
   DataFlowKind dfk)
{
  switch(tgt->getTag()) {
  default: xfailure("illegal value"); break;
  case Value::V_ATOMIC:
    tDataFlow_CVAtomicTgt(stable, src, tgt->asCVAtomicValue(), dfk); break;
  case Value::V_FUNCTION:
    tDataFlow_FuncTgt(stable, src, tgt->asFunctionValue(), dfk); break;
  case Value::V_POINTER:
  case Value::V_REFERENCE:
    tDataFlow_PtrOrRefTgt(stable, src, tgt, dfk); break;
  case Value::V_ARRAY:
    tDataFlow_ArrayTgt(stable, src, tgt->asArrayValue(), dfk); break;
  case Value::V_POINTERTOMEMBER:
    tDataFlow_PtrToMemTgt(stable, src, tgt->asPointerToMemberValue(), dfk); break;
  }
}
