// see License.txt for copyright and terms of use

// Qual subclass of dataflow_ex; this class inherits from the generic
// dataflow and modifies its behavior for purposes of the qual
// analysis.

#ifndef QUAL_DATAFLOW_EX_H
#define QUAL_DATAFLOW_EX_H

#include "dataflow_ex.h"        // DataFlowEx

class DataFlowEx_Qual : public DataFlowEx {
  public:
  explicit DataFlowEx_Qual(DataFlowTy *dft0)
    : DataFlowEx(dft0)
  {}
  virtual ~DataFlowEx_Qual() {}

  virtual Value *makeEllipsisMirrorParam
    (SourceLoc loc, Value *argValue, Value *ellipsisParamValue, FunctionValue *ft);

  virtual void eDataFlow_call0
    (SourceLoc loc, Expression *retObj, Value *callExprValue,
     Value *thisArgValue, FakeList<ArgExpression> *args, FunctionValue *ft);
};

#endif // QUAL_DATAFLOW_EX_H
