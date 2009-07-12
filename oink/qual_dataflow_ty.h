// see License.txt for copyright and terms of use

// Qual subclass of dataflow_ty; this class inherits from the generic
// dataflow and modifies its behavior for purposes of the qual
// analysis.

#ifndef QUAL_DATAFLOW_TY_H
#define QUAL_DATAFLOW_TY_H

#include "dataflow_ty.h"        // DataFlowTy
#include "cc_type.h"            // Type
#include "qual_libqual_iface.h" // LibQual::polarity

class DataFlowEx;             // FIX: get rid of this


// insert edges for qual inference
class DataFlowTy_Qual : public DataFlowTy {
  public:
  explicit DataFlowTy_Qual() {}
  virtual ~DataFlowTy_Qual() {}

  virtual void tDataFlow_one(StableArgs &stable, Value *src, Value* tgt,
                             DataFlowKind dfk);
  virtual void tDataFlow_FuncTgt
    (StableArgs &stable, Value *src, FunctionValue *tgt, DataFlowKind dfk);
  virtual void tDataFlow_FuncTgtFuncSrc
    (StableArgs &stable, Value *src, Value *tgt, bool cast, SourceLoc loc);
};

int incEdgeNumber(LibQual::Type_qualifier *src, LibQual::Type_qualifier *tgt, SourceLoc loc);
void mkLeq(LibQual::Type_qualifier *src, LibQual::Type_qualifier *tgt, SourceLoc loc,
           bool isCast = false,
           // FIX: why the defaults?
           bool useMI = false,
           LibQual::polarity miPolCqual = LibQual::p_non,
           int fcId = -1);
void unify(LibQual::Type_qualifier *src, LibQual::Type_qualifier *tgt,
           SourceLoc loc);

#endif // QUAL_DATAFLOW_TY_H
