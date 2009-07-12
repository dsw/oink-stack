// see License.txt for copyright and terms of use

#include "cc_type.h"
#include "cc_lang.h"
#include "oink_tcheck_env.h"
#include "oink_tcheck_env_clbk.h"
#include "oink_type_children.h"
#include "oink_global.h"

TcheckEnv::TcheckEnv(StringTable &str0, CCLang &lang0, TypeFactory &tfac0,
                     ArrayStack<Variable*> &madeUpVariables0,
                     ArrayStack<Variable*> &builtinVars0,
                     TranslationUnit *unit0)
  : Env (str0, lang0, tfac0, madeUpVariables0, builtinVars0, unit0)
{}

TcheckEnv::~TcheckEnv() {
  // remove the global scope from the scopes list so that it is not
  // destroyed when the parent dtor runs
  int count = scopes.count();
  xassert(count>0);
  Scope *gs = scopes.removeAt(count-1); // remove the last element
  xassert(gs);
  xassert(gs->isGlobalScope());
}

int TcheckEnv::countInitializers(SourceLoc loc, Type *type, IN_compound const *cpd) {
  Variable_O *tmpVar =
    asVariable_O(tFac->makeVariable
                 (loc, "<TcheckEnv::countInitializers tempVar>",
                  type,         // this argument is the whole point
                  DF_NONE));
  tmpVar->setReal(true);
  tmpVar->setFilteredKeep(true);
  // NOTE: there is no need to set the abstValue on tmpVar because
  // we aren't doing any real dataflow to it anyway
  Restorer<bool> restorer(value2typeIsOn, true);
  Restorer<bool> restorer2(omitCheckAbstrValueOnlyOnRealVars, true);
  int max_cursor = 0;
  try {
    max_cursor = compoundInit(NULL, loc, tmpVar, const_cast<IN_compound *>(cpd),
                              oneAssignmentTcheckEnv, reportUserErrorTcheckEnv);
  } catch (UserError &err) {
    // dsw: there are some compound initializers situations where gcc
    // just gives a warning instead of an error, such as if you put
    // too many elements into an array:
    // oink/Test/too_many_initializers1.c; here I turn the error into
    // a warning so we can at least process such files
    userReportWarning
      (loc, "Unsoundness warning: above error turned into a warning");
    // return Scott's approximation from the superclass method
    return Env::countInitializers(loc, type, cpd);
  }
  if (tmpVar) {
    // FIX: should attempt to delete the stuff hanging off of tmpVar
    // also
    delete tmpVar;
  }
  return max_cursor + 1;
}
