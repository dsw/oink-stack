#include "expr_visitor.h"

bool ExpressionVisitor::visitStatement(Statement *s) {
  switch(s->kind()){
  default:
    xassert(false);
  case Statement::S_SKIP:
    return visitS_skip(s->asS_skip());
  case Statement::S_LABEL:
    return visitS_label(s->asS_label());
  case Statement::S_CASE:
    return visitS_case(s->asS_case());
  case Statement::S_DEFAULT:
    return visitS_default(s->asS_default());
  case Statement::S_EXPR:
    return visitS_expr(s->asS_expr());
  case Statement::S_COMPOUND:
    return visitS_compound(s->asS_compound());
  case Statement::S_IF:
    return visitS_if(s->asS_if());
  case Statement::S_SWITCH:
    return visitS_switch(s->asS_switch());
  case Statement::S_WHILE:
    return visitS_while(s->asS_while());
  case Statement::S_DOWHILE:
    return visitS_doWhile(s->asS_doWhile());
  case Statement::S_FOR:
    return visitS_for(s->asS_for());
  case Statement::S_BREAK:
    return visitS_break(s->asS_break());
  case Statement::S_CONTINUE:
    return visitS_continue(s->asS_continue());
  case Statement::S_RETURN:
    return visitS_return(s->asS_return());
  case Statement::S_GOTO:
    return visitS_goto(s->asS_goto());
  case Statement::S_DECL:
    return visitS_decl(s->asS_decl());
  case Statement::S_TRY:
    return visitS_try(s->asS_try());
  case Statement::S_ASM:
    return visitS_asm(s->asS_asm());
  case Statement::S_NAMESPACEDECL:
    return visitS_namespaceDecl(s->asS_namespaceDecl());
  case Statement::S_FUNCTION:
    return visitS_function(s->asS_function());
  case Statement::S_RANGECASE:
    return visitS_rangeCase(s->asS_rangeCase());
  case Statement::S_COMPUTEDGOTO:
    return visitS_computedGoto(s->asS_computedGoto());
  }
}

bool ExpressionVisitor::visitExpression(Expression *e) {
  switch(e->kind()){
  default:
    xassert(false);
  case Expression::E_BOOLLIT:
    return visitE_boolLit(e->asE_boolLit());
  case Expression::E_INTLIT:
    return visitE_intLit(e->asE_intLit());
  case Expression::E_FLOATLIT:
    return visitE_floatLit(e->asE_floatLit());
  case Expression::E_STRINGLIT:
    return visitE_stringLit(e->asE_stringLit());
  case Expression::E_CHARLIT:
    return visitE_charLit(e->asE_charLit());
  case Expression::E_THIS:
    return visitE_this(e->asE_this());
  case Expression::E_VARIABLE:
    return visitE_variable(e->asE_variable());
  case Expression::E_FUNCALL:
    return visitE_funCall(e->asE_funCall());
  case Expression::E_CONSTRUCTOR:
    return visitE_constructor(e->asE_constructor());
  case Expression::E_FIELDACC:
    return visitE_fieldAcc(e->asE_fieldAcc());
  case Expression::E_SIZEOF:
    return visitE_sizeof(e->asE_sizeof());
  case Expression::E_UNARY:
    return visitE_unary(e->asE_unary());
  case Expression::E_EFFECT:
    return visitE_effect(e->asE_effect());
  case Expression::E_BINARY:
    return visitE_binary(e->asE_binary());
  case Expression::E_ADDROF:
    return visitE_addrOf(e->asE_addrOf());
  case Expression::E_DEREF:
    return visitE_deref(e->asE_deref());
  case Expression::E_CAST:
    return visitE_cast(e->asE_cast());
  case Expression::E_COND:
    return visitE_cond(e->asE_cond());
  case Expression::E_SIZEOFTYPE:
    return visitE_sizeofType(e->asE_sizeofType());
  case Expression::E_ASSIGN:
    return visitE_assign(e->asE_assign());
  case Expression::E_NEW:
    return visitE_new(e->asE_new());
  case Expression::E_DELETE:
    return visitE_delete(e->asE_delete());
  case Expression::E_THROW:
    return visitE_throw(e->asE_throw());
  case Expression::E_KEYWORDCAST:
    return visitE_keywordCast(e->asE_keywordCast());
  case Expression::E_TYPEIDEXPR:
    return visitE_typeidExpr(e->asE_typeidExpr());
  case Expression::E_TYPEIDTYPE:
    return visitE_typeidType(e->asE_typeidType());
  case Expression::E_GROUPING:
    return visitE_grouping(e->asE_grouping());
  case Expression::E_ARROW:
    return visitE_arrow(e->asE_arrow());
  case Expression::E_STATEMENT:
    return visitE_statement(e->asE_statement());
  case Expression::E_COMPOUNDLIT:
    return visitE_compoundLit(e->asE_compoundLit());
  case Expression::E___BUILTIN_CONSTANT_P:
    return visitE___builtin_constant_p(e->asE___builtin_constant_p());
  case Expression::E___BUILTIN_VA_ARG:
    return visitE___builtin_va_arg(e->asE___builtin_va_arg());
  case Expression::E_ALIGNOFTYPE:
    return visitE_alignofType(e->asE_alignofType());
  case Expression::E_ALIGNOFEXPR:
    return visitE_alignofExpr(e->asE_alignofExpr());
  case Expression::E_GNUCOND:
    return visitE_gnuCond(e->asE_gnuCond());
  case Expression::E_ADDROFLABEL:
    return visitE_addrOfLabel(e->asE_addrOfLabel());
  }
}

