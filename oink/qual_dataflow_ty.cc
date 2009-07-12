// see License.txt for copyright and terms of use

#include "qual_dataflow_ty.h"   // this module
#include "qual_global.h"        // qualCmd
#include "qual_annot.h"         // QualAnnot
#include "oink_util.h"

// DataFlowTy_Qual ****

void DataFlowTy_Qual::tDataFlow_one
  (StableArgs &stable, Value *src, Value* tgt, DataFlowKind dfk)
{
//    cout << "src->serialNumber: " << src->serialNumber << endl;
//    cout << "tgt->serialNumber: " << tgt->serialNumber << endl;
  LibQual::Type_qualifier *srcQvar = qa(src)->getQVar();
  LibQual::Type_qualifier *tgtQvar = qa(tgt)->getQVar();

  if (qualCmd->inference) {
    xassert(srcQvar);
    xassert(tgtQvar);
    //  NOTE: test/simple4.c demonstrates that it is not the case that
    //  you can't distinguish between two qvars that have been
    //  unified.  Turn these assertions back on to see it.
    //      if (src->isReferenceValue() && tgt->isPointerValue() &&
    //          is_ref_ptr_pair(src, tgt)) {
    //        xassert(LibQual::eq_qual(srcQvar, tgtQvar));
    //        return;
    //      }
    //      if (tgt->isReferenceValue() && src->isPointerValue() &&
    //          is_ref_ptr_pair(tgt, src)) {
    //        xassert(LibQual::eq_qual(srcQvar, tgtQvar));
    //        return;
    //      }
  }

  xassert(isLegalDataFlowKind(dfk));

  if (dfk == UN_DataFlowKind) {
    xassert(!stable.useMI);
    unify(srcQvar, tgtQvar, stable.loc);
  } else {
    // NOTE: the tgt MAY be CONST, for example for edges due to
    // initialization
    mkLeq(srcQvar, tgtQvar, stable.loc, stable.cast,
          stable.useMI, stable.miPol, stable.fcId);

    // do it the other way as well if not const; That is, there is never
    // any point in putting an edge TO a const.
    if (dfk == EQ_DataFlowKind) {
      if (qualCmd->use_const_subtyping && isConstForInferencePurposes(tgt)) {
      } else {
        // Rob gives his seal of approval for this call
        mkLeq
          (tgtQvar, srcQvar /*REVERSED from the above*/, stable.loc, stable.cast,
           stable.useMI, invertPolarity(stable.miPol), stable.fcId);
      }
    }
  }
}

void DataFlowTy_Qual::tDataFlow_FuncTgtFuncSrc
  (StableArgs &stable, Value *src, Value *tgt, bool cast, SourceLoc loc)
{
  if (!qualCmd->casts_preserve_below_functions) xassert(!cast);

  FunctionValue *src0 = src->asFunctionValue();
  xassert(src0);

  FunctionValue *tgt0 = tgt->asFunctionValue();
  xassert(tgt0);

  // delegate to superclass
  DataFlowTy::tDataFlow_FuncTgtFuncSrc(stable, src, tgt, cast, loc);
}

void DataFlowTy_Qual::tDataFlow_FuncTgt
  (StableArgs &stable, Value *src, FunctionValue *tgt, DataFlowKind dfk)
{
  // delegate to superclass
  DataFlowTy::tDataFlow_FuncTgt(stable, src, tgt, dfk);
  if (src->isFunctionValue()) {
    //  Thu, 02 Jan 2003 10:40:57 -0500
    //  >     Cqual does not preserve qualifiers on structure fields, function
    //  > arguments, or function result types at casts even if casts-preserve is
    //  > enabled.
    //  The way -fcasts-preserve works, it walks down the type
    //  structure of the two types and matches/collapses the
    //  qualifiers.  What that comment in the manual means is that,
    //  during its walk, when it gets to a function type a->b or a
    //  struct type struct foo { ... }, it does not walk down to a, b,
    //  or the fields of foo.  This is purely a heuristic: if you do
    //  walk down function and structure types, the results tends to
    //  be too conservative to be useful, in my experience.  Jeff
    if (qualCmd->casts_preserve_below_functions || !stable.cast) {
      // Special case: below here, _dfk_ is subsequently ignored, so if
      // it is UN_DataFlowKind, we should handle it here
      tDataFlow_FuncTgtFuncSrc(stable, src, tgt, stable.cast, stable.loc);
      if (dfk==UN_DataFlowKind || dfk==EQ_DataFlowKind/*sm*/) {
        // NOTE: the source and target arguments to this call are
        // REVERSED as compared to those above
        tDataFlow_FuncTgtFuncSrc(stable, tgt, src, stable.cast, stable.loc);
      }
    }
  }
}


