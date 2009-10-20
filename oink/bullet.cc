// see License.txt for copyright and terms of use

#include <iostream>

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

using std::cout;
using std::ostream;

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
  llvm::Module *mod = NULL;
  foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      printStart(file->name.c_str());
      TranslationUnit *unit = file2unit.get(file);
      CodeGenASTVisitor vis;
      unit->traverse(vis.loweredVisitor);
      mod = vis.mod;
      // post-processing here
      vis.printHistogram(cout);
      printStop();
  }

  // modified from:
  // file:///Users/dsw/Notes/llvm-2.5-src/docs/tutorial/JITTutorial1.html
  // llvm::Module *mod = makeModule();

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

// Example stage ****

void Bullet::printASTHistogram_stage() {
  printStage("printASTHistogram");
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    printStart(file->name.c_str());
    TranslationUnit *unit = file2unit.get(file);
    // NOTE: we aren't doing lowered visitation
    CodeGenASTVisitor vis;
    unit->traverse(vis);
    vis.printHistogram(cout);
    printStop();
  }
}

// CodeGenASTVisitor ****

CodeGenASTVisitor::CodeGenASTVisitor()
  : loweredVisitor(this)
  , num_TranslationUnit(0)
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
{
  mod = new llvm::Module("test");
}

void CodeGenASTVisitor::printHistogram(ostream &out) {
  out << "TranslationUnit: " << num_TranslationUnit << "\n";
  out << "TopForm: " << num_TopForm << "\n";
  out << "Function: " << num_Function << "\n";
  out << "MemberInit: " << num_MemberInit << "\n";
  out << "Declaration: " << num_Declaration << "\n";
  out << "ASTTypeId: " << num_ASTTypeId << "\n";
  out << "PQName: " << num_PQName << "\n";
  out << "TypeSpecifier: " << num_TypeSpecifier << "\n";
  out << "BaseClassSpec: " << num_BaseClassSpec << "\n";
  out << "Enumerator: " << num_Enumerator << "\n";
  out << "MemberList: " << num_MemberList << "\n";
  out << "Member: " << num_Member << "\n";
  out << "Declarator: " << num_Declarator << "\n";
  out << "IDeclarator: " << num_IDeclarator << "\n";
  out << "ExceptionSpec: " << num_ExceptionSpec << "\n";
  out << "OperatorName: " << num_OperatorName << "\n";
  out << "Statement: " << num_Statement << "\n";
  out << "Condition: " << num_Condition << "\n";
  out << "Handler: " << num_Handler << "\n";
  out << "Expression: " << num_Expression << "\n";
  out << "FullExpression: " << num_FullExpression << "\n";
  out << "ArgExpression: " << num_ArgExpression << "\n";
  out << "ArgExpressionListOpt: " << num_ArgExpressionListOpt << "\n";
  out << "Initializer: " << num_Initializer << "\n";
  out << "TemplateDeclaration: " << num_TemplateDeclaration << "\n";
  out << "TemplateParameter: " << num_TemplateParameter << "\n";
  out << "TemplateArgument: " << num_TemplateArgument << "\n";
  out << "NamespaceDecl: " << num_NamespaceDecl << "\n";
  out << "FullExpressionAnnot: " << num_FullExpressionAnnot << "\n";
  out << "ASTTypeof: " << num_ASTTypeof << "\n";
  out << "Designator: " << num_Designator << "\n";
  out << "AttributeSpecifierList: " << num_AttributeSpecifierList << "\n";
  out << "AttributeSpecifier: " << num_AttributeSpecifier << "\n";
  out << "Attribute: " << num_Attribute << "\n";
}

// ****

bool CodeGenASTVisitor::visitTranslationUnit(TranslationUnit *obj) {
  ++num_TranslationUnit;
  return true;
}

void CodeGenASTVisitor::postvisitTranslationUnit(TranslationUnit *obj) {
}

bool CodeGenASTVisitor::visitTopForm(TopForm *obj) {
  ++num_TopForm;
  return true;
}

void CodeGenASTVisitor::postvisitTopForm(TopForm *obj) {
}

