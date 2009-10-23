// see License.txt for copyright and terms of use

#include "alloctool.h"          // this module
#include "alloctool_cmd.h"      // AllocToolCmd
#include "alloctool_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "strutil.h"            // quoted
#include "oink_util.h"

// FIX: The heapify transformation done here has a lot in common with
// the stackness analysis in qual.cc

// FIX: This function has a lot of duplication with
// void MarkVarsStackness_VisitRealVars::visitVariableIdem(Variable*);
//
// Is this variable allocated on the stack?  Note that if it is in a
// class/struct/union we say no as it's container decides it
static bool allocatedOnStack(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  xassert(var->getReal());
  switch(var->getScopeKind()) {
  case NUM_SCOPEKINDS: xfailure("can't happen"); break; // silly gcc warning
  case SK_UNKNOWN:              // not yet registered in a scope
    // FIX: the global scope has ScopeKind SK_UNKNOWN
//     xfailure("got SK_UNKNOWN on a real variable");
    return false;
    break;
  case SK_GLOBAL:               // toplevel names
//     markNonStack(var);
    return false;
    break;
  case SK_PARAMETER:            // parameter list
//     markStack(var);
    return true;
    break;
  case SK_FUNCTION:             // includes local variables
    if (var->hasFlag(DF_STATIC)) {
//       markNonStack(var); // basically it is global
      return false;
    } else {
//       markStack(var);
      return true;
    }
    break;
  case SK_CLASS:                // class member scope
    if (var->hasFlag(DF_STATIC)) {
//       markNonStack(var);        // basically it is global
      return false;
    } else {
      // do not mark as the storage-class depends on the containing
      // class; the containing variable will be reallocated onto the
      // heap if it is on the stack and not if it is not
      return false;           // we don't know that it is on the stack
    }
    break;
  case SK_TEMPLATE_PARAMS: // template paramter list (inside the '<' and '>')
  case SK_TEMPLATE_ARGS: // bound template arguments, during instantiation
    // not code
    xfailure("got template variable as a real variable");
    break;
  case SK_NAMESPACE:            // namespace
    // don't mark as can't contain data
    return false;
    break;
  }
  xfailure("can't happen");
}

static void printLoc(std::ostream &out, SourceLoc loc) {
  out << sourceLocManager->getFile(loc) << ":" <<
    sourceLocManager->getLine(loc) << ": ";
}

// **** AddrTakenASTVisitor

// make a set of variables that have had their addr taken
class AddrTakenASTVisitor : private ASTVisitor {
public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  SObjSet<Variable*> &addrTaken;

public:
  AddrTakenASTVisitor(SObjSet<Variable*> &addrTaken0)
    : loweredVisitor(this)
    , addrTaken(addrTaken0)
  {}
  virtual ~AddrTakenASTVisitor() {}

  // visitor methods
  virtual bool visitPQName(PQName *);
  virtual bool visitExpression(Expression *);

  // utility methods
private:
  // if this expression ultimately resolves to a variable, find it;
  // otherwise return NULL
  void registerUltimateVariable(Expression *);
};

bool AddrTakenASTVisitor::visitPQName(PQName *obj) {
  // from RealVarAndTypeASTVisitor::visitPQName(PQName*): Scott points
  // out that we have to filter out the visitation of PQ_template-s:
  //
  // SGM 2007-08-25: Do not look inside PQ_template argument lists.
  // For template template parameters, the argument list may refer to
  // an uninstantiated template, but client analyses will just treat
  // the whole PQ_template as just a name; no need to look inside it.
  if (obj->isPQ_template()) {
    return false;
  }
  return true;
}

void AddrTakenASTVisitor::registerUltimateVariable(Expression *expr) {
  // Note that we do not do the right thing in the presence of
  // -fo-instance-sensitive, which is why alloctool_cmd.cc refuses to
  // run if you pass that flag.
  expr = expr->skipGroups();
  if (expr->isE_variable()) {
    addrTaken.add(expr->asE_variable()->var);
  } else if (expr->isE_fieldAcc()) {
    E_fieldAcc *efield = expr->asE_fieldAcc();
    Variable *field = efield->field;
    addrTaken.add(field);
    // the address of a field was taken so we consider the address of
    // the whole expression to have been taken:
    //   A a;
    //   &(a.x);
    // that technically takes the address of x but we consider it to
    // also take the address of a
    xassert(field->getScopeKind() == SK_CLASS);
    // static vars aren't actually allocated in their class
    if (!field->hasFlag(DF_STATIC)) {
      registerUltimateVariable(efield->obj);
    }
  } else if (expr->isE_arrow()) {
    xfailure("E_arrow should have been turned into E_fieldAcc "
             "during typechecking");
  } else if (expr->isE_cond()) {
    // gcc complains, but it still lets you do it:
    //   &(argc==1 ? a : b)
    // addr_of_econd.c:6: warning: argument to '&' not really an
    // lvalue; this will be a hard error in the future
    E_cond *econd = expr->asE_cond();
    // because of this non-determinisim, we can't make
    // registerUltimateVariable return a variable and de-couple it from
    // addrTake.add()
    registerUltimateVariable(econd->th);
    registerUltimateVariable(econd->el);
  }
  // if an E_addrOf of an E_deref results in a variable having its
  // address taken, then the variable's address was already taken in
  // the first place somewhere else
}

