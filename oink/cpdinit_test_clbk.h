// see License.txt for copyright and terms of use

// for use with cpdinit_test; prints out assignments rather than
// adding edges to an inference graph; imitates the (filtered) output
// of CIL

#ifndef CPDINIT_TEST_CLBK_H
#define CPDINIT_TEST_CLBK_H

#include "LibCpdInit/cpdinit.h"

void oneAssignmentCpdTest(void *context, SourceLoc loc, MemberValueIter &type_iter,
                          IN_expr *src_expr, Value *tgtContainer, Variable *tgtContentVar);
bool reportUserErrorCpdTest(void *context, SourceLoc loc, MemberValueIter &type_iter,
                            Initializer *init, UserError &e);

#endif // CPDINIT_TEST_CLBK_H
