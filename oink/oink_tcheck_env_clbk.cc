// see License.txt for copyright and terms of use

// clbks for LibCpdInit for oink_tcheck_env.cc
#include "oink_tcheck_env_clbk.h"

void oneAssignmentTcheckEnv(void *context, SourceLoc loc, MemberValueIter &type_iter,
                            IN_expr *src_expr,
                            Value *tgtValue, Variable *tgtContentVar) {
  // do nothing
}

bool reportUserErrorTcheckEnv(void *context, SourceLoc loc, MemberValueIter &type_iter,
                              Initializer *init, UserError &e) {
  return true;                  // rethrow
}