// for debugging; allow the user in gdb to intercept all edge
// constructions in one place; return a unique id for this edge
int incEdgeNumber(LibQual::Type_qualifier *src,
                  LibQual::Type_qualifier *tgt,
                  SourceLoc loc)
{
#if !DELAY_SET_NAMES
  // make these easily available to the debugger
  // NOTE: src and tgt can be null if inference is turned off
  char const *srcName = src ? LibQual::name_qual(src) : NULL;
  char const *tgtName = tgt ? LibQual::name_qual(tgt) : NULL;

  (void) srcName;                                   // prevent warnings in g++
  (void) tgtName;
#endif

  // here is a good place for the breakpoint
  return globalEdgeNumber++;
}

void mkLeq
  (LibQual::Type_qualifier *src, LibQual::Type_qualifier *tgt, SourceLoc loc,
   bool isCast,
   bool useMI, LibQual::polarity miPolCqual, int fcId)
{
  int edgeNumber = incEdgeNumber(src, tgt, loc);

  // if the user doesn't want inference, don't do it
  if (!qualCmd->inference) return;

  // Don't insert self-loops.
//    if (useMI) xassert(src != tgt);// should never even try for polymorphic edges
//    else if (src == tgt) return;
  // FIX: replace the below with the above.
  // FIX: I'll bet this is wrong for the polymorphic analysis
  if (!oinkCmd->merge_E_variable_and_var_values) {
    xassert(src != tgt);
  }

  // FIX: wrote Rob on 21 Feb 2005 to ask what should happen here in
  // the if(useMI) case, which used to say as follows
//      xassert(!isCast);           // can't see how it could be
  LibQual::qual_gate *qgate = isCast
    ? &LibQual::casts_preserve_qgate
    : &LibQual::open_qgate;
  // NOTE: This is probably an obsolete flag, but I keep it
  // implemented for testing purposes.
  if (qualCmd->casts_preserve) qgate = &LibQual::open_qgate;

  if (useMI) {
    xassert(miPolCqual!=LibQual::p_non);
    xassert(fcId!=-1);
    xassert(isPosOrNegPolarity(miPolCqual));
    mkinst_qual_CQUAL(loc,
                      qgate,
                      src, tgt,
                      miPolCqual,
                      fcId,
                      edgeNumber);
  } else {
    mkleq_qual_CQUAL(loc,
                     qgate,
                     src, tgt,
                     edgeNumber);
  }
}

void unify
  (LibQual::Type_qualifier *src, LibQual::Type_qualifier *tgt, SourceLoc loc)
{
  int edgeNumber = incEdgeNumber(src, tgt, loc);

  // if the user doesn't want inference, don't do it
  if (!qualCmd->inference) return;

  // Don't insert self-loops.
//    if (useMI) xassert(src != tgt);// should never even try for polymorphic edges
//    else if (src == tgt) return;
  // FIX: replace the below with the above.
  // FIX: I'll bet this is wrong for the polymorphic analysis
  if (!oinkCmd->merge_E_variable_and_var_values) {
    xassert(src != tgt);
  }

  // FIX: we temporarily add bidirectional edges instead of unifying

//  Subject:   Re: unify vs. bidirectional edges
//       Date: Fri, 22 Apr 2005 08:50:17 -0700
//       From: Rob Johnson
//         To: "Daniel S. Wilkerson"
//
//  Daniel S. Wilkerson wrote:
//  > When I unify two qual variables, one of them really goes away and in the
//  > quals graph that I get out during debugging, the two nodes have merged
//  > into one for purposes of the edges in the graph as well.  This is
//  > confusing to debug, because often the way I do it is to put a breakpoint
//  > in the global id factory and condition it upon the id of the node to see
//  > where it is being constructed.  So, for debugging, I just turned the
//  > unify into a bi-directional edge, which as far as I understand it is
//  > equivalent for purposes of the conclusions made, but keeps both nodes
//  > around so I can see both of their ids.  Is there any reason to not just
//  > keep it this way, rather than using the real unify?
//
//  The unification may be required in order to make certain theoretical
//  claims about the running time of the algorithm, but I doubt it has a
//  significant impact on the actual performance.
//
//  As for correctness, it should work either way.
//
//  Best,
//  Rob

  unify_qual_CQUAL(loc,
                   src, tgt,
                   edgeNumber);
}
