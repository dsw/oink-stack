#ifndef SQUASH_EXPR_VISITOR
#define SQUASH_EXPR_VISITOR
// See License.txt for copyright and terms of use

// Visits all of the expression subtypes

//#include "oink.gr.gen.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

class MyLoweredASTVisitor : public LoweredASTVisitor {
public:
  MyLoweredASTVisitor(ASTVisitor *client0) : LoweredASTVisitor(client0) {
  }
  void setEnsureSingleVisit(bool value) {
    ensureOneVisit = value;
  }
};

class ExpressionVisitor : public ASTVisitor {
public:
  MyLoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  ExpressionVisitor():loweredVisitor(this) {}


  virtual bool visitStatement(Statement *s);
  virtual bool visitExpression(Expression *e);
 
  virtual bool visitS_skip(S_skip *s) {return true;}
  virtual bool visitS_label(S_label *s) {return true;}
  virtual bool visitS_case(S_case *s) {return true;}
  virtual bool visitS_default(S_default *s) {return true;}
  virtual bool visitS_expr(S_expr *s) {return true;}
  virtual bool visitS_compound(S_compound *s) {return true;}
  virtual bool visitS_if(S_if *s) {return true;}
  virtual bool visitS_switch(S_switch *s) {return true;}
  virtual bool visitS_while(S_while *s) {return true;}
  virtual bool visitS_doWhile(S_doWhile *s) {return true;}
  virtual bool visitS_for(S_for *s) {return true;}
  virtual bool visitS_break(S_break *s) {return true;}
  virtual bool visitS_continue(S_continue *s) {return true;}
  virtual bool visitS_return(S_return *s) {return true;}
  virtual bool visitS_goto(S_goto *s) {return true;}
  virtual bool visitS_decl(S_decl *s) {return true;}
  virtual bool visitS_try(S_try *s) {return true;}
  virtual bool visitS_asm(S_asm *s) {return true;}
  virtual bool visitS_namespaceDecl(S_namespaceDecl *s) {return true;}
  virtual bool visitS_function(S_function *s) {return true;}
  virtual bool visitS_rangeCase(S_rangeCase *s) {return true;}
  virtual bool visitS_computedGoto(S_computedGoto *s) {return true;}

  virtual bool visitE_boolLit(E_boolLit *e) {return true;}
  virtual bool visitE_intLit(E_intLit *e) {return true;}
  virtual bool visitE_floatLit(E_floatLit *e) {return true;}
  virtual bool visitE_stringLit(E_stringLit *e) {return true;}
  virtual bool visitE_charLit(E_charLit *e) {return true;}
  virtual bool visitE_this(E_this *e) {return true;}
  virtual bool visitE_variable(E_variable *e) {return true;}
  virtual bool visitE_funCall(E_funCall *e) {return true;}
  virtual bool visitE_constructor(E_constructor *e) {return true;}
  virtual bool visitE_fieldAcc(E_fieldAcc *e) {return true;}
  virtual bool visitE_sizeof(E_sizeof *e) {return true;}
  virtual bool visitE_unary(E_unary *e) {return true;}
  virtual bool visitE_effect(E_effect *e) {return true;}
  virtual bool visitE_binary(E_binary *e) {return true;}
  virtual bool visitE_addrOf(E_addrOf *e) {return true;}
  virtual bool visitE_deref(E_deref *e) {return true;}
  virtual bool visitE_cast(E_cast *e) {return true;}
  virtual bool visitE_cond(E_cond *e) {return true;}
  virtual bool visitE_sizeofType(E_sizeofType *e) {return true;}
  virtual bool visitE_assign(E_assign *e) {return true;}
  virtual bool visitE_new(E_new *e) {return true;}
  virtual bool visitE_delete(E_delete *e) {return true;}
  virtual bool visitE_throw(E_throw *e) {return true;}
  virtual bool visitE_keywordCast(E_keywordCast *e) {return true;}
  virtual bool visitE_typeidExpr(E_typeidExpr *e) {return true;}
  virtual bool visitE_typeidType(E_typeidType *e) {return true;}
  virtual bool visitE_grouping(E_grouping *e) {return true;}
  virtual bool visitE_arrow(E_arrow *e) {return true;}
  virtual bool visitE_statement(E_statement *e) {return true;}
  virtual bool visitE_compoundLit(E_compoundLit *e) {return true;}
  virtual bool visitE___builtin_constant_p(E___builtin_constant_p *e) {
    return true;
  }
  virtual bool visitE___builtin_va_arg(E___builtin_va_arg *e) {return true;}
  virtual bool visitE_alignofType(E_alignofType *e) {return true;}
  virtual bool visitE_alignofExpr(E_alignofExpr *e) {return true;}
  virtual bool visitE_gnuCond(E_gnuCond *e) {return true;}
  virtual bool visitE_addrOfLabel(E_addrOfLabel *e) {return true;}