bool AddrTakenASTVisitor::visitExpression(Expression *obj) {
  if (obj->isE_addrOf()) {
    registerUltimateVariable(obj->asE_addrOf()->expr);
  }
  // FIX: E_funCall and E_constructor and E_throw can take the address
  // of a variable if it is passed to a reference
#warning do other expressions here
  return true;
}

// **** StackAllocVarPredicate

class StackAlloc_VarPredicate : public VarPredicate {
public:
  // require that the variable also be in this set if provided
  SObjSet<Variable*> *varSetFilter;

  explicit StackAlloc_VarPredicate(SObjSet<Variable*> *varSetFilter0=0)
    : varSetFilter(varSetFilter0)
  {}

  virtual bool pass(Variable *var) {
    if (!allocatedOnStack(var)) return false;
    if (!varSetFilter) return true;
    return varSetFilter->contains(var);
  }
};

// **** RealVarAllocAndUseVisitor

// visit real variables at their declarators and use sites
class RealVarAllocAndUseVisitor : private ASTVisitor {
public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  public:
  RealVarAllocAndUseVisitor()
    : loweredVisitor(this)
  {}
  virtual ~RealVarAllocAndUseVisitor() {}

  // visitor methods
  virtual bool visitPQName(PQName *obj);
  virtual bool visitDeclarator(Declarator *);
  virtual bool visitExpression(Expression *);

  // client methods
  virtual bool visit2Declarator(Declarator *) = 0;
  virtual bool visit2E_variable(E_variable *) = 0;
};

bool RealVarAllocAndUseVisitor::visitPQName(PQName *obj) {
  // from RealVarAndTypeASTVisitor::visitPQName(PQName*): Scott points
  // out that we have to filter out the visitation of PQ_template-s:
  //
  // SGM 2007-08-25: Do not look inside PQ_template argument lists.
  // For template template parameters, the argument list may refer to
  // an uninstantiated template, but client analyses will just treat
  // the whole PQ_template as just a name; no need to look inside it.
  if (obj->isPQ_template()) {
    return false;
  }
  return true;
}

bool RealVarAllocAndUseVisitor::visitDeclarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);
  if (var->filteredOut()) return false;
  return visit2Declarator(obj);
}

bool RealVarAllocAndUseVisitor::visitExpression(Expression *obj) {
  if (obj->isE_variable()) {
    return visit2E_variable(obj->asE_variable());
  }
  return true;
}

// **** Pred_RealVarAllocAndUseVisitor

class Pred_RealVarAllocAndUseVisitor : public RealVarAllocAndUseVisitor {
public:
  VarPredicate &varPred;

  Pred_RealVarAllocAndUseVisitor(VarPredicate &varPred0)
    : varPred(varPred0)
  {}
};

// **** Print_RealVarAllocAndUseVisitor

class Print_RealVarAllocAndUseVisitor : public Pred_RealVarAllocAndUseVisitor {
public:
  Print_RealVarAllocAndUseVisitor(VarPredicate &varPred0)
    : Pred_RealVarAllocAndUseVisitor(varPred0)
  {}

  virtual bool visit2Declarator(Declarator *);
  virtual bool visit2E_variable(E_variable *);
};

bool Print_RealVarAllocAndUseVisitor::visit2Declarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);
  if (varPred.pass(var)) {
    printLoc(std::cout, obj->decl->loc);
    std::cout << "decl " << var->name << std::endl;
  }
  return true;
}

bool Print_RealVarAllocAndUseVisitor::visit2E_variable(E_variable *evar) {
  // Note: if you compile without locations for expressions this will
  // stop working.
  Variable_O *var = asVariable_O(evar->var);
  if (varPred.pass(var)) {
    printLoc(std::cout, evar->loc);
    std::cout << "use " << var->name << std::endl;
  }
  return true;
}

// **** AllocTool

// if it is a parameter we are going to have a hard time
// auto-heapifying it; we should only do it for those on the stack and
// give an error for anything else
#warning cant heapify non-auto (such as params)

void AllocTool::printStackAlloc_stage() {
  printStage("print stack-allocated vars");
  // print the locations of declarators and uses of stack variables
  StackAlloc_VarPredicate sa_varPred;
  Print_RealVarAllocAndUseVisitor env(sa_varPred);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    printStart(file->name.c_str());
    TranslationUnit *unit = file2unit.get(file);
    unit->traverse(env.loweredVisitor);
    printStop();
  }
}

void AllocTool::printStackAllocAddrTaken_stage() {
  printStage("print stack-allocated addr-taken vars");
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    printStart(file->name.c_str());
    TranslationUnit *unit = file2unit.get(file);

    // optimization: while a variable in one translation unit may have
    // its address taken in another, this cannot happen to
    // stack-allocated variables; if you wanted to find all variables
    // that had their address taken, you would need the linker
    // imitator
    SObjSet<Variable*> addrTaken;
    AddrTakenASTVisitor at_env(addrTaken);
    unit->traverse(at_env.loweredVisitor);

    StackAlloc_VarPredicate sa_varPred(&addrTaken);
    Print_RealVarAllocAndUseVisitor env(sa_varPred);
    unit->traverse(env.loweredVisitor);

    printStop();
  }
}
