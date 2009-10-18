// see License.txt for copyright and terms of use

#include "alloctool.h"        // this module
#include "alloctool_cmd.h"    // AllocToolCmd
#include "alloctool_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "strutil.h"            // quoted
#include "oink_util.h"

// void printIhgOneVar(Variable *var0) {
//   Variable_O *var = asVariable_O(var0);
//   // NOTE: we don't do this as typedefs are not linker-visible.
//   // Waiting on a preprocessor canonicalizer
//   // FIX: linker-reps are gone but this concern is perhaps not.
//   //      var = var->lr();

//   // skip anything that isn't a typedef
//   if (!var->hasFlag(DF_TYPEDEF)) return;
//   // skip the selfnames
//   if (var->hasFlag(DF_SELFNAME)) return;
//   Type *t = var->getType();

//   // skip anything that isn't a typedef of an atomic type
//   if (!t->isCVAtomicType()) return;
//   AtomicType *atomic = t->asCVAtomicType()->atomic;

//   // skip anything that isn't a typedef of a compound type
//   if (!atomic->isCompoundType()) return;
//   CompoundType *ct = atomic->asCompoundType();

//   // get the name
//   // I no longer consider typedefs to be linker-visible
//   //      xassert(var->linkerVisibleName());
//   char const *name = strdup(var->fullyQualifiedName0().c_str());

//   // print regular bases
//   FOREACH_OBJLIST(BaseClass, ct->bases, iter) {
//     BaseClass const *bc = iter.data();
//     Variable_O *baseVar = asVariable_O(bc->ct->typedefVar);
//     // NOTE: we don't do this as typedefs are not linker-visible.
//     // Waiting on a preprocessor canonicalizer
//     // FIX: linker-reps are gone but this concern is perhaps not.
//     //        baseVar = baseVar->lr();
//     char const *baseName = strdup(baseVar->fullyQualifiedName0().c_str());
//     std::cout << quoted(stringc
//                         << baseName
//                         << "@"
//                         << locToStr(baseVar->loc))
//               << " -> "
//               << quoted(stringc
//                         << name
//                         << "@"
//                         << locToStr(var->loc))
//               << std::endl;
//     std::cout << "[";
//     // render the inheritance hierarchy in the traditional direction
//     std::cout << "dir=back,";
//     // render the access
//     switch (bc->access) {
//     default:
//       xfailure("illegal access"); break;
//     case AK_UNSPECIFIED:
//       xfailure("access should not still be unspecified"); break;
//     case AK_PUBLIC:
//       break;                  // the default
//     case AK_PROTECTED:
//       std::cout << "label=protected,"; break;
//     case AK_PRIVATE:
//       std::cout << "label=private,"; break;
//     }
//     // render in blue if virtual
//     if (bc->isVirtual) {
//       std::cout << "color=blue,";
//     }
//     std::cout << "]" << std::endl;
//   }
// }

// void AllocTool::printIhg_stage() {
//   std::cout << "digraph G {" << std::endl;
//   VisitRealVars_filter visPrintIhg(printIhgOneVar); // VAR-TRAVERSAL
//   visitVarsMarkedRealF_filtered(builtinVars, visPrintIhg);
//   foreachSourceFile {
//     File *file = files.data();
//     maybeSetInputLangFromSuffix(file);
//     TranslationUnit *unit = file2unit.get(file);
//     // print out the inheritance graph for each typedef variable that
//     // maps to a class
//     visitRealVarsF_filtered(unit, visPrintIhg);
//   }
//   std::cout << "}" << std::endl;
// }

// void AllocTool::printASTHistogram_stage() {
//   printStage("printASTHistogram");
//   foreachSourceFile {
//     File *file = files.data();
//     maybeSetInputLangFromSuffix(file);
//     printStart(file->name.c_str());
//     TranslationUnit *unit = file2unit.get(file);
//     // NOTE: we aren't doing lowered visitation
//     HistogramASTVisitor vis;
//     unit->traverse(vis);
//     vis.printHistogram(std::cout);
//     printStop();
//   }
// }

// // HistogramASTVisitor ****

// void HistogramASTVisitor::printHistogram(std::ostream &out) {
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

// // ****

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