  virtual void postvisitStatement(Statement *s);
  virtual void postvisitExpression(Expression *e);
 
  virtual void postvisitS_skip(S_skip *s) {}
  virtual void postvisitS_label(S_label *s) {}
  virtual void postvisitS_case(S_case *s) {}
  virtual void postvisitS_default(S_default *s) {}
  virtual void postvisitS_expr(S_expr *s) {}
  virtual void postvisitS_compound(S_compound *s) {}
  virtual void postvisitS_if(S_if *s) {}
  virtual void postvisitS_switch(S_switch *s) {}
  virtual void postvisitS_while(S_while *s) {}
  virtual void postvisitS_doWhile(S_doWhile *s) {}
  virtual void postvisitS_for(S_for *s) {}
  virtual void postvisitS_break(S_break *s) {}
  virtual void postvisitS_continue(S_continue *s) {}
  virtual void postvisitS_return(S_return *s) {}
  virtual void postvisitS_goto(S_goto *s) {}
  virtual void postvisitS_decl(S_decl *s) {}
  virtual void postvisitS_try(S_try *s) {}
  virtual void postvisitS_asm(S_asm *s) {}
  virtual void postvisitS_namespaceDecl(S_namespaceDecl *s) {}
  virtual void postvisitS_function(S_function *s) {}
  virtual void postvisitS_rangeCase(S_rangeCase *s) {}
  virtual void postvisitS_computedGoto(S_computedGoto *s) {}

  virtual void postvisitE_boolLit(E_boolLit *e) {}
  virtual void postvisitE_intLit(E_intLit *e) {}
  virtual void postvisitE_floatLit(E_floatLit *e) {}
  virtual void postvisitE_stringLit(E_stringLit *e) {}
  virtual void postvisitE_charLit(E_charLit *e) {}
  virtual void postvisitE_this(E_this *e) {}
  virtual void postvisitE_variable(E_variable *e) {}
  virtual void postvisitE_funCall(E_funCall *e) {}
  virtual void postvisitE_constructor(E_constructor *e) {}
  virtual void postvisitE_fieldAcc(E_fieldAcc *e) {}
  virtual void postvisitE_sizeof(E_sizeof *e) {}
  virtual void postvisitE_unary(E_unary *e) {}
  virtual void postvisitE_effect(E_effect *e) {}
  virtual void postvisitE_binary(E_binary *e) {}
  virtual void postvisitE_addrOf(E_addrOf *e) {}
  virtual void postvisitE_deref(E_deref *e) {}
  virtual void postvisitE_cast(E_cast *e) {}
  virtual void postvisitE_cond(E_cond *e) {}
  virtual void postvisitE_sizeofType(E_sizeofType *e) {}
  virtual void postvisitE_assign(E_assign *e) {}
  virtual void postvisitE_new(E_new *e) {}
  virtual void postvisitE_delete(E_delete *e) {}
  virtual void postvisitE_throw(E_throw *e) {}
  virtual void postvisitE_keywordCast(E_keywordCast *e) {}
  virtual void postvisitE_typeidExpr(E_typeidExpr *e) {}
  virtual void postvisitE_typeidType(E_typeidType *e) {}
  virtual void postvisitE_grouping(E_grouping *e) {}
  virtual void postvisitE_arrow(E_arrow *e) {}
  virtual void postvisitE_statement(E_statement *e) {}
  virtual void postvisitE_compoundLit(E_compoundLit *e) {}
  virtual void postvisitE___builtin_constant_p(E___builtin_constant_p *e) {}
  virtual void postvisitE___builtin_va_arg(E___builtin_va_arg *e) {}
  virtual void postvisitE_alignofType(E_alignofType *e) {}
  virtual void postvisitE_alignofExpr(E_alignofExpr *e) {}
  virtual void postvisitE_gnuCond(E_gnuCond *e) {}
  virtual void postvisitE_addrOfLabel(E_addrOfLabel *e) {}
};

#endif // SQUASH_EXPR_VISITOR
