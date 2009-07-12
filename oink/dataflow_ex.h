// see License.txt for copyright and terms of use

// Insert a single expression-level edge by driving a call with the
// appropriate arguments on the underlying DataFlowTy.

#ifndef DATAFLOW_EX_H
#define DATAFLOW_EX_H

#include "dataflow_ty.h"        // DataFlowTy, DataFlowKind

class DataFlowEx {
  protected:
  DataFlowTy *dft;

  public:
  explicit DataFlowEx(DataFlowTy *dft0) : dft(dft0) {}
  explicit DataFlowEx() : dft(NULL) {}
  virtual ~DataFlowEx() {}

  // all insertions end up here
  virtual void eDataFlow
    (Value *src, Value *tgt, SourceLoc loc,
     DataFlowKind dfk, bool mayUnrefSrc, bool cast,
     // FIX: 1) get rid of these defaults; 2)
     // qual_dataflow_ty.h:mkLeq() has the same defaults and is called
     // later; it is like there are two layers of the same defaults
     bool useMI = false,
     LibQual::polarity miPol = LibQual::p_non,
     int fcId = -1);

  // convenience methods that just delegate to eDataFlow() ****

  // normal
  virtual void eDataFlow_normal    (Value *src, Value *tgt, SourceLoc loc);
  virtual void eDataFlow_cast      (Value *src, Value *tgt, SourceLoc loc);

  // unification
  private:                      // I think uses of this externally might all be bugs
  virtual void eDataFlow_unify     (Value *src, Value *tgt, SourceLoc loc);
  public:
  virtual void eDataFlow_refUnify  (Value *src, Value *tgt, SourceLoc loc);

  // crossing the lvalue/rvalue boundary
  virtual void eDataFlow_deref     (Value *src, Value *tgt, SourceLoc loc);
  virtual void eDataFlow_addrof    (Value *src, Value *tgt, SourceLoc loc);

  // Insert the edges for a function call
  virtual void eDataFlow_call0
    (SourceLoc loc,             // location of the function call

     // exactly one of these should be relevant, depending on if it is
     // a function that returns an object by value (the first case) or
     // a normal function that returns something that fits in a
     // register (traditional C function call return, the second case)
     Expression *retObj,        // if ret by value, the caller's stack obj for the return value
     Value *callExprValue,      // the E_funCall expression

     Value *thisArgValue,       // 'this' argument for method calls
     FakeList<ArgExpression> *args, // the arguments for the function call
     FunctionValue *fv          // function value being called
     );

  protected:
  virtual Value *makeEllipsisMirrorParam
    (SourceLoc loc, Value *argValue, Value *ellipsisParamValue, FunctionValue *ft);

  virtual void eDataFlow_argToParam(Value *src, Value *tgt, SourceLoc loc,
                                   DataFlowKind dfk, bool mayUnrefSrc, int fcId);
};

#endif // DATAFLOW_EX_H
