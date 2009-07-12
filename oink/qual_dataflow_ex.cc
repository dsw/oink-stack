// see License.txt for copyright and terms of use

#include "qual_dataflow_ex.h"   // this module
#include "qual_global.h"        // qualCmd
#include "qual_annot.h"         // qa()
#include "oink_util.h"

// manufacture a fake parameter to assign to; FIX: factor this out
// into the value factory
Value *DataFlowEx_Qual::makeEllipsisMirrorParam
  (SourceLoc loc, Value *argValue, Value *ellipsisParamValue, FunctionValue *ft)
{
  // delegate to superclass
  Value *ellipsis_param = DataFlowEx::makeEllipsisMirrorParam
    (loc, argValue, ellipsisParamValue, ft);

  if (qualCmd->name_vars) {
    stringBuilder name = "ellipsis mirror of ";
    name << argValue->t();
    qa(ellipsis_param)->nameTree(name);
  }

  // attach all the attributes of the ellipsis to it and all its
  // subvalues down to function or atomic.  FIX: not sure why I
  // need this asRval() here; seems unnatural
  Value *tgt0 = ellipsis_param->asRval();
  xassert(tgt0);
  while(tgt0) {
    qa(ellipsisParamValue)->attachLiteralsTo(qa(tgt0));
    // including funky/polymorphic qualifiers
    qa(ft)->ensureFunkyInternalEllipsisEdges(loc, tgt0, *this);
    switch(tgt0->getTag()) {
    default: xfailure("illegal tag"); break;
    case Value::V_ATOMIC: case Value::V_FUNCTION:
      tgt0 = NULL;
      break;
    case Value::V_POINTER: case Value::V_REFERENCE:
    case Value::V_ARRAY: case Value::V_POINTERTOMEMBER:
      tgt0 = tgt0->getAtValue();
      break;
    }
  }

  return ellipsis_param;
}

void DataFlowEx_Qual::eDataFlow_call0
  (SourceLoc loc, Expression *retObj, Value *callExprValue,
   Value *thisArgValue, FakeList<ArgExpression> *args, FunctionValue *fv)
{
  // delegate to superclass
  DataFlowEx::eDataFlow_call0(loc, retObj, callExprValue, thisArgValue, args, fv);
}
