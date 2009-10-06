// see License.txt for copyright and terms of use

#include "bullet.h"        // this module
#include "bullet_cmd.h"    // BulletCmd
#include "bullet_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "strutil.h"            // quoted
#include "oink_util.h"

// LLVM headers ****

#define _DEBUG

// the headers seem to want these macros to be defined
// #define _GNU_SOURCE
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/PassManager.h>
#include <llvm/CallingConv.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

// Emit stage ****

// make an LLVM module
llvm::Module *makeModule() {
  printf("%s:%d make module\n", __FILE__, __LINE__);
//   Module* Mod = makeLLVMModule();
  llvm::Module *mod = new llvm::Module("test");

  // make a function object
  printf("%s:%d make function\n", __FILE__, __LINE__);
//   Constant* c = mod->getOrInsertFunction
//     ("mul_add",
//      /*ret type*/ IntegerType::get(32),
//      /*args*/ IntegerType::get(32),
//      IntegerType::get(32),
//      IntegerType::get(32),
//      /*varargs terminated with null*/ NULL);
  llvm::Constant *c = mod->getOrInsertFunction
    ("main",                    // function name
     llvm::IntegerType::get(32), // return type
     llvm::IntegerType::get(32), // one argument (argc)
     NULL                       // terminate list of varargs
     );
  llvm::Function *main_function = llvm::cast<llvm::Function>(c);
  main_function->setCallingConv(llvm::CallingConv::C);

  // make the body of the function
  printf("%s:%d make body\n", __FILE__, __LINE__);
  llvm::Function::arg_iterator args = main_function->arg_begin();
  llvm::Value* arg1 = args++;
  arg1->setName("argc");

  printf("%s:%d make block\n", __FILE__, __LINE__);
  llvm::BasicBlock *block = llvm::BasicBlock::Create("entry", main_function);
  llvm::IRBuilder<> builder(block);
  llvm::Value* tmp = builder.CreateBinOp(llvm::Instruction::Mul, arg1, arg1, "tmp");
  builder.CreateRet(tmp);

  return mod;
}

void Bullet::emit_stage() {
  printStage("emit");
//   foreachSourceFile {
//     File *file = files.data();
//     maybeSetInputLangFromSuffix(file);
//     printStart(file->name.c_str());
//     TranslationUnit *unit = file2unit.get(file);
//     CodeGenASTVisitor vis;
//     unit->traverse(vis.loweredVisitor);
//     // post-processing here
// //     vis.printHistogram(cout);
//     printStop();
//   }

  // modified from:
  // file:///Users/dsw/Notes/llvm-2.5-src/docs/tutorial/JITTutorial1.html
  llvm::Module *mod = makeModule();

  // verify the module
  printf("%s:%d verify\n", __FILE__, __LINE__);
  verifyModule(*mod, llvm::PrintMessageAction);

  // render the module
  printf("%s:%d render\n", __FILE__, __LINE__);
  llvm::PassManager PM;
  llvm::raw_os_ostream out(std::cout);
  llvm::ModulePass *pmp = createPrintModulePass(&out);
  PM.add(pmp);
  PM.run(*mod);

  // delete the module
  printf("%s:%d delete module\n", __FILE__, __LINE__);
  delete mod;
}

void CodeGenASTVisitor::postvisitStatement(Statement *obj) {
  obj->debugPrint(std::cout, 0);
//   std::cout << "" << std::endl;
}

// Example stage ****

// void Bullet::printASTHistogram_stage() {
//   printStage("printASTHistogram");
//   foreachSourceFile {
//     File *file = files.data();
//     maybeSetInputLangFromSuffix(file);
//     printStart(file->name.c_str());
//     TranslationUnit *unit = file2unit.get(file);
//     // NOTE: we aren't doing lowered visitation
//     CodeGenASTVisitor vis;
//     unit->traverse(vis);
//     vis.printHistogram(cout);
//     printStop();
//   }
// }

// CodeGenASTVisitor ****

// void CodeGenASTVisitor::printHistogram(ostream &out) {
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

// void CodeGenASTVisitor::postvisitTranslationUnit(TranslationUnit *obj) {
//   ++num_TranslationUnit;
// }

// void CodeGenASTVisitor::postvisitTopForm(TopForm *obj) {
//   ++num_TopForm;
// }

