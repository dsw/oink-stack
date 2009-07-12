// stubs.cc            see license.txt for copyright and terms of use
// stub implementations of things that are part of my prototype verifier,
// but I don't want to include in the parser generator release

#include "c.ast.gen.h"      // C AST declarations

AbsValue *IN_compound::vcgen(AEnv &, Type const *, int) const { return NULL; }
AbsValue *IN_expr::vcgen(AEnv &, Type const *, int) const { return NULL; }

AbsValue *E_quantifier::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_assign::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_new::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_sizeofType::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_comma::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_cond::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_cast::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_deref::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_addrOf::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_binary::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_effect::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_unary::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_sizeof::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_fieldAcc::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_funCall::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_variable::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_charLit::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_stringLit::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_floatLit::vcgen(AEnv &, int) const { return NULL; }
AbsValue *E_intLit::vcgen(AEnv &, int) const { return NULL; }

void S_thmprv::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_invariant::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_assume::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_assert::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_decl::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_goto::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_return::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_continue::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_break::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_for::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_doWhile::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_while::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_switch::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_if::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_compound::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_expr::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_default::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_caseRange::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_case::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_label::vcgen(AEnv &, bool, int, Statement const *) const {}
void S_skip::vcgen(AEnv &, bool, int, Statement const *) const {}

void TF_func::vcgen(AEnv &) const {}
void TF_decl::vcgen(AEnv &) const {}

Predicate *E_quantifier::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_assign::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_new::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_sizeofType::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_comma::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_cond::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_cast::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_deref::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_addrOf::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_binary::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_effect::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_unary::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_sizeof::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_fieldAcc::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_funCall::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_variable::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_charLit::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_stringLit::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_floatLit::vcgenPred(AEnv &, int) const { return NULL; }
Predicate *E_intLit::vcgenPred(AEnv &, int) const { return NULL; }

void S_thmprv::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_invariant::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_assume::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_assert::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_decl::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_goto::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_return::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_continue::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_break::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_for::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_doWhile::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_while::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_switch::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_if::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_compound::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_expr::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_default::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_caseRange::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_case::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_label::factFlow(SObjList<Expression> &, bool, void *) const {}
void S_skip::factFlow(SObjList<Expression> &, bool, void *) const {}
