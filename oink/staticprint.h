// see License.txt for copyright and terms of use

// Static Printer: A sample Oink tool that shows how to query the AST
// and typesystem; for now it just prints the inheritance graph.  If
// there is something you always wanted a tool to tell you about your
// raw program, implement it as feature here and send it to me.

#ifndef STATICPRINT_H
#define STATICPRINT_H

#include "oink.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

class Staticprint : public virtual Oink {
  // tor ****
  public:
  Staticprint() {}
  
  // methods ****
  void printIhg_stage();
  void printASTHistogram_stage();
};

class HistogramASTVisitor : public ASTVisitor {
  public:
//    LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  int num_TranslationUnit;
  int num_TopForm;
  int num_Function;
  int num_MemberInit;
  int num_Declaration;
  int num_ASTTypeId;
  int num_PQName;
  int num_TypeSpecifier;
  int num_BaseClassSpec;
  int num_Enumerator;
  int num_MemberList;
  int num_Member;
  int num_Declarator;
  int num_IDeclarator;
  int num_ExceptionSpec;
  int num_OperatorName;
  int num_Statement;
  int num_Condition;
  int num_Handler;
  int num_Expression;
  int num_FullExpression;
  int num_ArgExpression;
  int num_ArgExpressionListOpt;
  int num_Initializer;
  int num_TemplateDeclaration;
  int num_TemplateParameter;
  int num_TemplateArgument;
  int num_NamespaceDecl;
  int num_FullExpressionAnnot;
  int num_ASTTypeof;
  int num_Designator;
  int num_AttributeSpecifierList;
  int num_AttributeSpecifier;
  int num_Attribute;

  HistogramASTVisitor()
//      : loweredVisitor(this)
    : num_TranslationUnit(0)
    , num_TopForm(0)
    , num_Function(0)
    , num_MemberInit(0)
    , num_Declaration(0)
    , num_ASTTypeId(0)
    , num_PQName(0)
    , num_TypeSpecifier(0)
    , num_BaseClassSpec(0)
    , num_Enumerator(0)
    , num_MemberList(0)
    , num_Member(0)
    , num_Declarator(0)
    , num_IDeclarator(0)
    , num_ExceptionSpec(0)
    , num_OperatorName(0)
    , num_Statement(0)
    , num_Condition(0)
    , num_Handler(0)
    , num_Expression(0)
    , num_FullExpression(0)
    , num_ArgExpression(0)
    , num_ArgExpressionListOpt(0)
    , num_Initializer(0)
    , num_TemplateDeclaration(0)
    , num_TemplateParameter(0)
    , num_TemplateArgument(0)
    , num_NamespaceDecl(0)
    , num_FullExpressionAnnot(0)
    , num_ASTTypeof(0)
    , num_Designator(0)
    , num_AttributeSpecifierList(0)
    , num_AttributeSpecifier(0)
    , num_Attribute(0)
  {}
  virtual ~HistogramASTVisitor() {}

  void printHistogram(ostream &out);

  virtual void postvisitTranslationUnit(TranslationUnit *obj);
  virtual void postvisitTopForm(TopForm *obj);
  virtual void postvisitFunction(Function *obj);
  virtual void postvisitMemberInit(MemberInit *obj);
  virtual void postvisitDeclaration(Declaration *obj);
  virtual void postvisitASTTypeId(ASTTypeId *obj);
  virtual void postvisitPQName(PQName *obj);
  virtual void postvisitTypeSpecifier(TypeSpecifier *obj);
  virtual void postvisitBaseClassSpec(BaseClassSpec *obj);
  virtual void postvisitEnumerator(Enumerator *obj);
  virtual void postvisitMemberList(MemberList *obj);
  virtual void postvisitMember(Member *obj);
  virtual void postvisitDeclarator(Declarator *obj);
  virtual void postvisitIDeclarator(IDeclarator *obj);
  virtual void postvisitExceptionSpec(ExceptionSpec *obj);
  virtual void postvisitOperatorName(OperatorName *obj);
  virtual void postvisitStatement(Statement *obj);
  virtual void postvisitCondition(Condition *obj);
  virtual void postvisitHandler(Handler *obj);
  virtual void postvisitExpression(Expression *obj);
  virtual void postvisitFullExpression(FullExpression *obj);
  virtual void postvisitArgExpression(ArgExpression *obj);
  virtual void postvisitArgExpressionListOpt(ArgExpressionListOpt *obj);
  virtual void postvisitInitializer(Initializer *obj);
  virtual void postvisitTemplateDeclaration(TemplateDeclaration *obj);
  virtual void postvisitTemplateParameter(TemplateParameter *obj);
  virtual void postvisitTemplateArgument(TemplateArgument *obj);
  virtual void postvisitNamespaceDecl(NamespaceDecl *obj);
  virtual void postvisitFullExpressionAnnot(FullExpressionAnnot *obj);
  virtual void postvisitASTTypeof(ASTTypeof *obj);
  virtual void postvisitDesignator(Designator *obj);
  virtual void postvisitAttributeSpecifierList(AttributeSpecifierList *obj);
  virtual void postvisitAttributeSpecifier(AttributeSpecifier *obj);
  virtual void postvisitAttribute(Attribute *obj);
};

#endif // STATICPRINT_H