void ExpressionVisitor::postvisitStatement(Statement *s) {
  switch(s->kind()){
  default:
    xassert(false);
  case Statement::S_SKIP:
    postvisitS_skip(s->asS_skip());
    break;
  case Statement::S_LABEL:
    postvisitS_label(s->asS_label());
    break;
  case Statement::S_CASE:
    postvisitS_case(s->asS_case());
    break;
  case Statement::S_DEFAULT:
    postvisitS_default(s->asS_default());
    break;
  case Statement::S_EXPR:
    postvisitS_expr(s->asS_expr());
    break;
  case Statement::S_COMPOUND:
    postvisitS_compound(s->asS_compound());
    break;
  case Statement::S_IF:
    postvisitS_if(s->asS_if());
    break;
  case Statement::S_SWITCH:
    postvisitS_switch(s->asS_switch());
    break;
  case Statement::S_WHILE:
    postvisitS_while(s->asS_while());
    break;
  case Statement::S_DOWHILE:
    postvisitS_doWhile(s->asS_doWhile());
    break;
  case Statement::S_FOR:
    postvisitS_for(s->asS_for());
    break;
  case Statement::S_BREAK:
    postvisitS_break(s->asS_break());
    break;
  case Statement::S_CONTINUE:
    postvisitS_continue(s->asS_continue());
    break;
  case Statement::S_RETURN:
    postvisitS_return(s->asS_return());
    break;
  case Statement::S_GOTO:
    postvisitS_goto(s->asS_goto());
    break;
  case Statement::S_DECL:
    postvisitS_decl(s->asS_decl());
    break;
  case Statement::S_TRY:
    postvisitS_try(s->asS_try());
    break;
  case Statement::S_ASM:
    postvisitS_asm(s->asS_asm());
    break;
  case Statement::S_NAMESPACEDECL:
    postvisitS_namespaceDecl(s->asS_namespaceDecl());
    break;
  case Statement::S_FUNCTION:
    postvisitS_function(s->asS_function());
    break;
  case Statement::S_RANGECASE:
    postvisitS_rangeCase(s->asS_rangeCase());
    break;
  case Statement::S_COMPUTEDGOTO:
    postvisitS_computedGoto(s->asS_computedGoto());
    break;
  }
}