bool CodeGenASTVisitor::visitFunction(Function *obj) {
  obj->debugPrint(std::cout, 0);
  
  std::vector<const llvm::Type*> paramTypes;
  llvm::FunctionType* funcType =
    llvm::FunctionType::get(llvm::IntegerType::get(32), paramTypes, /*isVarArg*/false);
  llvm::Constant *c = mod->getOrInsertFunction
    (obj->nameAndParams->var->name,  // function name
     funcType);
  currentFunction = llvm::cast<llvm::Function>(c);
  ++num_Function;
  return true;
}

void CodeGenASTVisitor::postvisitFunction(Function *obj) {
  currentFunction = NULL;
}

bool CodeGenASTVisitor::visitMemberInit(MemberInit *obj) {
  ++num_MemberInit;
  return true;
}

void CodeGenASTVisitor::postvisitMemberInit(MemberInit *obj) {
}

bool CodeGenASTVisitor::visitDeclaration(Declaration *obj) {
  ++num_Declaration;
  return true;
}

void CodeGenASTVisitor::postvisitDeclaration(Declaration *obj) {
}

bool CodeGenASTVisitor::visitASTTypeId(ASTTypeId *obj) {
  ++num_ASTTypeId;
  return true;
}

void CodeGenASTVisitor::postvisitASTTypeId(ASTTypeId *obj) {
}

bool CodeGenASTVisitor::visitPQName(PQName *obj) {
  ++num_PQName;
  return true;
}

void CodeGenASTVisitor::postvisitPQName(PQName *obj) {
}

bool CodeGenASTVisitor::visitTypeSpecifier(TypeSpecifier *obj) {
  ++num_TypeSpecifier;
  return true;
}

void CodeGenASTVisitor::postvisitTypeSpecifier(TypeSpecifier *obj) {
}

bool CodeGenASTVisitor::visitBaseClassSpec(BaseClassSpec *obj) {
  ++num_BaseClassSpec;
  return true;
}

void CodeGenASTVisitor::postvisitBaseClassSpec(BaseClassSpec *obj) {
}

bool CodeGenASTVisitor::visitEnumerator(Enumerator *obj) {
  ++num_Enumerator;
  return true;
}

void CodeGenASTVisitor::postvisitEnumerator(Enumerator *obj) {
}

bool CodeGenASTVisitor::visitMemberList(MemberList *obj) {
  ++num_MemberList;
  return true;
}

void CodeGenASTVisitor::postvisitMemberList(MemberList *obj) {
}

bool CodeGenASTVisitor::visitMember(Member *obj) {
  ++num_Member;
  return true;
}

void CodeGenASTVisitor::postvisitMember(Member *obj) {
}

bool CodeGenASTVisitor::visitDeclarator(Declarator *obj) {
  ++num_Declarator;
  return true;
}

void CodeGenASTVisitor::postvisitDeclarator(Declarator *obj) {
}

bool CodeGenASTVisitor::visitIDeclarator(IDeclarator *obj) {
  ++num_IDeclarator;
  return true;
}

void CodeGenASTVisitor::postvisitIDeclarator(IDeclarator *obj) {
}

bool CodeGenASTVisitor::visitExceptionSpec(ExceptionSpec *obj) {
  ++num_ExceptionSpec;
  return true;
}

void CodeGenASTVisitor::postvisitExceptionSpec(ExceptionSpec *obj) {
}

bool CodeGenASTVisitor::visitOperatorName(OperatorName *obj) {
  ++num_OperatorName;
  return true;
}

void CodeGenASTVisitor::postvisitOperatorName(OperatorName *obj) {
}

bool CodeGenASTVisitor::visitStatement(Statement *obj) {
  obj->debugPrint(std::cout, 0);
  if (obj->kind() == Statement::S_COMPOUND) {
    assert (currentBlock == NULL); // Nested blocks unimplemented
    currentBlock = llvm::BasicBlock::Create(locToStr(obj->loc).c_str(), currentFunction);
  }
  ++num_Statement;
  return true;
}

void CodeGenASTVisitor::postvisitStatement(Statement *obj) {
  if (obj->kind() == Statement::S_COMPOUND) {
    assert (currentBlock != NULL);
    currentBlock = NULL;
  }
  else if (obj->kind() == Statement::S_RETURN) {
    assert (currentBlock != NULL);
    llvm::IRBuilder<> builder(currentBlock);
    builder.CreateRet(lastValue);
  }
}

bool CodeGenASTVisitor::visitCondition(Condition *obj) {
  ++num_Condition;
  return true;
}

