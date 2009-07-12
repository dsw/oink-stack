// see License.txt for copyright and terms of use

// Given two top-level Value trees between which data should flow,
// makes the dataflow calls between individual [Abstract] Value nodes.

#ifndef DATAFLOW_TY_H
#define DATAFLOW_TY_H

#include "cc_type.h"
#include "value.h"
#include "oink_var.h"
#include "qual_libqual_iface0.h" // LibQual::polarity

class DataFlowEx;

enum DataFlowKind {
  UNDEF_DataFlowKind = -1,        // undefined DataFlowKind
  LE_DataFlowKind = 1,            // make less than or equal
  EQ_DataFlowKind,                // make equal (things like const subtyping can intervene)
  UN_DataFlowKind,                // unify
};

bool isLegalDataFlowKind(DataFlowKind dfk);


// All dataflow edges at the expression granularity are inserted by
// calls on this class.
class DataFlowTy {
  public:

  DataFlowEx *dfe;              // FIX: get rid of this

  // arguments that are stable and need not be passed individually
  struct StableArgs {
    SourceLoc const loc;        // source line causing this edge
    bool const cast;            // is this insertion due to a cast?
    // for the polymorphic backend
    bool const useMI;           // Use mkinst for polymorphic edges instead of
                                // the standard edges (I think)
    LibQual::polarity const miPol;// polarity of the mkinst edge
    int const fcId;             // id of the function call site

    explicit StableArgs(SourceLoc loc0, bool cast0,
                        bool useMI0, LibQual::polarity miPol0, int fcId0)
      : loc(loc0), cast(cast0)
      , useMI(useMI0), miPol(miPol0), fcId(fcId0)
    {}
  };

  explicit DataFlowTy()
    : dfe(NULL)
  {}
  virtual ~DataFlowTy() {}

  // this is the starting method; note: some internal recursive calls
  // also call this
  virtual void tDataFlow
    (Value *src, Value *tgt, SourceLoc loc,
     DataFlowKind dfk, bool mayUnrefSrc, bool cast,
     bool useMI, LibQual::polarity miPol, int fcId);

  // this is the outgoing method; all external calls from this class
  // leave from here
  virtual void tDataFlow_one
    (StableArgs &stable, Value *src, Value* tgt, DataFlowKind dfk)
    = 0;

  protected:

  // Dispatch to the more specific methods below; there is no
  // double-dispatch in C++
  virtual void tDataFlow_dispatch
    (StableArgs &stable, Value *src, Value *tgt,
     DataFlowKind dfk, bool mayUnrefSrc);
  // Core double-recurse on our children; it is too much trouble to
  // try to get this to work with built-in virtual dispatch since Value
  // then has to know about EdgeInserter.
  virtual void tDataFlow_doubleRecurse
    (StableArgs &stable, Value *src, Value *tgt,
     DataFlowKind dfk);

  // do complete unification; FIX: this is redundant with
  // DataFlowEx::eDataFlow_refUnify()
  virtual void tDataFlow_refUnify(Value *src, Value *tgt, SourceLoc loc);

  // implementation of instance-sensitivity
  virtual void tDataFlow_CVAtomicUnifyFields
    (StableArgs &stable, CVAtomicValue *src, CVAtomicValue *tgt);

  // for when the double-dispatch has been fixed in one dimension by
  // tDataFlow_dispatch above
  virtual void tDataFlow_CVAtomicTgt
    (StableArgs &stable, Value *src, CVAtomicValue *tgt, DataFlowKind dfk);
  virtual void tDataFlow_FuncTgt
    (StableArgs &stable, Value *src, FunctionValue *tgt, DataFlowKind dfk);
  virtual void tDataFlow_PtrOrRefTgt
    (StableArgs &stable, Value *src, Value *tgt, DataFlowKind dfk);
  virtual void tDataFlow_ArrayTgt
    (StableArgs &stable, Value *src, ArrayValue *tgt, DataFlowKind dfk);
  virtual void tDataFlow_PtrToMemTgt
    (StableArgs &stable, Value *src, PointerToMemberValue *tgt, DataFlowKind dfk);

  // utility for function to function flow
  friend void unifyEllipsisPtr0(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);
  friend void unifyEllipsisPtr(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);
  friend void unifyRetVar(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);
  virtual void tDataFlow_FuncTgtFuncSrc
    (StableArgs &stable, Value *src, Value *tgt, bool cast, SourceLoc loc);
};

void unifyEllipsisPtr0(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);
void unifyEllipsisPtr(DataFlowTy &dft, FunctionValue *a, FunctionValue *b);

#endif // DATAFLOW_TY_H
