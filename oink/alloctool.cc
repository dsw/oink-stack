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

//**** StackAllocAddrTaken

// Stackness analysis classes in qual.cc:
//   class StacknessMarker {
//   class MarkStackness_ValueVisitor : public ValueVisitor {
//   class MarkVarsStackness_VisitRealVars : public VisitRealVars_filter {
//   class MarkAllocStackness_Visitor : private ASTVisitor {

// print out declarations of stack allocated variables that have their
// address taken; FIX: right now only prints stack allocated
class StackAllocAddrTaken : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  public:
  StackAllocAddrTaken()
    : loweredVisitor(this)
  {}
  virtual ~StackAllocAddrTaken() {}

  virtual bool visitPQName(PQName *obj);
  virtual bool visitDeclarator(Declarator *);
  virtual bool visitExpression(Expression *);
};

bool StackAllocAddrTaken::visitPQName(PQName *obj) {
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

bool StackAllocAddrTaken::visitDeclarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);
  if (var->filteredOut()) return false;
  if (allocatedOnStack(var)) {
    printLoc(std::cout, obj->decl->loc);
    std::cout << "decl " << var->name << std::endl;
  }
  return true;
}

bool StackAllocAddrTaken::visitExpression(Expression *obj) {
  if (obj->isE_variable()) {
    // Note: if you compile without locations for expressions this
    // will stop working.
    Variable_O *var = asVariable_O(obj->asE_variable()->var);
    if (allocatedOnStack(var)) {
      printLoc(std::cout, obj->loc);
      std::cout << "use " << var->name << std::endl;
    }
  }
  return true;
}

// **** AllocTool

void AllocTool::printStackAllocAddrTaken_stage() {
  printStage("print stack-alloc vars that have their addr taken");
  // print the locations of declarators and uses of stack variables
  StackAllocAddrTaken env;
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    printStart(file->name.c_str());
    TranslationUnit *unit = file2unit.get(file);
    unit->traverse(env.loweredVisitor);
    printStop();
  }
}
