// see License.txt for copyright and terms of use

#include "alloctool.h"          // this module
#include "alloctool_cmd.h"      // AllocToolCmd
#include "alloctool_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "strutil.h"            // quoted
#include "oink_util.h"
#include "Pork/patcher.h"       // Patcher

// FIX:
//
// If a stack-allocated variable is a parameter we are going to have a
// hard time auto-heapifying it; just print it out.
//
// In C++ E_funCall, E_constructor, and template instantiation can
// take the address of a variable if they take arguments by reference;
// further E_throw can take the address of a variable if it is caught
// by reference.

// FIX: The heapify transformation done here has a lot in common with
// the stackness analysis in qual.cc; we should be using that to
// confirm our transformations here.

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
    if (var->getScopeKind() == SK_PARAMETER) {
      printLoc(std::cout, obj->decl->loc);
      std::cout << "param decl " << var->name << std::endl;
    } else if (var->getScopeKind() == SK_FUNCTION) {
      printLoc(std::cout, obj->decl->loc);
      std::cout << "auto decl " << var->name << std::endl;
    } else xfailure("non-param non-auto var can't be stack allocated");
  }
  return true;
}

bool Print_RealVarAllocAndUseVisitor::visit2E_variable(E_variable *evar) {
  // Note: if you compile without locations for expressions this will
  // stop working.
  Variable_O *var = asVariable_O(evar->var);
  if (varPred.pass(var)) {
    if (var->getScopeKind() == SK_PARAMETER) {
      printLoc(std::cout, evar->loc);
      std::cout << "param use " << var->name << std::endl;
    } else if (var->getScopeKind() == SK_FUNCTION) {
      printLoc(std::cout, evar->loc);
      std::cout << "auto use " << var->name << std::endl;
    } else xfailure("non-param non-auto var can't be stack allocated");
  }
  return true;
}

// **** Heapify_RealVarAllocAndUseVisitor

class Heapify_RealVarAllocAndUseVisitor
  : public Pred_RealVarAllocAndUseVisitor
{
public:
  // NOTE: this emits the diff in its dtor
  Patcher patcher;

  Heapify_RealVarAllocAndUseVisitor(VarPredicate &varPred0)
    : Pred_RealVarAllocAndUseVisitor(varPred0)
  {}

  virtual bool visit2Declarator(Declarator *);
  virtual bool visit2E_variable(E_variable *);
};

// find the D_name nested down within an IDeclarator
static D_name *find_D_name(IDeclarator *decl) {
  if (false) {                  // orthogonality

  // "x" (NULL means abstract declarator or anonymous parameter);
  // this is used for ctors and dtors as well as ordinary names
  // (dtor names start with "~"); it's also used for operator names
//   -> D_name(PQName /*nullable*/ name);
  } else if (decl->isD_name()) {
    return decl->asD_name();

  // "*x" (as in "int *x")
//   -> D_pointer(CVFlags cv,  // optional qualifiers applied to ptr type
//                IDeclarator base);
  } else if (decl->isD_pointer()) {
    return find_D_name(decl->asD_pointer()->base);

  // "&x"
//   -> D_reference(IDeclarator base);
  } else if (decl->isD_reference()) {
    return find_D_name(decl->asD_pointer()->base);

  // "f(int)"
//   -> D_func(IDeclarator base,                       // D_name of function, typically
//             FakeList<ASTTypeId> *params,            // params with optional default values
//             CVFlags cv,                             // optional "const" for member functions
//             ExceptionSpec /*nullable*/ exnSpec);    // throwable exceptions
  } else if (decl->isD_func()) {
    return find_D_name(decl->asD_pointer()->base);

  // "a[5]" or "b[]"
//   -> D_array(IDeclarator base, Expression /*nullable*/ size);
  } else if (decl->isD_array()) {
    return find_D_name(decl->asD_pointer()->base);

  // "c : 2"
  //
  // I use a PQName here instead of a StringRef for uniformity
  // (so every IDeclarator ends with a PQName); there are never
  // qualifiers on a bitfield name
//   -> D_bitfield(PQName /*nullable*/ name, Expression bits);
  } else if (decl->isD_bitfield()) {
    return NULL;

  // "X::*p"
//   -> D_ptrToMember(PQName nestedName, CVFlags cv, IDeclarator base);
  } else if (decl->isD_ptrToMember()) {
    return find_D_name(decl->asD_pointer()->base);

  // declarator grouping operator: it's semantically irrelevant
  // (i.e. equivalent to just 'base' alone), but plays a role in
  // disambiguation
//   -> D_grouping(IDeclarator base);
  } else if (decl->isD_grouping()) {
    return find_D_name(decl->asD_pointer()->base);

  } else xfailure("can't happen");
}

bool Heapify_RealVarAllocAndUseVisitor::visit2Declarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);
  if (varPred.pass(var)) {
    if (var->getScopeKind() == SK_PARAMETER) {
      printLoc(std::cout, obj->decl->loc);
      std::cout << "param decl " << var->name << std::endl;
    } else if (var->getScopeKind() == SK_FUNCTION) {
//       printLoc(std::cout, obj->decl->loc);
//       std::cout << "auto decl " << var->name << std::endl;
      // find the D_name
      D_name *dname = find_D_name(obj->decl);
      // it shouldn't be possible to take the address of a bitfield
      // and that's the only way this can fail
      xassert(dname);
      CPPSourceLoc ppLoc(dname->loc);
      CPPSourceLoc ppLoc_end(dname->endloc);
      if (!(ppLoc_end.hasExactPosition() && ppLoc_end.hasExactPosition())) {
        printLoc(std::cout, obj->decl->loc);
        std::cout << "auto decl does not have exact position"
                  << var->name << std::endl;
        return true;
      }
      patcher.insertBefore(ppLoc, "(*");
      patcher.insertBefore(ppLoc_end, ")");
    } else xfailure("non-param non-auto var can't be stack allocated");
  }
  return true;
}

bool Heapify_RealVarAllocAndUseVisitor::visit2E_variable(E_variable *evar) {
  // Note: if you compile without locations for expressions this will
  // stop working.
  Variable_O *var = asVariable_O(evar->var);
  if (varPred.pass(var)) {
    if (var->getScopeKind() == SK_PARAMETER) {
      printLoc(std::cout, evar->loc);
      std::cout << "param use " << var->name << std::endl;
    } else if (var->getScopeKind() == SK_FUNCTION) {
//       printLoc(std::cout, evar->loc);
//       std::cout << "auto use " << var->name << std::endl;
      CPPSourceLoc ppLoc(evar->loc);
      patcher.insertBefore(ppLoc, "\n// xform this use\n");
    } else xfailure("non-param non-auto var can't be stack allocated");
  }
  return true;
}

// **** AllocTool

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

void AllocTool::heapifyStackAllocAddrTaken_stage() {
  printStage("heapify stack-allocated addr-taken vars");
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    // FIX: this doesn't work for C++ yet
    if (globalLang.isCplusplus) {
      throw UserError(USER_ERROR_ExitCode,
                      "Can't heapify C++ with alloctool yet.");
    }
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
    Heapify_RealVarAllocAndUseVisitor env(sa_varPred);
    unit->traverse(env.loweredVisitor);

    printStop();
    // NOTE: the Heapify_RealVarAllocAndUseVisitor will be dtored
    // after this so anything we print above will be delimited from
    // the patch
  }
}
