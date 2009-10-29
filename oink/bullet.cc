// see License.txt for copyright and terms of use

#include <iostream>
#include <fstream>

#include "bullet.h"        // this module
#include "bullet_cmd.h"    // BulletCmd
#include "bullet_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "strutil.h"            // quoted
#include "oink_util.h"

#define _DEBUG

using std::cout;
using std::ostream;

// Emit stage ****

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
      printStop();
  }

  // verify the module
  printf("%s:%d verify\n", __FILE__, __LINE__);
  verifyModule(*mod, llvm::PrintMessageAction);

  // render the module
  printf("%s:%d render\n", __FILE__, __LINE__);
  llvm::PassManager PM;
  std::ofstream outFile("out.ll");
  llvm::raw_os_ostream out(outFile);
  llvm::ModulePass *pmp = createPrintModulePass(&out);
  PM.add(pmp);
  PM.run(*mod);

  delete mod;
}

// CodeGenASTVisitor ****

/// createTempAlloca - This creates a alloca and inserts it into the entry
/// block.
llvm::AllocaInst *CodeGenASTVisitor::createTempAlloca(const llvm::Type *ty, const char *name)
{
  return new llvm::AllocaInst(ty, 0, name, allocaInsertPt);
}

CodeGenASTVisitor::CodeGenASTVisitor()
  : context(llvm::getGlobalContext())
  , loweredVisitor(this)
{
  mod = new llvm::Module("test", context);
}

// ****