void ExpressionVisitor::postvisitExpression(Expression *e) {
  switch(e->kind()){
  default:
    xassert(false);
  case Expression::E_BOOLLIT:
    postvisitE_boolLit(e->asE_boolLit());
    break;
  case Expression::E_INTLIT:
    postvisitE_intLit(e->asE_intLit());
    break;
  case Expression::E_FLOATLIT:
    postvisitE_floatLit(e->asE_floatLit());
    break;
  case Expression::E_STRINGLIT:
    postvisitE_stringLit(e->asE_stringLit());
    break;
  case Expression::E_CHARLIT:
    postvisitE_charLit(e->asE_charLit());
    break;
  case Expression::E_THIS:
    postvisitE_this(e->asE_this());
    break;
  case Expression::E_VARIABLE:
    postvisitE_variable(e->asE_variable());
    break;
  case Expression::E_FUNCALL:
    postvisitE_funCall(e->asE_funCall());
    break;
  case Expression::E_CONSTRUCTOR:
    postvisitE_constructor(e->asE_constructor());
    break;
  case Expression::E_FIELDACC:
    postvisitE_fieldAcc(e->asE_fieldAcc());
    break;
  case Expression::E_SIZEOF:
    postvisitE_sizeof(e->asE_sizeof());
    break;
  case Expression::E_UNARY:
    postvisitE_unary(e->asE_unary());
    break;
  case Expression::E_EFFECT:
    postvisitE_effect(e->asE_effect());
    break;
  case Expression::E_BINARY:
    postvisitE_binary(e->asE_binary());
    break;
  case Expression::E_ADDROF:
    postvisitE_addrOf(e->asE_addrOf());
    break;
  case Expression::E_DEREF:
    postvisitE_deref(e->asE_deref());
    break;
  case Expression::E_CAST:
    postvisitE_cast(e->asE_cast());
    break;
  case Expression::E_COND:
    postvisitE_cond(e->asE_cond());
    break;
  case Expression::E_SIZEOFTYPE:
    postvisitE_sizeofType(e->asE_sizeofType());
    break;
  case Expression::E_ASSIGN:
    postvisitE_assign(e->asE_assign());
    break;
  case Expression::E_NEW:
    postvisitE_new(e->asE_new());
    break;
  case Expression::E_DELETE:
    postvisitE_delete(e->asE_delete());
    break;
  case Expression::E_THROW:
    postvisitE_throw(e->asE_throw());
    break;
  case Expression::E_KEYWORDCAST:
    postvisitE_keywordCast(e->asE_keywordCast());
    break;
  case Expression::E_TYPEIDEXPR:
    postvisitE_typeidExpr(e->asE_typeidExpr());
    break;
  case Expression::E_TYPEIDTYPE:
    postvisitE_typeidType(e->asE_typeidType());
    break;
  case Expression::E_GROUPING:
    postvisitE_grouping(e->asE_grouping());
    break;
  case Expression::E_ARROW:
    postvisitE_arrow(e->asE_arrow());
    break;
  case Expression::E_STATEMENT:
    postvisitE_statement(e->asE_statement());
    break;
  case Expression::E_COMPOUNDLIT:
    postvisitE_compoundLit(e->asE_compoundLit());
    break;
  case Expression::E___BUILTIN_CONSTANT_P:
    postvisitE___builtin_constant_p(e->asE___builtin_constant_p());
    break;
  case Expression::E___BUILTIN_VA_ARG:
    postvisitE___builtin_va_arg(e->asE___builtin_va_arg());
    break;
  case Expression::E_ALIGNOFTYPE:
    postvisitE_alignofType(e->asE_alignofType());
    break;
  case Expression::E_ALIGNOFEXPR:
    postvisitE_alignofExpr(e->asE_alignofExpr());
    break;
  case Expression::E_GNUCOND:
    postvisitE_gnuCond(e->asE_gnuCond());
    break;
  case Expression::E_ADDROFLABEL:
    postvisitE_addrOfLabel(e->asE_addrOfLabel());
    break;
  }
}