void CodeGenASTVisitor::postvisitCondition(Condition *obj) {
}

bool CodeGenASTVisitor::visitHandler(Handler *obj) {
  ++num_Handler;
  return true;
}

void CodeGenASTVisitor::postvisitHandler(Handler *obj) {
}

bool CodeGenASTVisitor::visitExpression(Expression *obj) {
  ++num_Expression;
  return true;
}

void CodeGenASTVisitor::postvisitExpression(Expression *obj) {
  lastValue = NULL;
  obj->debugPrint(std::cout, 0);
  if (obj->kind() == Expression::E_INTLIT) {
    E_intLit* intLit = static_cast<E_intLit *>(obj);
    llvm::IRBuilder<> builder(currentBlock);
    lastValue = llvm::ConstantInt::get(llvm::Type::Int32Ty, intLit->i);
  }
}

bool CodeGenASTVisitor::visitFullExpression(FullExpression *obj) {
  ++num_FullExpression;
  return true;
}

void CodeGenASTVisitor::postvisitFullExpression(FullExpression *obj) {
}

bool CodeGenASTVisitor::visitArgExpression(ArgExpression *obj) {
  ++num_ArgExpression;
  return true;
}

void CodeGenASTVisitor::postvisitArgExpression(ArgExpression *obj) {
}

bool CodeGenASTVisitor::visitArgExpressionListOpt(ArgExpressionListOpt *obj) {
  ++num_ArgExpressionListOpt;
  return true;
}

void CodeGenASTVisitor::postvisitArgExpressionListOpt(ArgExpressionListOpt *obj) {
}

bool CodeGenASTVisitor::visitInitializer(Initializer *obj) {
  ++num_Initializer;
  return true;
}

void CodeGenASTVisitor::postvisitInitializer(Initializer *obj) {
}

bool CodeGenASTVisitor::visitTemplateDeclaration(TemplateDeclaration *obj) {
  ++num_TemplateDeclaration;
  return true;
}

void CodeGenASTVisitor::postvisitTemplateDeclaration(TemplateDeclaration *obj) {
}

bool CodeGenASTVisitor::visitTemplateParameter(TemplateParameter *obj) {
  ++num_TemplateParameter;
  return true;
}

void CodeGenASTVisitor::postvisitTemplateParameter(TemplateParameter *obj) {
}

bool CodeGenASTVisitor::visitTemplateArgument(TemplateArgument *obj) {
  ++num_TemplateArgument;
  return true;
}

void CodeGenASTVisitor::postvisitTemplateArgument(TemplateArgument *obj) {
}

bool CodeGenASTVisitor::visitNamespaceDecl(NamespaceDecl *obj) {
  ++num_NamespaceDecl;
  return true;
}

void CodeGenASTVisitor::postvisitNamespaceDecl(NamespaceDecl *obj) {
}

bool CodeGenASTVisitor::visitFullExpressionAnnot(FullExpressionAnnot *obj) {
  ++num_FullExpressionAnnot;
  return true;
}

void CodeGenASTVisitor::postvisitFullExpressionAnnot(FullExpressionAnnot *obj) {
}

bool CodeGenASTVisitor::visitASTTypeof(ASTTypeof *obj) {
  ++num_ASTTypeof;
  return true;
}

void CodeGenASTVisitor::postvisitASTTypeof(ASTTypeof *obj) {
}

bool CodeGenASTVisitor::visitDesignator(Designator *obj) {
  ++num_Designator;
  return true;
}

void CodeGenASTVisitor::postvisitDesignator(Designator *obj) {
}

bool CodeGenASTVisitor::visitAttributeSpecifierList(AttributeSpecifierList *obj) {
  ++num_AttributeSpecifierList;
  return true;
}

void CodeGenASTVisitor::postvisitAttributeSpecifierList(AttributeSpecifierList *obj) {
}

bool CodeGenASTVisitor::visitAttributeSpecifier(AttributeSpecifier *obj) {
  ++num_AttributeSpecifier;
  return true;
}

void CodeGenASTVisitor::postvisitAttributeSpecifier(AttributeSpecifier *obj) {
}

bool CodeGenASTVisitor::visitAttribute(Attribute *obj) {
  ++num_Attribute;
  return true;
}

void CodeGenASTVisitor::postvisitAttribute(Attribute *obj) {
}
