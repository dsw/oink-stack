// see License.txt for copyright and terms of use

// Extensions and modifications by oink to the typechecker; for
// processing the nodes added by oink.ast.

#include "cc_ast.h"             // C++ AST
#include "cc_env.h"             // Env

// FIX: record the DeclFlags?
Type *TS_typeVariable::itcheck(Env &env, DeclFlags d, LookupFlags f) {
  return env.makeType(new TypeVariable(name));
}

void S_assert_type::itcheck(Env &env) {
  e->tcheck(env);
  ASTTypeId::Tcheck tc(DF_NONE, DC_S_ASSERT_TYPE);
  tid = tid->tcheck(env, tc);
}

void S_change_type::itcheck(Env &env) {
  e->tcheck(env);
  ASTTypeId::Tcheck tc(DF_NONE, DC_S_CHANGE_TYPE);
  tid = tid->tcheck(env, tc);
}
