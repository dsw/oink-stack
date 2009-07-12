// exprvisit.cc            see license.txt for copyright and terms of use
// code for exprvisit.h

#include "exprvisit.h"       // this module

#include "c.ast.gen.h"       // C AST definitions

void walkExpression(ExpressionVisitor &vis, Expression const *root)
{     
  // visit the current node
  vis.visitExpr(root);

  ASTSWITCHC(Expression, root) {
    ASTCASEC(E_funCall, e)
      walkExpression(vis, e->func);
      FOREACH_ASTLIST(Expression, e->args, iter) {
        walkExpression(vis, iter.data());
      }

    ASTNEXTC(E_fieldAcc, e)
      walkExpression(vis, e->obj);

    ASTNEXTC(E_sizeof, e)     
      // this is potentially bad since, e.g. if I'm searching for
      // modifications to variables, it doesn't hurt inside sizeof..
      // need walk cancellation semantics, but whatever
      walkExpression(vis, e->expr);

    ASTNEXTC(E_unary, e)
      walkExpression(vis, e->expr);

    ASTNEXTC(E_effect, e)     
      walkExpression(vis, e->expr);

    ASTNEXTC(E_binary, e)
      walkExpression(vis, e->e1);
      walkExpression(vis, e->e2);

    ASTNEXTC(E_addrOf, e)
      walkExpression(vis, e->expr);

    ASTNEXTC(E_deref, e)
      walkExpression(vis, e->ptr);

    ASTNEXTC(E_cast, e)
      walkExpression(vis, e->expr);

    ASTNEXTC(E_cond, e)
      walkExpression(vis, e->cond);
      walkExpression(vis, e->th);
      walkExpression(vis, e->el);

    ASTNEXTC(E_comma, e)
      walkExpression(vis, e->e1);
      walkExpression(vis, e->e2);

    ASTNEXTC(E_assign, e)
      walkExpression(vis, e->target);
      walkExpression(vis, e->src);

    ASTNEXTC(E_quantifier, e)
      walkExpression(vis, e->pred);

    ASTENDCASECD
  }
}

