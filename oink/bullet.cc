// see License.txt for copyright and terms of use

#include "bullet.h"        // this module
#include "bullet_cmd.h"    // BulletCmd
#include "bullet_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "strutil.h"            // quoted
#include "oink_util.h"

// void Bullet::printASTHistogram_stage() {
//   printStage("printASTHistogram");
//   foreachSourceFile {
//     File *file = files.data();
//     maybeSetInputLangFromSuffix(file);
//     printStart(file->name.c_str());
//     TranslationUnit *unit = file2unit.get(file);
//     // NOTE: we aren't doing lowered visitation
//     HistogramASTVisitor vis;
//     unit->traverse(vis);
//     vis.printHistogram(cout);
//     printStop();
//   }
// }

// HistogramASTVisitor ****

// void HistogramASTVisitor::printHistogram(ostream &out) {
//   out << "TranslationUnit: " << num_TranslationUnit << "\n";
//   out << "TopForm: " << num_TopForm << "\n";
//   out << "Function: " << num_Function << "\n";
//   out << "MemberInit: " << num_MemberInit << "\n";
//   out << "Declaration: " << num_Declaration << "\n";
//   out << "ASTTypeId: " << num_ASTTypeId << "\n";
//   out << "PQName: " << num_PQName << "\n";
//   out << "TypeSpecifier: " << num_TypeSpecifier << "\n";
//   out << "BaseClassSpec: " << num_BaseClassSpec << "\n";
//   out << "Enumerator: " << num_Enumerator << "\n";
//   out << "MemberList: " << num_MemberList << "\n";
//   out << "Member: " << num_Member << "\n";
//   out << "Declarator: " << num_Declarator << "\n";
//   out << "IDeclarator: " << num_IDeclarator << "\n";
//   out << "ExceptionSpec: " << num_ExceptionSpec << "\n";
//   out << "OperatorName: " << num_OperatorName << "\n";
//   out << "Statement: " << num_Statement << "\n";
//   out << "Condition: " << num_Condition << "\n";
//   out << "Handler: " << num_Handler << "\n";
//   out << "Expression: " << num_Expression << "\n";
//   out << "FullExpression: " << num_FullExpression << "\n";
//   out << "ArgExpression: " << num_ArgExpression << "\n";
//   out << "ArgExpressionListOpt: " << num_ArgExpressionListOpt << "\n";
//   out << "Initializer: " << num_Initializer << "\n";
//   out << "TemplateDeclaration: " << num_TemplateDeclaration << "\n";
//   out << "TemplateParameter: " << num_TemplateParameter << "\n";
//   out << "TemplateArgument: " << num_TemplateArgument << "\n";
//   out << "NamespaceDecl: " << num_NamespaceDecl << "\n";
//   out << "FullExpressionAnnot: " << num_FullExpressionAnnot << "\n";
//   out << "ASTTypeof: " << num_ASTTypeof << "\n";
//   out << "Designator: " << num_Designator << "\n";
//   out << "AttributeSpecifierList: " << num_AttributeSpecifierList << "\n";
//   out << "AttributeSpecifier: " << num_AttributeSpecifier << "\n";
//   out << "Attribute: " << num_Attribute << "\n";
// }

// ****

// void HistogramASTVisitor::postvisitTranslationUnit(TranslationUnit *obj) {
//   ++num_TranslationUnit;
// }

// void HistogramASTVisitor::postvisitTopForm(TopForm *obj) {
//   ++num_TopForm;
// }

// void HistogramASTVisitor::postvisitFunction(Function *obj) {
//   ++num_Function;
// }

// void HistogramASTVisitor::postvisitMemberInit(MemberInit *obj) {
//   ++num_MemberInit;
// }

// void HistogramASTVisitor::postvisitDeclaration(Declaration *obj) {
//   ++num_Declaration;
// }

// void HistogramASTVisitor::postvisitASTTypeId(ASTTypeId *obj) {
//   ++num_ASTTypeId;
// }

// void HistogramASTVisitor::postvisitPQName(PQName *obj) {
//   ++num_PQName;
// }

// void HistogramASTVisitor::postvisitTypeSpecifier(TypeSpecifier *obj) {
//   ++num_TypeSpecifier;
// }

// void HistogramASTVisitor::postvisitBaseClassSpec(BaseClassSpec *obj) {
//   ++num_BaseClassSpec;
// }

// void HistogramASTVisitor::postvisitEnumerator(Enumerator *obj) {
//   ++num_Enumerator;
// }

// void HistogramASTVisitor::postvisitMemberList(MemberList *obj) {
//   ++num_MemberList;
// }

// void HistogramASTVisitor::postvisitMember(Member *obj) {
//   ++num_Member;
// }

// void HistogramASTVisitor::postvisitDeclarator(Declarator *obj) {
//   ++num_Declarator;
// }

// void HistogramASTVisitor::postvisitIDeclarator(IDeclarator *obj) {
//   ++num_IDeclarator;
// }

// void HistogramASTVisitor::postvisitExceptionSpec(ExceptionSpec *obj) {
//   ++num_ExceptionSpec;
// }

// void HistogramASTVisitor::postvisitOperatorName(OperatorName *obj) {
//   ++num_OperatorName;
// }

// void HistogramASTVisitor::postvisitStatement(Statement *obj) {
//   ++num_Statement;
// }

// void HistogramASTVisitor::postvisitCondition(Condition *obj) {
//   ++num_Condition;
// }

// void HistogramASTVisitor::postvisitHandler(Handler *obj) {
//   ++num_Handler;
// }

// void HistogramASTVisitor::postvisitExpression(Expression *obj) {
//   ++num_Expression;
// }

// void HistogramASTVisitor::postvisitFullExpression(FullExpression *obj) {
//   ++num_FullExpression;
// }

// void HistogramASTVisitor::postvisitArgExpression(ArgExpression *obj) {
//   ++num_ArgExpression;
// }

// void HistogramASTVisitor::postvisitArgExpressionListOpt(ArgExpressionListOpt *obj) {
//   ++num_ArgExpressionListOpt;
// }

// void HistogramASTVisitor::postvisitInitializer(Initializer *obj) {
//   ++num_Initializer;
// }

// void HistogramASTVisitor::postvisitTemplateDeclaration(TemplateDeclaration *obj) {
//   ++num_TemplateDeclaration;
// }

// void HistogramASTVisitor::postvisitTemplateParameter(TemplateParameter *obj) {
//   ++num_TemplateParameter;
// }

// void HistogramASTVisitor::postvisitTemplateArgument(TemplateArgument *obj) {
//   ++num_TemplateArgument;
// }

// void HistogramASTVisitor::postvisitNamespaceDecl(NamespaceDecl *obj) {
//   ++num_NamespaceDecl;
// }

// void HistogramASTVisitor::postvisitFullExpressionAnnot(FullExpressionAnnot *obj) {
//   ++num_FullExpressionAnnot;
// }

// void HistogramASTVisitor::postvisitASTTypeof(ASTTypeof *obj) {
//   ++num_ASTTypeof;
// }

// void HistogramASTVisitor::postvisitDesignator(Designator *obj) {
//   ++num_Designator;
// }

// void HistogramASTVisitor::postvisitAttributeSpecifierList(AttributeSpecifierList *obj) {
//   ++num_AttributeSpecifierList;
// }

// void HistogramASTVisitor::postvisitAttributeSpecifier(AttributeSpecifier *obj) {
//   ++num_AttributeSpecifier;
// }

// void HistogramASTVisitor::postvisitAttribute(Attribute *obj) {
//   ++num_Attribute;
// }