bool CodeGenASTVisitor::visitTranslationUnit(TranslationUnit *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitTranslationUnit(TranslationUnit *obj) {
}

bool CodeGenASTVisitor::visitTopForm(TopForm *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitTopForm(TopForm *obj) {
}

bool CodeGenASTVisitor::visitFunction(Function *obj) {
  obj->debugPrint(std::cout, 0);
  
  std::vector<const llvm::Type*> paramTypes;
  llvm::FunctionType* funcType =
    llvm::FunctionType::get(llvm::IntegerType::get(context, 32), paramTypes, /*isVarArg*/false);
  llvm::Constant *c = mod->getOrInsertFunction
    (obj->nameAndParams->var->name,  // function name
     funcType);
  currentFunction = llvm::cast<llvm::Function>(c);

  llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", currentFunction);

  // Create a marker to make it easy to insert allocas into the entryblock
  // later.  Don't create this with the builder, because we don't want it
  // folded.
  llvm::Value *Undef = llvm::UndefValue::get(llvm::Type::getInt32Ty(context));
  allocaInsertPt = new llvm::BitCastInst(Undef, llvm::Type::getInt32Ty(context), "",
					 entryBlock);

  llvm::BasicBlock* bodyEnterBlock =
    llvm::BasicBlock::Create(context, locToStr(obj->body->loc).c_str(), currentFunction);
  llvm::IRBuilder<> builder(entryBlock);
  builder.CreateBr(bodyEnterBlock);
  genStatement(bodyEnterBlock, obj->body);

  return true;
}

bool CodeGenASTVisitor::visitMemberInit(MemberInit *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitMemberInit(MemberInit *obj) {
}

bool CodeGenASTVisitor::visitDeclaration(Declaration *obj) {
  return true;
}

const llvm::Type* CodeGenASTVisitor::makeTypeSpecifier(Type *t)
{
  const llvm::Type* type = NULL;

  switch (t->getTag())
  {
  case Type::T_ATOMIC: {
    CVAtomicType *cvat = t->asCVAtomicType();
    AtomicType *at = cvat->atomic;
    switch (at->getTag()) {
    case AtomicType::T_SIMPLE: {
      SimpleType *st = at->asSimpleType();
      SimpleTypeId id = st->type;
      switch (id) {
      case ST_CHAR: {
	type = llvm::IntegerType::get(context, 8);
	break;
      }
      case ST_UNSIGNED_CHAR: {
	type = llvm::IntegerType::get(context, 8);
	break;
      }
      case ST_SIGNED_CHAR: {
	type = llvm::IntegerType::get(context, 8);
	break;
      }
      case ST_INT: {
	type = llvm::IntegerType::get(context, 32);
	break;
      }
      case ST_UNSIGNED_INT: {
	type = llvm::IntegerType::get(context, 32);
	break;
      }
      case ST_LONG_INT: {
	type = llvm::IntegerType::get(context, 32);
	break;
      }
      case ST_UNSIGNED_LONG_INT: {
	type = llvm::IntegerType::get(context, 32);
	break;
      }
      case ST_LONG_LONG: {             // GNU/C99 extension
	type = llvm::IntegerType::get(context, 64);
	break;
      }
      case ST_UNSIGNED_LONG_LONG: {     // GNU/C99 extension
	type = llvm::IntegerType::get(context, 64);
	break;
      }
      case ST_SHORT_INT: {
	type = llvm::IntegerType::get(context, 16);
	break;
      }
      case ST_UNSIGNED_SHORT_INT: {
	type = llvm::IntegerType::get(context, 16);
	break;
      }
      case ST_WCHAR_T: {
	type = llvm::IntegerType::get(context, 16);
	break;
      }
      case ST_BOOL: {
	type = llvm::IntegerType::get(context, 8);
	break;
      }
      default: {
	assert(0);
      }
      }
      break;
    }
    default: {
      assert(0);
    }
    }
    break;
  }
  default: {
    assert(0);
  }
  }
  return type;
}

void CodeGenASTVisitor::postvisitDeclaration(Declaration *obj) {
}

bool CodeGenASTVisitor::visitASTTypeId(ASTTypeId *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitASTTypeId(ASTTypeId *obj) {
}

bool CodeGenASTVisitor::visitPQName(PQName *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitPQName(PQName *obj) {
}

bool CodeGenASTVisitor::visitTypeSpecifier(TypeSpecifier *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitTypeSpecifier(TypeSpecifier *obj) {
}

bool CodeGenASTVisitor::visitBaseClassSpec(BaseClassSpec *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitBaseClassSpec(BaseClassSpec *obj) {
}

bool CodeGenASTVisitor::visitEnumerator(Enumerator *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitEnumerator(Enumerator *obj) {
}

bool CodeGenASTVisitor::visitMemberList(MemberList *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitMemberList(MemberList *obj) {
}

bool CodeGenASTVisitor::visitMember(Member *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitMember(Member *obj) {
}

bool CodeGenASTVisitor::visitDeclarator(Declarator *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitDeclarator(Declarator *obj) {
}

bool CodeGenASTVisitor::visitIDeclarator(IDeclarator *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitIDeclarator(IDeclarator *obj) {
}

bool CodeGenASTVisitor::visitExceptionSpec(ExceptionSpec *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitExceptionSpec(ExceptionSpec *obj) {
}

bool CodeGenASTVisitor::visitOperatorName(OperatorName *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitOperatorName(OperatorName *obj) {
}

llvm::BasicBlock* CodeGenASTVisitor::genStatement(llvm::BasicBlock* currentBlock, Statement *obj) {
  assert (currentBlock != NULL);

  switch (obj->kind()) {
  case Statement::S_COMPOUND: {
    S_compound * s_compound = static_cast<S_compound*>(obj);
    FOREACH_ASTLIST_NC(Statement, s_compound->stmts, iter) {
      if (currentBlock == NULL) {
	currentBlock = llvm::BasicBlock::Create(context, locToStr(iter.data()->loc).c_str(), currentFunction);
      }
      currentBlock = genStatement(currentBlock, iter.data());
    }
    return currentBlock;
  }
  case Statement::S_RETURN: {
    S_return* returnStatement = static_cast<S_return*>(obj);
    llvm::Value* returnValue = fullExpressionToValue(currentBlock, returnStatement->expr);
    llvm::IRBuilder<> builder(currentBlock);
    builder.CreateRet(returnValue);
    return NULL;
  }
  case Statement::S_EXPR: {
    S_expr* exprStatement = static_cast<S_expr*>(obj);
    fullExpressionToValue(currentBlock, exprStatement->expr); // discard return
    return currentBlock;
  }
  case Statement::S_DECL: {
    S_decl * s_decl = static_cast<S_decl *>(obj);
    FAKELIST_FOREACH_NC(Declarator, s_decl->decl->decllist, iter) {
      Variable* var = iter->var;
      const llvm::Type* type = makeTypeSpecifier(var->type);

      if (var->flags & (DF_DEFINITION|DF_TEMPORARY)) {
	// A local variable.
	llvm::AllocaInst* lv = createTempAlloca(type, var->name);
	variables[var] = lv;
      }  
    }
    return currentBlock;
  }
  case Statement::S_IF: {
    S_if* s_if = static_cast<S_if*>(obj);

    llvm::BasicBlock* thenEnterBlock =
      llvm::BasicBlock::Create(context, "thenenter", currentFunction); // TODO: better name
    llvm::BasicBlock* thenExitBlock = genStatement(thenEnterBlock, s_if->thenBranch);

    bool noElseClause = s_if->elseBranch->kind() == Statement::S_COMPOUND &&
      static_cast<S_compound*>(s_if->elseBranch)->stmts.count() == 1 &&
      static_cast<S_compound*>(s_if->elseBranch)->stmts.first()->kind() == Statement::S_SKIP;
    llvm::BasicBlock* elseEnterBlock = NULL;
    llvm::BasicBlock* elseExitBlock = NULL;
    llvm::BasicBlock* ifAfterBlock = NULL;
    if (noElseClause) {
      elseEnterBlock = ifAfterBlock = llvm::BasicBlock::Create(context, "ifafter", currentFunction); // TODO: better name
    } else {
      elseEnterBlock = llvm::BasicBlock::Create(context, "elseenter", currentFunction); // TODO: better name
      elseExitBlock = genStatement(elseEnterBlock, s_if->elseBranch);
      if (thenExitBlock != NULL || elseExitBlock != NULL) {
	ifAfterBlock = llvm::BasicBlock::Create(context, "ifafter", currentFunction); // TODO: better name
      }
    }
    if (ifAfterBlock != NULL) {
      if (thenExitBlock != NULL) {
	llvm::IRBuilder<> builder(thenExitBlock);
	builder.CreateBr(ifAfterBlock);
      }
      if (elseExitBlock != NULL) {
	llvm::IRBuilder<> builder(elseExitBlock);
	builder.CreateBr(ifAfterBlock);
      }
    }
    {
      llvm::IRBuilder<> builder(currentBlock);
      llvm::Value* condValue = condToValue(currentBlock, s_if->cond);
      builder.CreateCondBr(condValue, thenEnterBlock, elseEnterBlock);
    }
    return ifAfterBlock;
  }
  case Statement::S_WHILE: {
    S_while* s_while = static_cast<S_while*>(obj);

    llvm::BasicBlock* whileCondBlock =
      llvm::BasicBlock::Create(context, "whilecond", currentFunction); // TODO: better name
    llvm::BasicBlock* bodyEnterBlock =
      llvm::BasicBlock::Create(context, "whilebody", currentFunction); // TODO: better name
    llvm::BasicBlock* whileAfterBlock =
      llvm::BasicBlock::Create(context, "whileafter", currentFunction); // TODO: better name

    {
      llvm::IRBuilder<> builder(currentBlock);
      builder.CreateBr(whileCondBlock);
    }
    {
      llvm::IRBuilder<> builder(whileCondBlock);
      llvm::Value* condValue = condToValue(whileCondBlock, s_while->cond);
      builder.CreateCondBr(condValue, bodyEnterBlock, whileAfterBlock);
    }

    llvm::BasicBlock* bodyExitBlock = genStatement(bodyEnterBlock, s_while->body);
    if (bodyExitBlock != NULL) {
      llvm::IRBuilder<> builder(bodyExitBlock);
      builder.CreateBr(whileCondBlock);
    }

    return whileAfterBlock;
  }
  case Statement::S_SKIP: {
    return currentBlock;
  }
  default: {
  assert(0);
  return NULL;
  }
  }
}

llvm::Value* CodeGenASTVisitor::condToValue(llvm::BasicBlock* currentBlock, Condition *obj) {
  switch (obj->kind()) {
  case Condition::CN_EXPR: {
    CN_expr* cn_expr = static_cast<CN_expr*>(obj);
    llvm::Value* valueAsInt = fullExpressionToValue(currentBlock, cn_expr->expr);
    llvm::IRBuilder<> builder(currentBlock);
    return builder.CreateICmp(llvm::CmpInst::ICMP_NE, valueAsInt,
			      llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0), "convertToBool");
  }
  default:
    assert(0);
    return NULL;
  }
}

bool CodeGenASTVisitor::visitCondition(Condition *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitCondition(Condition *obj) {
}

bool CodeGenASTVisitor::visitHandler(Handler *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitHandler(Handler *obj) {
}

llvm::Value* CodeGenASTVisitor::fullExpressionToValue(llvm::BasicBlock* currentBlock, FullExpression *obj) {
  return expressionToValue(currentBlock, obj->expr);
}

struct LlvmExpressionType {
  enum LlvmExpressionType_t {
    BinOpExpr,
    ICmpExpr,
  };
};

llvm::Value* CodeGenASTVisitor::expressionToLvalue(llvm::BasicBlock* currentBlock, Expression *obj) {
  switch (obj->kind()) {
  case Expression::E_VARIABLE: {
    E_variable* variableExpr = static_cast<E_variable *>(obj);
    return variables[variableExpr->var];
  }
  default: {
    assert(0);
    return NULL;
  }
  }
}

llvm::Value* CodeGenASTVisitor::expressionToValue(llvm::BasicBlock* currentBlock, Expression *obj) {
  switch (obj->kind()) {
  case Expression::E_INTLIT: {
    E_intLit* intLit = static_cast<E_intLit *>(obj);
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), intLit->i);
  }
  case Expression::E_VARIABLE: {
    E_variable* variableExpr = static_cast<E_variable *>(obj);
    llvm::IRBuilder<> builder(currentBlock);
    return builder.CreateLoad(variables[variableExpr->var], variableExpr->var->name);
  }
  case Expression::E_ASSIGN: {
    E_assign* assignExpr = static_cast<E_assign *>(obj);
    llvm::IRBuilder<> builder(currentBlock);
    switch (assignExpr->op) {
    case BIN_ASSIGN: {
      llvm::Value* src = expressionToValue(currentBlock, assignExpr->src);
      llvm::Value* target = expressionToLvalue(currentBlock, assignExpr->target);
      return builder.CreateStore(src, target);
    }
    default: {
      assert(0);
      return NULL;
    }
    }
  }
  case Expression::E_BINARY: {
    E_binary* binaryExpr = static_cast<E_binary *>(obj);
    llvm::IRBuilder<> builder(currentBlock);
    LlvmExpressionType::LlvmExpressionType_t exprType;
    llvm::BinaryOperator::BinaryOps op;
    llvm::CmpInst::Predicate pred;

    switch (binaryExpr->op) {
    case BIN_EQUAL:     pred = llvm::CmpInst::ICMP_EQ;  exprType = LlvmExpressionType::ICmpExpr; break;
    case BIN_NOTEQUAL:  pred = llvm::CmpInst::ICMP_NE;  exprType = LlvmExpressionType::ICmpExpr; break;
    case BIN_LESS:      pred = llvm::CmpInst::ICMP_SLT; exprType = LlvmExpressionType::ICmpExpr; break;
    case BIN_GREATER:   pred = llvm::CmpInst::ICMP_SGT; exprType = LlvmExpressionType::ICmpExpr; break;
    case BIN_LESSEQ:    pred = llvm::CmpInst::ICMP_SLE; exprType = LlvmExpressionType::ICmpExpr; break;
    case BIN_GREATEREQ: pred = llvm::CmpInst::ICMP_SGE; exprType = LlvmExpressionType::ICmpExpr; break;
    case BIN_PLUS:  op = llvm::Instruction::Add; exprType = LlvmExpressionType::BinOpExpr; break;
    case BIN_MINUS: op = llvm::Instruction::Sub; exprType = LlvmExpressionType::BinOpExpr; break;
    case BIN_MULT:  op = llvm::Instruction::Mul; exprType = LlvmExpressionType::BinOpExpr; break;
    default: assert(0); break;
    }
    llvm::Value* value1 = expressionToValue(currentBlock, binaryExpr->e1);
    llvm::Value* value2 = expressionToValue(currentBlock, binaryExpr->e2);
    switch (exprType) {
    case LlvmExpressionType::ICmpExpr: {
      llvm::Value* resultAsBool = builder.CreateICmp(pred, value1, value2, "expr"/*locToStr(obj->loc).c_str()*/);
      return builder.CreateIntCast(resultAsBool, llvm::Type::getInt32Ty(context), /*isSigned*/true);
    }
    case LlvmExpressionType::BinOpExpr: {
      return builder.CreateBinOp(op, value1, value2, "expr"/*locToStr(obj->loc).c_str()*/);
    }
    default: {
      assert(0);
      return NULL;
    }
    }
  }
  default: {
    assert(0);
    return NULL;
  }
  }
}

bool CodeGenASTVisitor::visitArgExpression(ArgExpression *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitArgExpression(ArgExpression *obj) {
}

bool CodeGenASTVisitor::visitArgExpressionListOpt(ArgExpressionListOpt *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitArgExpressionListOpt(ArgExpressionListOpt *obj) {
}

bool CodeGenASTVisitor::visitInitializer(Initializer *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitInitializer(Initializer *obj) {
}

bool CodeGenASTVisitor::visitTemplateDeclaration(TemplateDeclaration *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitTemplateDeclaration(TemplateDeclaration *obj) {
}

bool CodeGenASTVisitor::visitTemplateParameter(TemplateParameter *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitTemplateParameter(TemplateParameter *obj) {
}

bool CodeGenASTVisitor::visitTemplateArgument(TemplateArgument *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitTemplateArgument(TemplateArgument *obj) {
}

bool CodeGenASTVisitor::visitNamespaceDecl(NamespaceDecl *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitNamespaceDecl(NamespaceDecl *obj) {
}

bool CodeGenASTVisitor::visitFullExpressionAnnot(FullExpressionAnnot *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitFullExpressionAnnot(FullExpressionAnnot *obj) {
}

bool CodeGenASTVisitor::visitASTTypeof(ASTTypeof *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitASTTypeof(ASTTypeof *obj) {
}

bool CodeGenASTVisitor::visitDesignator(Designator *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitDesignator(Designator *obj) {
}

bool CodeGenASTVisitor::visitAttributeSpecifierList(AttributeSpecifierList *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitAttributeSpecifierList(AttributeSpecifierList *obj) {
}

bool CodeGenASTVisitor::visitAttributeSpecifier(AttributeSpecifier *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitAttributeSpecifier(AttributeSpecifier *obj) {
}

bool CodeGenASTVisitor::visitAttribute(Attribute *obj) {
  return true;
}

void CodeGenASTVisitor::postvisitAttribute(Attribute *obj) {
}
