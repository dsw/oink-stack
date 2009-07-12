// see License.txt for copyright and terms of use

// Callback into the dataflow analysis that we hand to LibCpdInit when
// we want the dataflow for a compound initializer.

#ifndef DATAFLOW_CPDINIT_CLBK_H
#define DATAFLOW_CPDINIT_CLBK_H

#include "LibCpdInit/cpdinit.h" // MemberValueIter

void oneAssignmentDataFlow(void *context, SourceLoc loc, MemberValueIter &,
                           IN_expr *src_init, Value *tgtContainer, Variable *tgtContentVar);
bool reportUserErrorDataFlow(void *context, SourceLoc loc, MemberValueIter &,
                             Initializer *init, UserError &e);

#endif // DATAFLOW_CPDINIT_CLBK_H
