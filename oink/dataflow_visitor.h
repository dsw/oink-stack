// see License.txt for copyright and terms of use

// An AST Visitor that computes a instance-sensitive, polymorphic,
// non-flow-sensitive, non-path-sensitive, expression granularity
// dataflow graph on a Translation Unit AST.  When a pair of
// expressions is found between which data flows, the pair is handed
// off to the dataflow_ex module.

#ifndef DATAFLOW_VISITOR_H
#define DATAFLOW_VISITOR_H

#include "cc_ast.h"
#include "cc_lang.h"            // CCLang
#include "oink_var.h"
#include "dataflow_ex.h"        // DataFlowEx
#include "cc_ast_aux.h"         // LoweredASTVisitor


class DataFlowVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  TranslationUnit *tunit;
  DataFlowEx &dfe;
  CCLang &lang;

  protected:
  // The stack of nested functions
  SObjStack<Function> functionStack;

  // tor ****
  public:
  DataFlowVisitor(TranslationUnit *tunit0,
                  DataFlowEx &dfe0,
                  CCLang &lang0)
    : loweredVisitor(this)
    , tunit(tunit0)
    , dfe(dfe0)
    , lang(lang0)
  {}
  virtual ~DataFlowVisitor(){}

  // methods ****

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  Variable_O *findGlobalDecl(string const &name, SourceLoc loc);

  // manually called visit methods ****
  virtual bool visitFunction_once_initial(Function *);
  virtual bool visitFunction_once_decl(Function *);
  virtual bool visitFunction_once_body(Function *);

  virtual bool subVisitTF_asm(TF_asm *);

  virtual bool subVisitS_asm(S_asm *);
  virtual bool subVisitS_return(S_return *);
  virtual bool subVisitS_change_type(S_change_type *);

  virtual bool subVisitE_compoundLit(E_compoundLit *);
  virtual bool subVisitE_this(E_this *);
  virtual bool subVisitE_variable(E_variable *);
  virtual bool subVisitE_funCall(E_funCall *);
  virtual bool subVisitE_constructor(E_constructor *);
  virtual bool subVisitE_fieldAcc(E_fieldAcc *);
  virtual bool subVisitE_grouping(E_grouping *);
  virtual bool subVisitE_arrow(E_arrow *);
  virtual bool subVisitE_unary(E_unary *);
  virtual bool subVisitE_effect(E_effect *);
  virtual bool subVisitE_binary(E_binary *);
  virtual bool subVisitE_addrOf(E_addrOf *);
  virtual bool subVisitE_deref(E_deref *);
  virtual bool subVisitE_cast(E_cast *);
  virtual bool subVisitE_cond(E_cond *);
  virtual bool subVisitE_gnuCond(E_gnuCond *);
  virtual bool subVisitE_assign(E_assign *);
  virtual bool subVisitE_new(E_new *);
  virtual bool subVisitE_throw(E_throw *);
  virtual bool subVisitE_keywordCast(E_keywordCast *);
  virtual bool subVisitE_statement(E_statement *);

  // varargs; the assymetry in the names reflects the fact that
  // E___builtin_va_arg is a real AST node whereas the others are
  // parsed and represened as E_funCall.
  virtual bool subVisit__builtin_va_start(Expression *vaIterExpr, Expression *preEllipsisParam);
  virtual bool subVisit__builtin_va_copy(Expression *tgtVaIterExpr, Expression *srcVaIterExpr);
  virtual bool subVisitE___builtin_va_arg(E___builtin_va_arg *);
  virtual bool subVisit__builtin_va_end();

  // methods called by traverse()
  virtual void postvisitExpression(Expression *);

  virtual bool visitTopForm(TopForm *);
  virtual bool visitMemberInit(MemberInit *);
  virtual bool visitStatement(Statement *);
  virtual bool visitHandler(Handler *);
  virtual bool visitTemplateArgument(TemplateArgument *);
  virtual bool visitDeclarator(Declarator *);
  virtual bool visitFunction(Function *);
#ifdef GNU_EXTENSION
  virtual bool visitASTTypeof(ASTTypeof *obj);
#endif // GNU_EXTENSION
};

// An AST visitor that finds all of the top-level Values in the AST.
class FindValueASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  ValueVisitor &valueVisitor;   // a visitor for values

  // ctor ****
  public:
  FindValueASTVisitor(ValueVisitor &valueVisitor0)
    : loweredVisitor(this)
    , valueVisitor(valueVisitor0)
  {}
  virtual ~FindValueASTVisitor() {}

  virtual bool visitExpression(Expression *);
  virtual bool visitFunction(Function *);
  virtual bool visitDeclarator(Declarator *);
};

// Put in various kinds of dataflow edges for structures.
class StructuralFlow_ValueVisitor : public ValueVisitor {
  public:
  DataFlowEx &dfe;              // for adding dataflow edges
  bool compoundUp;              // insert var to container edges
  bool compoundDown;            // insert container to var edges
  bool pointUp;                 // insert value to pointer/array edges
  bool pointDown;               // insert pointer/array to value edges
  bool refUp;                   // insert value to ref edges
  bool refDown;                 // insert ref to value edges

  private:
  SObjSet<Value*> visitedValue; // set for idempotent visiting

  public:
  StructuralFlow_ValueVisitor(DataFlowEx &dfe0)
    : dfe(dfe0)
    , compoundUp   (false)
    , compoundDown (false)
    , pointUp      (false)
    , pointDown    (false)
    , refUp        (false)
    , refDown      (false)
  {}
  virtual ~StructuralFlow_ValueVisitor() {}

  virtual bool preVisitValue(Value *obj);
};

// An AST visitor that finds all of the declaration/definition
// variables of FunctionType in the AST.
class FuncDeclVar_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  ValueVisitor &valueVisitor;   // a visitor for values

  // ctor ****
  public:
  FuncDeclVar_ASTVisitor(ValueVisitor &valueVisitor0)
    : loweredVisitor(this)
    , valueVisitor(valueVisitor0)
  {}
  virtual ~FuncDeclVar_ASTVisitor() {}

  virtual bool visitDeclarator(Declarator *);
};

#endif // DATAFLOW_VISITOR_H