// void CodeGenASTVisitor::postvisitFunction(Function *obj) {
//   ++num_Function;
// }

// void CodeGenASTVisitor::postvisitMemberInit(MemberInit *obj) {
//   ++num_MemberInit;
// }

// void CodeGenASTVisitor::postvisitDeclaration(Declaration *obj) {
//   ++num_Declaration;
// }

// void CodeGenASTVisitor::postvisitASTTypeId(ASTTypeId *obj) {
//   ++num_ASTTypeId;
// }

// void CodeGenASTVisitor::postvisitPQName(PQName *obj) {
//   ++num_PQName;
// }

// void CodeGenASTVisitor::postvisitTypeSpecifier(TypeSpecifier *obj) {
//   ++num_TypeSpecifier;
// }

// void CodeGenASTVisitor::postvisitBaseClassSpec(BaseClassSpec *obj) {
//   ++num_BaseClassSpec;
// }

// void CodeGenASTVisitor::postvisitEnumerator(Enumerator *obj) {
//   ++num_Enumerator;
// }

// void CodeGenASTVisitor::postvisitMemberList(MemberList *obj) {
//   ++num_MemberList;
// }

// void CodeGenASTVisitor::postvisitMember(Member *obj) {
//   ++num_Member;
// }

// void CodeGenASTVisitor::postvisitDeclarator(Declarator *obj) {
//   ++num_Declarator;
// }

// void CodeGenASTVisitor::postvisitIDeclarator(IDeclarator *obj) {
//   ++num_IDeclarator;
// }

// void CodeGenASTVisitor::postvisitExceptionSpec(ExceptionSpec *obj) {
//   ++num_ExceptionSpec;
// }

// void CodeGenASTVisitor::postvisitOperatorName(OperatorName *obj) {
//   ++num_OperatorName;
// }

// void CodeGenASTVisitor::postvisitStatement(Statement *obj) {
//   ++num_Statement;
// }

// void CodeGenASTVisitor::postvisitCondition(Condition *obj) {
//   ++num_Condition;
// }

// void CodeGenASTVisitor::postvisitHandler(Handler *obj) {
//   ++num_Handler;
// }

// void CodeGenASTVisitor::postvisitExpression(Expression *obj) {
//   ++num_Expression;
// }

// void CodeGenASTVisitor::postvisitFullExpression(FullExpression *obj) {
//   ++num_FullExpression;
// }

// void CodeGenASTVisitor::postvisitArgExpression(ArgExpression *obj) {
//   ++num_ArgExpression;
// }

// void CodeGenASTVisitor::postvisitArgExpressionListOpt(ArgExpressionListOpt *obj) {
//   ++num_ArgExpressionListOpt;
// }

// void CodeGenASTVisitor::postvisitInitializer(Initializer *obj) {
//   ++num_Initializer;
// }

// void CodeGenASTVisitor::postvisitTemplateDeclaration(TemplateDeclaration *obj) {
//   ++num_TemplateDeclaration;
// }

// void CodeGenASTVisitor::postvisitTemplateParameter(TemplateParameter *obj) {
//   ++num_TemplateParameter;
// }

// void CodeGenASTVisitor::postvisitTemplateArgument(TemplateArgument *obj) {
//   ++num_TemplateArgument;
// }

// void CodeGenASTVisitor::postvisitNamespaceDecl(NamespaceDecl *obj) {
//   ++num_NamespaceDecl;
// }

// void CodeGenASTVisitor::postvisitFullExpressionAnnot(FullExpressionAnnot *obj) {
//   ++num_FullExpressionAnnot;
// }

// void CodeGenASTVisitor::postvisitASTTypeof(ASTTypeof *obj) {
//   ++num_ASTTypeof;
// }

// void CodeGenASTVisitor::postvisitDesignator(Designator *obj) {
//   ++num_Designator;
// }

// void CodeGenASTVisitor::postvisitAttributeSpecifierList(AttributeSpecifierList *obj) {
//   ++num_AttributeSpecifierList;
// }

// void CodeGenASTVisitor::postvisitAttributeSpecifier(AttributeSpecifier *obj) {
//   ++num_AttributeSpecifier;
// }

// void CodeGenASTVisitor::postvisitAttribute(Attribute *obj) {
//   ++num_Attribute;
// }
