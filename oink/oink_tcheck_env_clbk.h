// see License.txt for copyright and terms of use

#ifndef OINK_TCHECK_ENV_CLBK_H
#define OINK_TCHECK_ENV_CLBK_H

#include "LibCpdInit/cpdinit.h"

void oneAssignmentTcheckEnv(void *context, SourceLoc loc, MemberValueIter &type_iter,
                            IN_expr *src_expr,
                            Value *tgtValue, Variable *tgtContentVar);
bool reportUserErrorTcheckEnv(void *context, SourceLoc loc, MemberValueIter &type_iter,
                              Initializer *init, UserError &e);

#endif // OINK_TCHECK_ENV_CLBK_H
