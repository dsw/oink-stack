// see License.txt for copyright and terms of use

#include "dataflow_cpdinit_clbk.h" // this module
#include "dataflow_visitor.h"   // DataFlowVisitor
#include "oink_global.h"        // oinkCmd

void oneAssignmentDataFlow(void *context, SourceLoc loc, MemberValueIter &,
                           IN_expr *src_init, Value *tgtContainer, Variable *tgtContentVar) {
  xassert(context);
  DataFlowEx *dfe = static_cast<DataFlowEx*>(context);
  // FIX: GATED_EDGE: don't put an edge from a string literal due to
  // const inference problems; when we get gated edges, put the edge
  // back and put one here.
  if (!src_init->e->isE_stringLit()) {
    Value *instSpecField = asVariable_O(tgtContentVar)->abstrValue();
    if (oinkCmd->instance_sensitive && tgtContainer && tgtContainer->asRval()->isCVAtomicValue()) {
      CVAtomicValue *cvat = tgtContainer->asRval()->asCVAtomicValue();
      instSpecField = cvat->getInstanceSpecificValue(tgtContentVar->name, instSpecField);
    }
    dfe->eDataFlow_normal(src_init->getE()->abstrValue, instSpecField, loc);
  }
}

bool reportUserErrorDataFlow(void *context, SourceLoc loc, MemberValueIter &,
                         Initializer *init, UserError &e) {
  return true;
}
