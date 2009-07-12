// see License.txt for copyright and terms of use

#ifndef OINK_TCHECK_ENV_H
#define OINK_TCHECK_ENV_H

#include "cc_env.h"

class TcheckEnv : public Env {
  public:
  TcheckEnv(StringTable &str0, CCLang &lang0, TypeFactory &tfac0,
            ArrayStack<Variable*> &madeUpVariables0,
            ArrayStack<Variable*> &builtinVars0,
            TranslationUnit *unit0);
  virtual ~TcheckEnv();
  // return # of array elements initialized
  virtual int countInitializers(SourceLoc loc, Type *type, IN_compound const *cpd);
};

#endif // OINK_TCHECK_ENV_H
