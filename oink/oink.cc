// see License.txt for copyright and terms of use

#include "oink.h"               // this module
#include "oink_global.h"        // FIX: I think this is circular

#include "trace.h"              // tracingSys, traceAddSys
#include "parssppt.h"           // ParseTreeAndTokens, treeMain
#include "srcloc.h"             // sourceFileList (r), SourceLocManager
#include "ckheap.h"             // malloc_stats
#include "oink_tcheck_env.h"    // TcheckEnv
#include "str.h"                // stringBuilder
#include "strutil.h"            // plural
#include "cc_lang.h"            // CCLang
#include "parsetables.h"        // ParseTables
#include "cc_print.h"           // PrintEnv
#include "oink_cmd.h"           // OinkCmd
#include "cc_elaborate.h"       // ElabVisitor
#include "oink_integrity.h"     // IntegrityVisitor
#include "ptreenode.h"          // PTreeNode
#include "ptreeact.h"           // ParseTreeLexer, ParseTreeActions

#include "value_ast_visitor.h"  // ValueASTVisitor

#include "dataflow_visitor.h"   // DataFlowVisitor
#include "dataflow_ex.h"        // DataFlowEx

#include "xml_file_writer.h"    // XmlFileWriter
#include "xml_type_writer.h"    // XmlTypeWriter
#include "xml_value_writer.h"   // XmlValueWriter

#include "xml_lexer.h"          // XmlLexer
#include "xml_reader.h"         // XmlReader
#include "xml_file_reader.h"    // XmlFileReader
#include "xml_type_reader.h"    // XmlTypeReader
#include "xml_value_reader.h"   // XmlValueReader

#include "archive_srz.h"        // ArchiveSerializer/ArchiveDeserializer
#include "archive_srz_format.h"

#include "oink_util.h"

#include <iostream>             // cout
#include <cstdlib>              // exit
#include <cstring>              // strcmp, streq

// FIX: dsw: g++ can't find this on my machine
// #include <endian.h>             // BYTE_ORDER

// FIX: this is a duplicate declaration from oink_global.h
extern ValueTypePrinter typePrinterOink;

// currently, we only support one format version at a time, but at least this
// detects format changes before it burns us later.
//
// for grep: SERIALIZATION_VERSION
static std::string const SERIALIZATION_FORMAT = "v17";

const size_t LINK_ERROR_MAX_SYMBOLS_REPORTED = 5;

// **** utilities

void printLoc(SourceLoc loc, std::ostream &out) {
  out << sourceLocManager->getFile(loc) << ":" <<
    sourceLocManager->getLine(loc) << ": ";
}

void printStart(char const *name) {
  if (oinkCmd->print_startstop) {
    std::cout << std::endl;
    std::cout << "---- START ---- " << name << std::endl;
  }
}

void printStop() {
  if (oinkCmd->print_startstop) {
    std::cout << "---- STOP ----" << std::endl;
    std::cout << std::endl;
  }
}


// **** class to module map

// visit all of the class/struct/union definitions and map each to a
// module
class ModuleClassDef_Visitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  // map fully qualified names of classes to their modules
  StringRefMap<char const> &classFQName2Module;
  // list of class typedef variables
  SObjList<Variable_O> &classVars;

  public:
  ModuleClassDef_Visitor
  (StringRefMap<char const> &classFQName2Module0,
   SObjList<Variable_O> &classVars0)
    : loweredVisitor(this)
    , classFQName2Module(classFQName2Module0)
    , classVars(classVars0)
  {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  // print out the class name to modules map
  void print_class2mod(std::ostream &out = std::cout);

  virtual void postvisitTypeSpecifier(TypeSpecifier *);
  virtual void subPostVisitTS_classSpec(TS_classSpec *);
};

void ModuleClassDef_Visitor::print_class2mod(std::ostream &out) {
  out << "---- START class to module map" << std::endl;
  SFOREACH_OBJLIST(Variable_O, classVars, iter) {
    Variable_O const *typedefVar = iter.data();
    StringRef fqname = globalStrTable
      (typedefVar->fullyQualifiedMangledName0().c_str());
    StringRef module = classFQName2Module.get(fqname);
    out << fqname << " " << module << std::endl;
  }
  out << "---- END class to module map" << std::endl;
}

void ModuleClassDef_Visitor::postvisitTypeSpecifier(TypeSpecifier *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case TypeSpecifier::TS_CLASSSPEC:
    subPostVisitTS_classSpec(obj->asTS_classSpec());
    break;
  }
}

void ModuleClassDef_Visitor::subPostVisitTS_classSpec(TS_classSpec *obj) {
//   std::cout << std::endl;
//   std::cout << "ModuleClassDef_Visitor::visitTS_classSpec" << std::endl;

  StringRef module = moduleForLoc(obj->loc);
  Variable_O * const typedefVar = asVariable_O(obj->ctype->typedefVar);
  StringRef lookedUpModule = classFQName2Module.get
    (globalStrTable(typedefVar->fullyQualifiedMangledName0().c_str()));
  if (lookedUpModule) {
    // if already in the map, check they agree
    if (module != lookedUpModule) {
      userFatalError(obj->loc, "class %s maps to two modules %s and %s",
                     typedefVar->fullyQualifiedMangledName0().c_str(),
                     module, lookedUpModule);
    }
  } else {
    // otherwise, insert it
//     std::cout << "**** mapping class "
//               << typedefVar->fullyQualifiedMangledName0()
//               << " to module " << module
//               << std::endl;
//     std::cout << std::endl;

    // FIX: dealing with superclasses is messy, so for now we just
    // throw an unimplemented exception if you have any; we should be
    // ensuring that all superclasses map to the same module; note
    // that in C/C++ we must have seen our superclasses by now so we
    // could just check that they have the same module; however not
    // sure what the right thing to do for virtual inheritance is
    SObjList<BaseClassSubobj const> objs;
    obj->ctype->getSubobjects(objs);
    SFOREACH_OBJLIST(BaseClassSubobj const, objs, iter) {
      CompoundType *base = iter.data()->ct;
      if (base == obj->ctype) { continue; }
      userFatalError(obj->loc,
                     "unimplemented (map from class to module): "
                     "class %s has superclasses",
                     typedefVar->fullyQualifiedMangledName0().c_str());
    }

    classFQName2Module.add
      (globalStrTable(typedefVar->fullyQualifiedMangledName0().c_str()),
       module);
    // NOTE: don't use append!
    classVars.prepend(typedefVar);
  }
}

void Oink::build_classFQName2Module() {
  xassert(!classFQName2Module);
  classFQName2Module = new StringRefMap<char const>();
  xassert(!classVars);
  classVars = new SObjList<Variable_O>();
  ModuleClassDef_Visitor classDefEnv(*classFQName2Module, *classVars);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    unit->traverse(classDefEnv.loweredVisitor);
  }
  if (oinkCmd->module_print_class2mod) {
    classDefEnv.print_class2mod();
  }
}

// **** AllocSites_ASTVisitor

bool AllocSites_ASTVisitor::visitExpression(Expression *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case Expression::E_FUNCALL: {
    E_funCall *efun = obj->asE_funCall();
    if (isAllocator0(efun, efun->loc)) seenAllocators.add(efun);
    break;
  }
  case Expression::E_KEYWORDCAST: {
    E_keywordCast *ekwcast = obj->asE_keywordCast();
    subVisitCast(ekwcast, ekwcast->expr);
    break;
  }
  case Expression::E_CAST: {
    E_cast *ecast = obj->asE_cast();
    subVisitCast(ecast, ecast->expr);
    break;
  }
  case Expression::E_NEW: {
    USER_ASSERT(obj->type->isPointerType(), obj->loc,
                "new expression is not of pointer type");
    subVisitE_new0(obj->asE_new());
    break;
  }
  }
  return true;
}

void AllocSites_ASTVisitor::subVisitCast(Expression *cast, Expression *expr) {
  expr = expr->skipGroups();
  if (!expr->isE_funCall()) return;
  E_funCall *efun = expr->asE_funCall();
  if (isAllocator0(efun, cast->loc)) {
    USER_ASSERT(cast->type->isPointerType(), cast->loc,
                "cast from allocator is not of pointer type");
    castedAllocators.add(efun);
    subVisitCast0(cast, expr);
  }
}

bool AllocSites_ASTVisitor::checkAlloc_seenImpliesCasted() {
  bool ret = true;
  for(SObjSetIter<E_funCall*> seenIter(seenAllocators);
      !seenIter.isDone(); seenIter.adv()) {
    E_funCall *call0 = seenIter.data();
    if (!castedAllocators.contains(call0)) {
      ret = false;
      userReportWarning
        (call0->loc, "allocator that was not inside a cast expression");
    }
  }
  return ret;
}

// **** find/print attributes visitors

void printAttributeSpecifierList(AttributeSpecifierList *alist) {
  // Note: this traversal initially copied verbaitm from
  // D_attribute::tcheck()
  for (AttributeSpecifierList *l = alist; l; l = l->next) {
    for (AttributeSpecifier *s = l->spec; s; s = s->next) {
      if (s->attr->isAT_empty()) {
//         AT_empty *atE = s->attr->asAT_empty();
        // nothing to do; this is just here to be complete
      } else if (s->attr->isAT_word()) {
        AT_word *atW = s->attr->asAT_word();
        std::cout << atW->w;
      } else if (s->attr->isAT_func()) {
        AT_func *atF = s->attr->asAT_func();
        std::cout << atF->f << "(";
        bool first = true;
        FAKELIST_FOREACH_NC(ArgExpression, atF->args, argExp) {
          if (first) first = false;
          else std::cout << ", ";
          std::cout << prettyPrintASTNode(argExp->expr);
        }
        std::cout << ")";
      }
    }
  }
}

void printAttribute(D_attribute *dattr) {
  std::cout << "__attribute__((";
  printAttributeSpecifierList(dattr->alist);
  std::cout << "))";
}

// Find and print out all of the declarators that have GNU Attribute
// Specifier Lists; print the var and the attr list.
class PrintAttributes_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  SObjSet<D_attribute*> &dattrSeen;

  PrintAttributes_ASTVisitor(SObjSet<D_attribute*> &dattrSeen0)
    : loweredVisitor(this)
    , dattrSeen(dattrSeen0)
  {}

  virtual bool visitDeclarator(Declarator *);
};

bool PrintAttributes_ASTVisitor::visitDeclarator(Declarator *obj) {
  IDeclarator *idecl = obj->decl;
  if (idecl->isD_grouping()) {
    // there is no "isD_attribute()"
    if (D_attribute *dattr = dynamic_cast<D_attribute*>(idecl)) {
      printLoc(dattr->loc);
      std::cout << obj->var->name << " ";
      printAttribute(dattr);
      std::cout << std::endl;
      dattrSeen.add(dattr);
    }
  }
  return true;
}

class FindUnseenAttributes_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  SObjSet<D_attribute*> &dattrSeen;
  bool foundOne;

  FindUnseenAttributes_ASTVisitor(SObjSet<D_attribute*> &dattrSeen0)
    : loweredVisitor(this)
    , dattrSeen(dattrSeen0)
    , foundOne(false)
  {}

  virtual bool visitIDeclarator(IDeclarator *);
};

bool FindUnseenAttributes_ASTVisitor::visitIDeclarator(IDeclarator *obj) {
  if (obj->isD_grouping()) {
    // there is no "isD_attribute()"
    if (D_attribute *dattr = dynamic_cast<D_attribute*>(obj)) {
      if (!dattrSeen.contains(dattr)) {
        printLoc(dattr->loc);
        printAttribute(dattr);
      }
    }
  }
  return true;
}

void Oink::printFuncAttrs_stage() {
  printStage("find attributes");
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    SObjSet<D_attribute*> dattrSeen;

    // print out all of the functions having attributes
    PrintAttributes_ASTVisitor vis(dattrSeen);
    printStart(file->name.c_str());
    unit->traverse(vis.loweredVisitor);
    printStop();

    // print out any we missed; there shouldn't be any
    FindUnseenAttributes_ASTVisitor findVis(dattrSeen);
    unit->traverse(findVis.loweredVisitor);
    if (findVis.foundOne) {
      throw UserError(INTERNALERROR_ExitCode,
                      "Found some attributes not on functions.");
    }
  }
}

// **** FuncGranGraph

void FuncGranGraph::addEdge(Variable *from, Variable *to) {
  xassert(from);
  xassert(to);
  SObjSet<Variable*> *toSet = flowsTo.get(from);
  if (!toSet) {
    toSet = new SObjSet<Variable*>();
    flowsTo.add(from, toSet);
  }
  toSet->add(to);
}

void FuncGranGraph::build_flowsFrom() {
  xassert(!flowsFrom);
  flowsFrom = new PtrMap<Variable, SObjSet<Variable*> >();

  // for each edge in flowsTo
  for(PtrMap<Variable,
        SObjSet<Variable*> >::Iter mapIter(flowsTo);
      !mapIter.isDone(); mapIter.adv())
  {
    Variable *from = mapIter.key();
    SObjSet<Variable*> *toSet = mapIter.value();
    for(SObjSetIter<Variable*> toSetIter(*toSet);
        !toSetIter.isDone(); toSetIter.adv())
    {
      Variable *to = toSetIter.data();

      // add edge in flowsFrom from 'to' to 'from'
      SObjSet<Variable*> *fromSet = flowsFrom->get(to);
      if (!fromSet) {
        fromSet = new SObjSet<Variable*>();
        flowsFrom->add(to, fromSet);
      }
      fromSet->add(from);
    }
  }
}

void FuncGranGraph::build_module2ToVars() {
  xassert(!module2ToVars);
  module2ToVars = new PtrMap<char const, SObjSet<Variable*> >();

  // for each to-var in flowsFrom
  for(PtrMap<Variable,
        SObjSet<Variable*> >::Iter mapIter(*flowsFrom);
      !mapIter.isDone(); mapIter.adv())
  {
    Variable *to = mapIter.key();
    // map its module to it in module2ToVars
    StringRef module = moduleForLoc(to->loc);
    SObjSet<Variable*> *varSet = module2ToVars->get(module);
    if (!varSet) {
      varSet = new SObjSet<Variable*>();
      module2ToVars->add(module, varSet);
    }
    varSet->add(to);
  }
}


// **** FuncGranASTVisitor

void FuncGranASTVisitor::dependOnCurrentFunc(Variable *to) {
  // the current Function reaches this use of a Variable
  if (!to) return;
  xassert(to->getReal());
  // only record Variables of function type
  if (! (to->type && to->type->asRval()->isFunctionType()) ) return;

  // // quarl 2006-07-12
  // //    omit __builtin_va_start, __builtin_va_arg, __builtin_va_end
  // if (streq(to->name, "__builtin_va_start") ||
  //     streq(to->name, "__builtin_va_arg") ||
  //     streq(to->name, "__builtin_va_end"))
  //   return;

  Variable *from = NULL;
  if (functionStack.isNotEmpty()) {
    from = functionStack.top()->nameAndParams->var;
  } else {
    from = funcGranGraph.getRoot();
  }
  xassert(from);
  xassert(from->getReal());

  funcGranGraph.addEdge(from, to);
}

void FuncGranASTVisitor::dependOnSuperMain(Variable *to) {
  // the current Function reaches this use of a Variable
  xassert(to);
  xassert(to->getReal());
  // only record Variables of function type
  if (! (to->type && to->type->asRval()->isFunctionType()) ) return;

  Variable *from = funcGranGraph.getRoot();
  xassert(from);
  xassert(from->getReal());

  funcGranGraph.addEdge(from, to);
}

void FuncGranASTVisitor::visitArgumentList(FakeList<ArgExpression> *args) {
  FAKELIST_FOREACH_NC(ArgExpression, args, iter) {
    iter->traverse(this->loweredVisitor);
  }
}

bool FuncGranASTVisitor::visitExpression(Expression *obj) {
  if (obj->isE_funCall()) {
    // we need to see if this is a "direct" function call, in which
    // case we just depend on the top of the current function stack;
    // we prune the walk so that the recursive decent does not go down
    // into the E_variable or E_fieldAcc and make a dependency on
    // super-main
    E_funCall *ef = obj->asE_funCall();
    if (ef->func->skipGroups()->isE_variable()) {
      dependOnCurrentFunc(ef->func->skipGroups()->asE_variable()->var);
      visitArgumentList(ef->args);
      return false;             // prune the walk
    } else if (ef->func->skipGroups()->isE_fieldAcc()) {
      dependOnCurrentFunc(ef->func->skipGroups()->asE_fieldAcc()->field);
      visitArgumentList(ef->args);
      return false;             // prune the walk
    }
  } else if (obj->isE_constructor()) {
    dependOnCurrentFunc(obj->asE_constructor()->ctorVar);
  } else if (obj->isE_new()) {
    dependOnCurrentFunc(obj->asE_new()->ctorVar);
  }

  // these are different: we cannot be sure that it is not a function
  // with its address being taken
  else if (obj->isE_variable()) {
    dependOnSuperMain(obj->asE_variable()->var);
  } else if (obj->isE_fieldAcc()) {
    dependOnSuperMain(obj->asE_fieldAcc()->field);
  }
  return true;
}

bool FuncGranASTVisitor::visitInitializer(Initializer *obj) {
  // FIX: I think an IN_expr can act as an IN_ctor, but there is no
  // ctorVar for a constructor there to call; as soon as there is, we
  // need to put a dependency here.
  //
  // FIX: IN_compound can call constructors also in C++
  if (obj->isIN_ctor()) {
    dependOnCurrentFunc(obj->asIN_ctor()->ctorVar);
  }
  return true;
}

bool FuncGranASTVisitor::visitMemberInit(MemberInit *obj) {
  dependOnCurrentFunc(obj->ctorVar);
  return true;
}

bool FuncGranASTVisitor::visitFunction_once_initial(Function *obj) {
  Variable *var = obj->nameAndParams->var;
  static StringRef mainName = globalStrTable("main");
  if (var->name == mainName &&
      var->type && var->type->isFunctionType() &&
      var->scope && var->scope->isGlobalScope()) {
    dependOnSuperMain(var);
  }
  return true;
}

// NOTE: modified from DataFlowVisitor::visitFunction(); FIX: this
// should probably be abstracted out into a class that both
// DataFlowVisitor and FuncGranASTVisitor inherit from.
bool FuncGranASTVisitor::visitFunction(Function *obj) {
  xassert(!obj->instButNotTchecked());
  TemplateInfo *tinfo = obj->nameAndParams->var->templateInfo();
  if (tinfo) {
    xassert(tinfo->isCompleteSpecOrInstantiation());
  }

  if (!visitFunction_once_initial(obj)) return false; // prune

  // ignore this function if we should
  Variable_O *napVar = asVariable_O(obj->nameAndParams->var);
//   xassert(tunit == napVar->tunit);
  if (controls && controls->isIgnored(napVar)) {
    std::cout << sourceLocManager->getString(napVar->loc)
              << " ignoring function body since in ignore file: "
              << napVar->name
              << std::endl;
    return false;               // prune
  }

  // Run a sub traverse here on every field of Function and then
  // return false to prune the whole walk.
  obj->retspec->traverse(this->loweredVisitor);
  obj->nameAndParams->traverse(this->loweredVisitor);

  if (!visitFunction_once_decl(obj)) return false; // prune

  // Not sure that this is the right place.  Perhaps it should include
  // the initializer list and the exception handlers.
  // 7sept2003: yes, it should
  functionStack.push(obj);
  if (!visitFunction_once_body(obj)) return false; // prune

  // Visit the 'greater' function body
  FAKELIST_FOREACH_NC(MemberInit, obj->inits, iter) {
    iter->traverse(this->loweredVisitor);
  }
  obj->body->traverse(this->loweredVisitor);
  FAKELIST_FOREACH_NC(Handler, obj->handlers, iter) {
    iter->traverse(this->loweredVisitor);
  }
  // if it is a dtor, visit the implicit superclass and member dtors
  if (obj->dtorStatement) {
    obj->dtorStatement->traverse(this->loweredVisitor);
  }

  functionStack.pop();

  // PRUNE, because we handled all the children above in the sub walk
  return false;
}


// **** FuncGranVarVisitor

// NOTE: do NOT inherit from VisitRealVars_filter
class FuncGranVarVisitor : public VisitRealVars {
  FuncGranGraph &funcGranGraph;

  public:
  explicit FuncGranVarVisitor(FuncGranGraph &funcGranGraph0)
    : VisitRealVars(NULL)
    , funcGranGraph(funcGranGraph0)
  {}
  // only visits each Variable once
  virtual void visitVariableIdem(Variable *var);
};

void FuncGranVarVisitor::visitVariableIdem(Variable *var) {
  if (var->virtuallyOverride) {
    xassert(var->type && var->type->isMethod() && var->hasFlag(DF_VIRTUAL));
    for(SObjSetIter<Variable*> iter(*var->virtuallyOverride);
        !iter.isDone();
        iter.adv()) {
      Variable *elt = iter.data();
      // NOTE: We do not unify, but only make a dependency in one
      // direction, from up to down
      funcGranGraph.addEdge(elt, var);
    }
  }
}

// **** filtered visitation

bool RealVarAndTypeASTVisitor_filtered::visitDeclarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);
  if (var->filteredOut()) return false;
  // delegate to superclass
  return RealVarAndTypeASTVisitor::visitDeclarator(obj);
}

bool RealVarAndTypeASTVisitor_filtered::visitFunction(Function *obj) {
  Variable_O *var = asVariable_O(obj->nameAndParams->var);
  if (var->filteredOut()) return false;
  // delegate to superclass
  return RealVarAndTypeASTVisitor::visitFunction(obj);
}

void visitVarsMarkedRealF_filtered
  (ArrayStack<Variable*> &builtinVars, VisitRealVars &visitReal)
{
  FOREACH_ARRAYSTACK_NC(Variable*, builtinVars, iter) {
    Variable_O *var = asVariable_O(*iter.data());
    if (!var->getReal()) continue;
    if (var->filteredOut()) continue;
    visitReal.visitVariable(var);
  }
}

void visitRealVarsF_filtered(TranslationUnit *tunit, VisitRealVars &visitReal) {
  RealVarAndTypeASTVisitor_filtered vis(&visitReal, &visitReal.doNothing_tv);
  tunit->traverse(vis.loweredVisitor);
}

// **** Oink

Oink::Oink()
  : parseUserActions(NULL)
  , parseEnv(NULL)
  // , parseTables(NULL)
  , typePrinter(NULL)
  , funcGranRoot(tFac->makeVariable(SL_UNKNOWN, "<super-main>",
                                    NULL, DF_NAMESPACE))
  , funcGranGraph(funcGranRoot)
  , archiveSrzManager(NULL)

  , classFQName2Module(NULL)
  , classVars(NULL)
{
  // FIX: it would be much more elegant to somehow do this during
  // variable construction
  funcGranRoot->setReal(true);
}

Oink::~Oink() {}

void Oink::printStage(const char *name) {
  if (oinkCmd->print_stages)
    printStage0(name);
}

void Oink::printStage0(char const *name) {
  std::cout << "%%" << explain_process_stats << getBackendName()
            << " stage: " << name << std::endl;
}

void Oink::printBuckets() {
  CVAtomicValue::dumpBuckets();
}

// virtual
void Oink::printStats()
{
  std::cout << "Variables: " << Variable::numVariables << std::endl;
  std::cout << "Values: " << vFac->getNumValues() << std::endl;
#if DEBUG_INSTANCE_SPECIFIC_VALUES
  std::cout << "  dataDeclaratorValues: "
            << dataDeclaratorValues.getNumEntries() << std::endl;
  std::cout << "  instanceSpecificValues: "
            << instanceSpecificValues.getNumEntries() << std::endl;
#endif
}

// virtual
void Oink::printSizes()
{
#define P(T) std::cout << "  sizeof(" #T ") = " << sizeof(T) << std::endl

  std::cout << "Elsa:\n";

  P(Variable);

  std::cout << std::endl;
  std::cout << "Oink:\n";

  P(CVAtomicValue);
  P(PointerValue);
  P(ReferenceValue);
  P(FunctionValue);
  P(ArrayValue);
  P(PointerToMemberValue);

#undef P
}

// virtual
void Oink::printProcStats()
{
  cat_proc_status(std::cout);
}

bool Oink::isSource(File const *file) {
  InputLang tgtLang = oinkCmd->lang;
  if (oinkCmd->lang == SUFFIX_InputLang) {
    tgtLang = getLangFromSuffix(getSuffix(file->name.c_str()));
  }
  return tgtLang != UNKNOWN_InputLang;
}

bool Oink::isSerialized(File const *file) {
  return archiveSrzManager->isArchiveName(file->name.c_str());
}

void Oink::maybeSetInputLangFromSuffix(File *file) {
  if (oinkCmd->lang == SUFFIX_InputLang) {
    InputLang tgtLang = getLangFromSuffix(getSuffix(file->name.c_str()));
    if (tgtLang != UNKNOWN_InputLang) {
      setLangState_fromInputLang(globalLang, tgtLang);
    }
  }

  // FIX: this is a bit perverse; get Scott to rename the flags
  if (oinkCmd->do_overload && globalLang.allowOverloading) {
    if (tracingSys("doNotOverload")) {
      traceRemoveSys("doNotOverload"); // double negative
    }
  } else {
    traceAddSys("doNotOverload");
  }
  if (oinkCmd->do_op_overload && globalLang.allowOverloading) {
    if (tracingSys("doNotOperatorOverload")) {
      traceRemoveSys("doNotOperatorOverload"); // double negative
    }
  } else {
    traceAddSys("doNotOperatorOverload");
  }

  if (globalLang.isCplusplus) {
    anyCplusplus = true;
  }
}

void Oink::loadControlFile() {
  xassert(!oinkCmd->control.empty());
  std::ifstream in0(oinkCmd->control.c_str());
  if (!in0) {
    userFatalError(SL_UNKNOWN, "Couldn't open control file '%s' for reading",
                   oinkCmd->control.c_str());
  }
  ControlStream in(in0);
  try {
    controls = new Controls();
    controls->load(in);
  } catch (xBase &x) {
    x.addContextLeft(stringc << oinkCmd->control.c_str()
                     << ":" << in.lineCount << ": ");
    throw x;
  }
  if (oinkCmd->print_controls_and_exit) {
    controls->print(std::cout);
    exit(0);
  }
}

void Oink::parseOneFile(File *file, ParseTables *parseTables) {
  maybeSetInputLangFromSuffix(file);
  if (oinkCmd->lang == SUFFIX_InputLang) {
    setLangState_fromInputLang
      (globalLang, getLangFromSuffix(getSuffix(file->name.c_str())));
  }
  try {
    // sm: I need to pass the source file's name up here, but I'm
    // not testing whether argc>0 ..
    SemanticValue treeTop;
    ParseTreeAndTokens tree(globalLang, treeTop,
                            globalStrTable, file->name.c_str());
    Lexer *lexer = dynamic_cast<Lexer*>(tree.lexer);  // hack..
    xassert(lexer);
    tree.userAct = parseUserActions;
    tree.tables = parseTables;
    maybeUseTrivialActions(tree);

    if (tracingSys("parseTree")) {
      // make some helpful aliases
      LexerInterface *underLexer = tree.lexer;
      UserActions *underAct = parseUserActions;

      // replace the lexer and parser with parse-tree-building versions
      tree.lexer = new ParseTreeLexer(underLexer, underAct);
      tree.userAct = new ParseTreeActions(underAct, parseTables);

      // FIX: ??
      // 'underLexer' and 'tree.userAct' will be leaked.. oh well
    }

    if (!toplevelParse(tree, file->name.c_str()))
      exit(LEX_OR_PARSE_ERROR_ExitCode); // parse error
    // FIX: If I turn the exit into a throw, as below, then I get an
    // infinite loop.
    // throw UserError(LEX_OR_PARSE_ERROR_ExitCode); // parse error

    // check for parse errors detected by the context class
    if (parseEnv->errors || lexer->errors) {
      throw UserError(LEX_OR_PARSE_ERROR_ExitCode);
    }

    if (tracingSys("parseTree")) {
      // the 'treeTop' is actually a PTreeNode pointer; print the
      // tree and bail
      PTreeNode *ptn = (PTreeNode*)treeTop;
      ptn->printTree(std::cout, PTreeNode::PF_EXPAND);
      exit(0);
    }

    TranslationUnit *unit = (TranslationUnit*)treeTop;
    xassert(unit);
    file2unit.add(file, unit);
  } catch (XOpen &e) {
    throw UserError(USER_ERROR_ExitCode, e.why().c_str());
  }
}

void Oink::typecheckOneFile(File *file) {
  maybeSetInputLangFromSuffix(file);
  TranslationUnit *unit = file2unit.get(file);

  TcheckEnv env(globalStrTable, globalLang, *tFac,
                madeUpVariables, builtinVars, unit);
  // store the first one in a global
  env.tcheckTranslationUnit(unit);

  // print errors and warnings
  int numErrors = env.errors.numErrors();
  int numWarnings = env.errors.numWarnings();
  env.errors.print(std::cout);
  if (oinkCmd->verbose || numErrors > 0 || numWarnings > 0) {
    std::cout << "typechecking results:\n"
         << "  errors:   " << numErrors << "\n"
         << "  warnings: " << numWarnings << "\n";
  }
  if (numErrors != 0) throw UserError(TYPECHECKING_ERROR_ExitCode);

  // verify the tree now has no ambiguities
  if (unit && numAmbiguousNodes(unit) != 0) {
    std::cout << "UNEXPECTED: ambiguities remain after type checking!\n";
    throw UserError(TYPECHECKING_ERROR_ExitCode);
  }

  // check that the AST is a tree *and* that the lowered AST is a tree
  LoweredIsTreeVisitor treeCheckVisitor;
  unit->traverse(treeCheckVisitor.loweredVisitor);
}

void Oink::astPrintOneFile(File *file) {
  maybeSetInputLangFromSuffix(file);
  printStart(file->name.c_str());
  TranslationUnit *unit = file2unit.get(file);
  unit->debugPrint(std::cout, 0);
  printStop();
}

void Oink::init_stage(int argc, char **argv) {
  traceAddSys("serialNumbers");
  xBase::logExceptions = false;
  oinkCmd->readEnvironment();
  oinkCmd->readCommandLine(argc, argv);
  // we only know whether to printStage after readCommandLine
  printStage("init");
  oinkCmd->flattenInputFiles();
  // check files are not repeated; the source location manager can't
  // handle it
  {
    StringTable fileNamesSeen;
    FOREACH_ASTLIST(File, oinkCmd->inputFilesFlat, iter) {
      if (fileNamesSeen.get(iter.data()->name.c_str())) {
        throw UserError
          (USER_ERROR_ExitCode,
           stringc << "input file name repeated; the Elsa source "
           "location manager can't deal with this so don't do it: "
           << iter.data()->name.c_str());
      }
      fileNamesSeen.add(iter.data()->name.c_str());
    }
  }
  if (oinkCmd->verbose) oinkCmd->dump();
  oinkCmd->initializeFromFlags();
  if (!oinkCmd->control.empty()) {
    loadControlFile();
    controls->buildIndex();
  }
  globalLang.GNU_Cplusplus();   // default to C++
  if (oinkCmd->lang != SUFFIX_InputLang) {
    setLangState_fromInputLang(globalLang, oinkCmd->lang);
  }
  globalLang.enableAllWarnings();
  if (oinkCmd->verbose) {
    std::cout << "**** language settings:\n";
    std::cout << globalLang.toString();
    std::cout << std::endl;
    std::cout << "**** tracing flags:\n\t";
    printTracers(std::cout, "\n\t");
    std::cout << std::endl;
  }

  checkInputFilesRecognized();

  xBase::logExceptions = false;
}

void Oink::checkInputFilesRecognized() {
  foreachFile {
    File const * file = files.data();
    if (!(isSource(file) || isSerialized(file))) {
      userFatalError
        (SL_UNKNOWN,
         "Unknown file '%s' - extension not recognized that of a "
         "source file or serialized archive.",
         file->name.c_str());
    }
  }
}

void Oink::parse_stage() {
  printStage("parse");
  // FIX: don't need var tables; also make exn safe
  {
    std::auto_ptr<ParseTables> parseTables (parseUserActions->makeTables());
    foreachSourceFile parseOneFile(files.data(), parseTables.get());
  }

  checkHeap();
  if (oinkCmd->print_ast) foreachSourceFile astPrintOneFile(files.data());
  if (oinkCmd->exit_after_parse) throw UserError(NORMAL_ExitCode);
}

void Oink::typecheck_stage() {
  printStage("typecheck");

  foreachSourceFile typecheckOneFile(files.data());
  typecheckingDone = true;

  if (oinkCmd->print_typed_ast) {
    foreachSourceFile astPrintOneFile(files.data());
  }
  if (oinkCmd->exit_after_typecheck) {
    throw UserError(NORMAL_ExitCode);
  }
}

void Oink::elaborate_stage() {
  printStage("elaborate");
  if (oinkCmd->do_elaboration) {
    foreachSourceFile {
      File *file = files.data();

      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);

      ElabVisitor vis(globalStrTable, *tFac, unit);
      if (!globalLang.isCplusplus) {
        vis.activities = EA_C_ACTIVITIES;
      }

      // if we are going to pretty print, then we need to retain
      // defunct children
      if (oinkCmd->pretty_print) {
        vis.cloneDefunctChildren = true;
      }

      unit->traverse(vis.loweredVisitor);

      // check that the AST is a tree *and* that the lowered AST is a tree
      LoweredIsTreeVisitor treeCheckVisitor;
      unit->traverse(treeCheckVisitor.loweredVisitor);
    }
  }
  // this may be a bit odd to do if do_elaboration is off, but it just
  // means that the stage has run, not that any elaboration got done
  elaboratingDone = true;
  if (oinkCmd->print_elaborated_ast) {
    foreachSourceFile astPrintOneFile(files.data());
  }
  if (oinkCmd->exit_after_elaborate) {
    throw UserError(NORMAL_ExitCode);
  }
}

void Oink::markRealVars_stage() {
  printStage("mark real vars");
//   Restorer<bool> restorer(value2typeIsOn, true);
  MarkRealVars markReal;
  visitVarsF(builtinVars, markReal);
  // do the vars in each source file
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    visitRealVarsF(unit, markReal);
  }
}

void Oink::compute_funcGran() {
  // We wish to compute the control flow graph at the whole-function
  // granularity.  We can then analysise only those functions which
  // can possibly be called from main or global data ctors and dtors.
  //
  // First we invent a notion of a "super-main" which calls all that.
  // For E_variables of function type, if they are in an E_funCall or
  // E_constructor, we just have the current function depend on them.
  // For E_varaibles in any other context (especially but not limited
  // to E_addrOf) we have to assume that its address is being taken;
  // when E_addrOf is being precisely elaborated onto the AST then we
  // can only depend on super-main at E_addrOf.
  //
  // That is, we distinguish some Variables as called directly and
  // therefore depending only on the enclosing function.  1) When we
  // see an E_funCall we look down inside the func Expression; if that
  // is an E_variable or an E_fieldAcc (after going down inside any
  // E_groupings), this is a direct call.  2) Also for MemberInit,
  // E_constructor, E_new, IN_ctor, the ctorVar is a direct call.
  // FIX: IN_expr and IN_compound can probably also call ctors but
  // that doesn't show up in the AST right now so I skip it.
  //
  // For direct call function vars of kind 2) above, we just make the
  // dependency from the current function.  For direct call function
  // vars of kind 1) above, we launch a sub-traversal of the arguments
  // and then return false so that the E_variable or E_fieldAcc is not
  // reached.  Otherwise, at an E_variable or E_funCall, we must make
  // <super-main> depend on the variable.  FIX: What about
  // PQ_variable? TS_name?  E_variable::nondependentVar?
  //
  // Also have to consider virtual functions as being called whenever
  // any of them are called.  We do a seperate pass over the variables
  // and for any that are virtual methods, traverse those overloading
  // it and make a down-dependency from A::f to B::f whenever B
  // inherits from A.
  printStage("compute function granularity control flow graph");
  FuncGranASTVisitor funcGranVis(funcGranGraph);
  FuncGranVarVisitor funcGranVarVis(funcGranGraph);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    // get the local dependencies from a straight call to a function
    // and the global dependencies from taking the address of a
    // function and the special dependency of super-main on main()
    unit->traverse(funcGranVis.loweredVisitor);
    // get the implicit function pointers from virtual methods
    visitRealVarsF(unit, funcGranVarVis);
  }

  if (oinkCmd->func_gran_rev_mod_pub) {
    funcGranGraph.build_flowsFrom();
    funcGranGraph.build_module2ToVars();
  }
}

void Oink::printVariableName_funcGran(std::ostream &out, Variable *var) {
  if (var == funcGranGraph.root) {
    out << "super-main";
    return;
  }
  // normal var; prefix non-linker-visible names with the filename
  if (var->linkerVisibleName()) {
    out << var->fullyQualifiedMangledName0();
  } else {
    out << locToStr(var->loc) << ":";
    if (oinkCmd->func_gran_dot) out << "\\n";
    out << var->mangledName0();
  }
}

void Oink::printVariableAndDep_funcGran
  (std::ostream &out, Variable *from, SObjSet<Variable*> *toSet)
{
  // print Variable from
  printVariableName_funcGran(out, from);
  out << '\n';
  // print the set it goes to
  for(SObjSetIter<Variable*> setIter(*toSet);
      !setIter.isDone(); setIter.adv())
  {
    out << '\t';               // tab-indent; this is redundant
    printVariableName_funcGran(out, setIter.data());
    out << '\n';
  }
  // separate with a blank line
  out << '\n';
}

// print the above in DOT format
void Oink::printVariableAndDep_DOT_funcGran
  (std::ostream &out, Variable *from, SObjSet<Variable*> *toSet)
{
  for(SObjSetIter<Variable*> setIter(*toSet);
      !setIter.isDone(); setIter.adv())
  {
    out << '\"';
    printVariableName_funcGran(out, from);
    out << "\" -> \"";
    printVariableName_funcGran(out, setIter.data());
    out << '\"';
  }
  out << '\n';
}

void Oink::output_funcGran(std::ostream &out, bool dot)
{
  if (dot) out << "digraph {\n";
  // NOTE: the output order is not canonical
  for(PtrMap<Variable,
        SObjSet<Variable*> >::Iter mapIter(funcGranGraph.flowsTo);
      !mapIter.isDone(); mapIter.adv())
  {
    if (dot) {
      printVariableAndDep_DOT_funcGran(out, mapIter.key(), mapIter.value());
    } else {
      printVariableAndDep_funcGran(out, mapIter.key(), mapIter.value());
    }
  }
  if (dot) out << "}\n";
}

void Oink::output_funcGranRevModPub(std::ostream &out) {
  xassert(funcGranGraph.flowsFrom);
  xassert(funcGranGraph.module2ToVars);

  // FIX: until we do linking to unify all the vars that would link
  // together we can report a varible as being called from more than
  // one module because its Variable object is different in each file
  bool const verbose = false;

  // For each module, iterate over its to-vars; for each to-var look
  // up its loc; from that look up its module.  If that is different
  // from the current module, this function is public, otherwise not.
  SFOREACH_OBJLIST(char, moduleList, iter) {
    StringRef module = iter.data();
    if (verbose) {
      std::cerr << std::endl;
      std::cerr << "module: " << module << std::endl;
    }
    SObjSet<Variable*> *varSet = funcGranGraph.module2ToVars->get(module);
    if (!varSet) continue;
    for(SObjSetIter<Variable*> setIter(*varSet);
        !setIter.isDone(); setIter.adv())
    {
      // for each variable 'to' in 'module'
      Variable *to = setIter.data();
      if (verbose) {
        std::cerr << "var:";
        if (to->linkerVisibleName()) {
          std::cerr << to->fullyQualifiedMangledName0();
        } else {
          std::cerr << "loc:" << locToStr(to->loc) << ":" << to->mangledName0();
        }
        std::cerr << std::endl;
      }
      // Note: if we look up module of 'to' we should get 'module'

      // get the set of variables that call 'to'
      SObjSet<Variable*> *fromSet = funcGranGraph.flowsFrom->get(to);
      if (!fromSet) continue;

      // is 'to' public?
      bool to_isPublic = false;
      for(SObjSetIter<Variable*> fromSetIter(*fromSet);
          !fromSetIter.isDone(); fromSetIter.adv()) {
        Variable *from = fromSetIter.data();
        if (from == funcGranGraph.root) {
          // if you are called from super-main, you are public
          to_isPublic = true;
          if (verbose) {
            std::cerr << "\tcalled from super-main" << std::endl;
          }
          break;
        }
        StringRef fromModule = moduleForLoc(from->loc);
        // FIX: in theory we can use == here
        if (!streq(fromModule, module)) {
          // 'to' is called from another module
          to_isPublic = true;
          if (verbose) {
            std::cerr << "\tcalled from module " << fromModule << std::endl;
          }
          break;
        } else {
          if (verbose) {
            std::cerr << "\tcalled from its own module" << std::endl;
          }
        }
      }

      // if 'to' is public, print it
      if (to_isPublic) {
        xassert(to->linkerVisibleName());
        out << "module:" << module << " "
            << "var:" << to->fullyQualifiedMangledName0()
            << std::endl;
      }
    }
  }
}

// error if extension is wrong
static void expectExtension(char const *fname, char const *expectedExt)
{
  char const *ext = getSuffix0(oinkCmd->srz.c_str());
  if (0!=strcmp(ext, expectedExt)) {
    userFatalError(SL_UNKNOWN, "%s: expecting extension '%s', not '%s'",
                   fname, expectedExt, ext);
  }
}

void Oink::print_funcGran() {
  printStage("print function granularity control flow graph");

  if (!oinkCmd->srz.empty()) {
    // serialize fgcfg
    expectExtension(oinkCmd->srz.c_str(),
                    oinkCmd->func_gran_dot ? ".dot" : ".fgcfg");

    std::ofstream out(oinkCmd->srz.c_str());
    if (!out) {
      userFatalError(SL_UNKNOWN, "Couldn't open file '%s'\n",
                     oinkCmd->srz.c_str());
    }

    if (oinkCmd->func_gran_rev_mod_pub) {
      output_funcGranRevModPub(out);
    } else {
      output_funcGran(out, oinkCmd->func_gran_dot);
    }
  } else {
    char const *mysplash = "";
    if (oinkCmd->func_gran_rev_mod_pub) {
      mysplash = "fg-CFG-rev-mod-pub";
    } else if (oinkCmd->func_gran_dot) {
      mysplash = "fg-CFG-dot";
    } else {
      mysplash = "fg-CFG";
    }
    std::cout << "---- START ---- " << mysplash << std::endl;
    if (oinkCmd->func_gran_rev_mod_pub) {
      output_funcGranRevModPub(std::cout);
    } else {
      output_funcGran(std::cout, oinkCmd->func_gran_dot);
    }
    std::cout << "---- STOP ---- " << mysplash << std::endl;
  }
}

void Oink::loadFuncFilterNames() {
  xassert(!oinkCmd->func_filter.empty());
  xassert(!funcFilterNames);
  funcFilterNames = new PtrSet<char const>;
  std::ifstream in0(oinkCmd->func_filter.c_str());
  int lineCount = 0;
  if (!in0) {
    userFatalError(SL_UNKNOWN,
                   "Couldn't open function filter file '%s' for reading",
                   oinkCmd->func_filter.c_str());
  }
  try {
    while(in0) {
      // FIX: could a string copy be saved if I used fopen() and
      // scanf() instead?
      string line;
      getline(in0, line);
      ++lineCount;
      if (line.empty()) continue; // skip blank lines
      // NOTE: I don't bother to allow '#' commented lines
      funcFilterNames->add(globalStrTable(line.c_str()));
    }
  } catch (xBase &x) {
    x.addContextLeft(stringc << oinkCmd->func_filter.c_str()
                     << ":" << lineCount << ": ");
    throw x;
  }
}

static void filterVar_keepFuncFilterNames(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  if (var->type && var->type->asRval()->isFunctionType()) {
    stringBuilder name;
    if (var->linkerVisibleName()) {
      name << var->fullyQualifiedMangledName0();
    } else {
      name << locToStr(var->loc) << ":";
      name << var->mangledName0();
    }
    StringRef nameRef = globalStrTable(name.c_str());
    if (funcFilterNames->contains(nameRef)) {
      var->setFilteredKeep(true);
    }
  } else {
    // all non-function Variables get a by.
    var->setFilteredKeep(true);
  }
}

static void filterVar_keepAll(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  var->setFilteredKeep(true);
}

void Oink::markFuncFilterVars(void (*marker)(Variable*)) {
  // NOTE: do NOT use VisitRealVars_filter here
  VisitRealVars visFilter(marker); // VAR-TRAVERSAL
  visitVarsMarkedRealF(builtinVars, visFilter);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    visitRealVarsF(unit, visFilter);
  }
}

void Oink::filter_stage() {
  printStage("filter stage");
  if (!oinkCmd->func_filter.empty()) loadFuncFilterNames();
  markFuncFilterVars(funcFilterNames
                     ? filterVar_keepFuncFilterNames
                     : filterVar_keepAll);
  // pruneFilteredBuiltins();
}

// quarl 2006-07-17
//    Remove the builtinVars that are filtered out.
//
//    TODO: don't even create the builtins that we don't need
void Oink::pruneFilteredBuiltins()
{
  Restorer<bool> restorer(oinkCmd->all_pass_filter, false);
  int wrIndex = 0;
  for (int rdIndex = 0; rdIndex < builtinVars.length(); rdIndex++)
  {
    xassert(builtinVars[rdIndex]);
    if (asVariable_O(builtinVars[rdIndex])->filteredOut()) continue;
    if (rdIndex != wrIndex) {
      builtinVars[wrIndex] = builtinVars[rdIndex];
    }
    ++wrIndex;
  }
  builtinVars.setLength(wrIndex);
  if (wrIndex <= builtinVars.size()/2)
    builtinVars.consolidate(); // consolidate to get memory back
}

static void registerVar(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  if (var->filteredOut()) {
    userFatalError(var->loc, "Tried to use filtered-out variable '%s'",
                   var->fullyQualifiedMangledName0().c_str());
  }
  // register it with the Linker
  if (var->linkerVisibleName(true /*evenIfStaticLinkage*/)) linker.add(var);
  // set its hasFuncDefn field
  // TODO: hasFuncDefn should also be true if there are funky qualifiers.
  var->setHasFuncDefn(!!var->funcDefn);
}

void Oink::value_stage() {
  printStage("value");
  Restorer<bool> restorer(value2typeIsOn, true);
  // do the user vars in each source file
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    // make the Value annotations for Expressions and Functions
    ValueASTVisitor vis;
    unit->traverse(vis.loweredVisitor);
  }
}

// register variables with the linker
void Oink::registerVars_stage() {
  printStage("register vars");

  // FIX: I am not setting a particular lang here but I don't think it
  // matters.
  VisitRealVars_filter visLinkerReg(registerVar); // VAR-TRAVERSAL
  visitVarsMarkedRealF_filtered(builtinVars, visLinkerReg);

  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    visitRealVarsF_filtered(unit, visLinkerReg);
  }
}

void Oink::integrity_stage() {
  if (oinkCmd->check_AST_integrity) {
    printStage("integrity");
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      IntegrityVisitor vis(false, true);
      unit->traverse(vis.loweredVisitor);
    }
  }
}

// **** exclude sytnax

class ExcludeSyntax_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  SourceLoc loc;                // current source location as we traverse
  bool exclude_extra_star_amp;  // should we exclude '&*' and '*&' ?

  public:
  ExcludeSyntax_ASTVisitor(bool exclude_extra_star_amp0)
    : loweredVisitor(this)
    , loc(SL_UNKNOWN)
    , exclude_extra_star_amp(exclude_extra_star_amp0)
  {}

  virtual bool visitExpression(Expression *);

  virtual bool subVisitE_addrOf(E_addrOf *);
  virtual bool subVisitE_deref(E_deref *);
};

// look for expressions to exclude

bool ExcludeSyntax_ASTVisitor::visitExpression(Expression *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case Expression::E_ADDROF: return subVisitE_addrOf(obj->asE_addrOf()); break;
  case Expression::E_DEREF:  return subVisitE_deref(obj->asE_deref());   break;
  }
  return true;
}

bool ExcludeSyntax_ASTVisitor::subVisitE_addrOf(E_addrOf *eaddr) {
  if (exclude_extra_star_amp && eaddr->expr->isE_deref()) {
    userFatalError(loc, "illegal expression: '&*'");
  }
  return true;
}

bool ExcludeSyntax_ASTVisitor::subVisitE_deref(E_deref *ederef) {
  if (exclude_extra_star_amp && ederef->ptr->isE_addrOf()) {
    userFatalError(loc, "illegal expression: '*&'");
  }
  return true;
}

void Oink::exclude_syntax_stage() {
  if (oinkCmd->exclude_extra_star_amp) {
    printStage("exclude syntax");
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      ExcludeSyntax_ASTVisitor vis(oinkCmd->exclude_extra_star_amp);
      unit->traverse(vis.loweredVisitor);
    }
  }
}

// ****

void Oink::prettyPrint_stage() {
  if (oinkCmd->pretty_print) {
    printStage("prettyPrint");
    Restorer<bool> restorer(value2typeIsOn, true);
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      printStart(file->name.c_str());
      std::cout << "// -*-c++-*-" << std::endl;
      OStreamOutStream out0(std::cout);
      CodeOutStream codeOut(out0);
      // some clients of oink will never do pretty printing
      xassert(typePrinter);
      PrintEnv env(*(typePrinter), &codeOut);
      TranslationUnit *unit = file2unit.get(file);
      unit->print(env);
      codeOut.finish();
      printStop();
    }
  }
}

void Oink::deserialize_stage() {
  printStage("deserialize");
  xmlDanglingPointersAllowed = false;
  foreachSerializedFile {
    File *file = files.data();
    try {
      ArchiveDeserializerP arc =
        archiveSrzManager->getArchiveDeserializer(file->name.c_str());
      if (oinkCmd->print_stages) {
        printStage(stringc << "   deserialize_1archive '"
                   << file->name.c_str() << "'");
      }

      XmlLexer lexer;
      XmlReaderManager manager(lexer, globalStrTable);
      deserialize_1archive(arc.get(), manager);
    } catch(ArchiveIOException& e) {
      userFatalError(SL_UNKNOWN, "Failed to deserialize %s: %s",
                     file->name.c_str(), e.error.c_str());
    }
  }
}

/* virtual */
void Oink::deserialize_1archive
  (ArchiveDeserializer *arc, XmlReaderManager &manager)
{
  deserialize_formatVersion(arc);
  deserialize_files(arc, manager);
  deserialize_abstrValues(arc, manager);
}

// record the values that were used in the dataflow
class DataFlowEx_RecUsedValues : public DataFlowEx {
  public:
  explicit DataFlowEx_RecUsedValues() {}

  virtual void eDataFlow
    (Value *src, Value *tgt, SourceLoc loc,
     DataFlowKind dfk, bool mayUnrefSrc, bool cast,
     bool useMI, LibQual::polarity miPol, int fcId)
  {
    xassert(src);
    xassert(tgt);
    src->asRval()->setUsedInDataflow(true);
    tgt->asRval()->setUsedInDataflow(true);
  }
};

void Oink::simpleDataflow_stage() {
  printStage("simple dataflow");
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    DataFlowEx_RecUsedValues dfmuv;
    DataFlowVisitor env(unit, dfmuv, globalLang);
    unit->traverse(env.loweredVisitor);
  }
}

inline bool funcIsWeak(Variable_O *var) {
  return controls && controls->isWeakDef(var);
}

inline bool funcIsIgnored(Variable_O *var) {
  return controls && controls->isIgnored(var);
}

inline bool funcIsUnsoundIfUsed(Variable_O *var) {
  return controls && controls->isUnsoundIfUsed(var);
}

// // remember all of the variables that were used and should be warned
// // about
// class CheckUsedRealVars : public VisitRealVars_filter {
//   public:
//   // data
//   TailList<Variable_O> usedAndUnsound;
//   // tor
//   explicit CheckUsedRealVars() {}
//   // only visits each Variable once
//   virtual void visitVariableIdem(Variable *var0) {
//     Variable_O *var = asVariable_O(var0);
//     if (funcIsUnsoundIfUsed(var)) {
//       usedAndUnsound.append(var);
//     }
//   }
// };

class CheckForUsedVarsASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  // tor
  public:
  explicit CheckForUsedVarsASTVisitor()
    : loweredVisitor(this)
  {}
  virtual ~CheckForUsedVarsASTVisitor() {}

  // methods
  virtual bool visitExpression(Expression *obj) {
    if (obj->isE_variable()) {
      Variable_O *var = asVariable_O(obj->asE_variable()->var);
      if (funcIsUnsoundIfUsed(var)) {
        userReportWarning
          (var->loc, "Unsoundness warning: used %s '%s'",
           (var->type->isFunctionType() ? "function" : "variable"),
           var->name);
      }
    }
    return true;
  }
#ifdef GNU_EXTENSION
  // FIX: also don't look inside E_sizeof and E_alignofExpr
  virtual bool visitASTTypeof(ASTTypeof *obj) {
    // we do not want to do dataflow down inside a typeof expression
    // because that code isn't actually run and so it makes it look
    // like those variables were used when in fact they were not:
    // Test/typeof_undefined1.c; therefore prune the tree.
    if (obj->isTS_typeof_expr()) return false;
    return true;      // otherwise, just keep going with the traversal
  }
#endif // GNU_EXTENSION
};

void Oink::checkForUsedVars_stage() {
  printStage("check for used vars");

  // optimization: if there are no unsound-if-used controls, skip the
  // visitation as we can't get any warnings anyway
  if (!controls ||
      controls->getNumControls(OinkControl::CK_UNSOUNED_IF_USED) == 0) {
    return;
  }

  // this checks 'possibly used in dataflow', as in it also includes
  // defined functions that are not called because another translation
  // unit *might* call them when they are linked together.  This is
  // not what Karl wants but I leave it here in case we want this
  // semantics back someday.
// //   Restorer<bool> restorer(value2typeIsOn, true);
// // FIX: I am not setting a particular lang here but I don't think it
// // matters.
// CheckUsedRealVars visCheckUsed; // VAR-TRAVERSAL
// visitVarsMarkedRealF_filtered(builtinVars, visCheckUsed);
// // do the user vars in each source file
// foreachSourceFile {
//   File *file = files.data();
//   maybeSetInputLangFromSuffix(file);
//   TranslationUnit *unit = file2unit.get(file);
//   visitRealVarsF_filtered(unit, visCheckUsed);
// }
// if (visCheckUsed.usedAndUnsound.isNotEmpty()) {
//   FOREACH_TAILLIST(Variable_O, visCheckUsed.usedAndUnsound, iter) {
//     Variable_O const *var = iter.data();
//     userReportWarning(var->loc, "Unsoundness warning: used %s '%s'",
//                      (var->type->isFunctionType() ? "function" : "variable"),
//                       var->name);
//   }
// }

  // this checks 'actually used in dataflow' as in someone actually
  // used the function
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    CheckForUsedVarsASTVisitor cuvVis;
    unit->traverse(cuvVis.loweredVisitor);
  }
}

// Perform "linking".  This checks if any symbols are undefined or
// over-defined.  It puts variables to link in
// linker.tolink_name2vars.  It calls the virtual function
// linkVariables() implemented by analyses (e.g. Qual).
void Oink::link_stage() {
  printStage("link");
  // we want to report an error if a control is never used
  if (controls) controls->makeEmptyUsedControls();

  int linkErrors = 0;

  Restorer<bool> restorer(value2typeIsOn, true);
  for(PtrMap<char const, Linker::VarList>::Iter iter(linker.extern_name2vars);
      !iter.isDone();
      iter.adv())
  {
    char const *name = iter.key();
    Linker::VarList &vars = *iter.value();
    Linker::VarList &varsToLink = *new Linker::VarList;
    // we should never have made the list if it were empty
    xassert(vars.isNotEmpty());

    linkErrors += linkGetVariablesToLink(vars, varsToLink);

    if (oinkCmd->trace_link) {
      // TODO: omit builtinVars where varsToLink==0
      fprintf(stderr,"\n");
      userReportError(SL_UNKNOWN, "linking '%s' (%d variables, %d to link):",
                      vars.first()->fullyQualifiedMangledName0().c_str(),
                      vars.count(),
                      varsToLink.count());

      FOREACH_TAILLIST_NC(Variable_O, varsToLink, iter2) {
        Variable_O *var = iter2.data();
        userReportError(var->loc, "    %s%s%s%s",
                        var->fullyQualifiedMangledName0().c_str(),
                        var->getHasFuncDefn() ? " (defn)" : "",
                        funcIsWeak(var) ? " (weak)" : "",
                        funcIsIgnored(var) ? " [ignored]" :"");
      }
    }

    xassert(varsToLink.count() <= vars.count());

    // fprintf(stderr,
    //         "## linkVariables %s: ", name);
    // for (TailListIterNC<Variable_O> iter(varsToLink);
    //      !iter.isDone(); iter.adv())
    // {
    //   Variable_O *var = iter.data();
    //   fprintf(stderr, "%p ", var);
    // }
    // fprintf(stderr,"\n");

    linkVariables(varsToLink);
    if (varsToLink.isNotEmpty()) {
      linker.tolink_name2vars.add(name, &varsToLink);
    } else {
      // Karl says use concat(); I'll leave it for now.
      delete &varsToLink;
    }
  }

  linker.haveLinked = true;
  linkCheckErrors(linkErrors);
}

// **** linking

void Linker::add(Variable_O *var) {
  // std::cout << "Linker::add() var=" << (void*)var << ", name="
  // << var->name << std::endl;
  xassert(!var->isTemplate());
  xassert(!var->isUninstTemplateMember());
  xassert(var->linkerVisibleName(true /*evenIfStaticLinkage*/));

  string fullName0 = var->fullyQualifiedMangledName0();
  StringRef fullName = globalStrTable(fullName0.c_str());
  VarList *list = extern_name2vars.get(fullName);
  if (!list) {
    list = new VarList;
    extern_name2vars.add(fullName, list);
  }
  xassert(list);
  list->append(var);
}

Linker::~Linker() {
//   for (StringRefMap<VarList>::Iter iter(extern_name2vars);
//        !iter.isDone(); iter.adv())
//   {
//     VarList *list = iter.value();
//     delete list;
//   }
//   for (StringRefMap<VarList>::Iter iter(tolink_name2vars);
//        !iter.isDone(); iter.adv())
//   {
//     VarList *list = iter.value();
//     delete list;
//   }
}


Variable_O *Linker::getFunctionForLinking(const char *name)
{
  xassert(haveLinked);
  VarList *vars =
    tolink_name2vars.get(globalStrTable(name));

  if (vars) {
    // we only need to check the first item in the list because the linker
    // must have put the definition(s), if any, at the beginning.
    xassert(vars->isNotEmpty());
    Variable_O *var = vars->first();
    if (var->getHasFuncDefn()) {
      return var;
    }
  }

  return false;
}

// print out where the all the definitions came from
void dumpLocationsOfDefinitions(Linker::VarList &vars) {
  size_t n = 0;
  FOREACH_TAILLIST_NC(Variable_O, vars, iter) {
    Variable_O *var = iter.data();
    if (!var->getHasFuncDefn()) continue;

    if (n > LINK_ERROR_MAX_SYMBOLS_REPORTED) {
      userReportError(SL_UNKNOWN, "...");
      break;
    }

    userReportError(var->loc, "    %s%s%s",
                    var->fullyQualifiedMangledName0().c_str(),
                    funcIsWeak(var) ? " (weak)" : "",
                    funcIsIgnored(var) ? " [ignored]" :"");
    ++n;
  }
}

// virtual
bool Oink::varUsedInDataflow(Variable_O *var) {
  if (!var->hasAbstrValue()) return false;
  return var->abstrValue()->asRval()->getUsedInDataflow();
}

// called by Oink::link_stage() to do anything necessary to link a
// list of variables (in Qual's case, unification)
void Oink::linkVariables(Linker::VarList &varsToLink) {
  // NOTE: a class A that occurs in multiple translation units may
  // (easily) have elaborated onto it implicit function members
  // (ctors, dtors, operator=); these memebers will seem to be
  // multiply-defined; make sure to always use the first one in case
  // it matters.

  // we wish to avoid linking variables where all versions have not
  // either participated in dataflow or been defined; by doing this
  // we prevent the need to add any qvars than were already there.
  // We both use and maintain the invariant that an abstract value
  // has participated in dataflow only if it has qvar annotations

  if (varsToLink.isEmpty()) {
    // nothing to link: this must be because it was just unused function
    // prototypes.
    return;
  }

  TailListIterNC<Variable_O> iter(varsToLink);
  xassert(!iter.isDone());

  // get the first variable in the list
  Variable_O *first = iter.data();
  iter.adv();                   // skip first
  xassert(first->getReal());
  // I want to make sure this call to filteredOut happens as it
  // contains an assertion when -fo-all-pass-filter is on.
  bool filteredOut = first->filteredOut();
  xassert(!filteredOut);
  // dsw: why is this assertion here?  If we want static definitions
  // and declarations to meet up, we need to link them too; I just add
  // the filename to the fully qualified name and it should all work.
//   xassert(!first->isStaticLinkage() && !first->isStaticLocal() &&
//           "f5e7f910-2486-4fb8-b463-a91b5e6f8aa1");
  // if (first->hasFlag(DF_GNU_EXTERN_INLINE)) {
  //   first->remove_GnuExternInline();
  // }
  linker.linkerVisVarsToBeSrz.add(first);

  // do any necessary unification
  for(; !iter.isDone(); iter.adv()) {
    Variable_O *var = iter.data();
    xassert(var->getReal());
    // I want to make sure this call to filteredOut happens as it
    // contains an assertion when -fo-all-pass-filter is on.
    bool filteredOut = var->filteredOut();
    xassert(!filteredOut);
    unifyVars(first, var, var->loc); // virtual
    // make sure we mark the representative variable as defined if
    // any of the others are in case the output is linked again
    if (var->getHasFuncDefn()) {
      first->setHasFuncDefn(true);
    }
  }
}

// Iterate over vars, and put the symbols that should be linked into
// varsToLink.  The first entry is the function definition.
//
// Check whether this list of variables is over-satisfied or under-satisfied.
// Returns number of errors found.
//
int Oink::linkGetVariablesToLink(Linker::VarList &vars,
                                 Linker::VarList &varsToLink)
{
  // NOTE: we will never report a static function as being over or
  // undersatisified because we aren't even attempting to link them together
  // in the first place

  Variable_O *first = vars.first();

  // quarl 2006-06-07
  //    Check that various properties of the variables to link are consistent.
  bool varImplicit = first->hasFlag(DF_IMPLICIT);
  bool varTemplate = first->isTemplate();
  bool varInstantiation = first->isInstantiation();
  xassert(first->type);         // namespace variables should not get here
  // TODO: generalize to check type matches also if not function type
  bool varFunction = first->type->isFunctionType();
  bool varPureVirtual = first->isPureVirtualMethod();
  FOREACH_TAILLIST_NC(Variable_O, vars, iter) {
    Variable_O *var = iter.data();
    // quarl 2006-06-07
    //    See Test/inconsistent_implicit1a.cc: die if some functions
    //    implicit but others not.
    if (var->hasFlag(DF_IMPLICIT) != varImplicit) {
      userFatalError
        (var->loc,
         "Variable '%s' is inconsistently implicit across translation units",
         var->fullyQualifiedMangledName0().c_str());
    }

    if (var->isTemplate() != varTemplate) {
      userFatalError
        (var->loc,
         "Variable '%s' is inconsistently a template across translation units",
         var->fullyQualifiedMangledName0().c_str());
    }

    // This happens for e.g. clog (complex.h vs iostream); TODO allow
    // data/function segmentation
    if (var->type->isFunctionType() != varFunction) {
      userFatalError
        (var->loc,
         "Variable '%s' is inconsistently a function across translation units",
         var->fullyQualifiedMangledName0().c_str());
      FOREACH_TAILLIST_NC(Variable_O, vars, iter2) {
        Variable_O *var = iter2.data();
        var->gdb();
      }
    }

    if (var->isInstantiation() != varInstantiation) {
      userFatalError
        (var->loc,
         "Variable '%s' is inconsistently an instantiation across "
         "translation units",
         var->fullyQualifiedMangledName0().c_str());
    }

    if (var->isPureVirtualMethod() != varPureVirtual) {
      userFatalError
        (var->loc,
         "Variable '%s' is inconsistently a pure virtual method across "
         "translation units",
         var->fullyQualifiedMangledName0().c_str());
    }
  }

  // NOTE: we only report missing definitions for functions;
  // TODO: report data members as well; see FIX above as well when this is
  // done
  if (!varFunction) {
    // Link all of them
    varsToLink.appendAll(vars);
    return 0; // no errors
  }

  // count number of strong and weak symbols.
  size_t numDefsStrong = 0;
  size_t numDefsWeak = 0;

  Variable_O * firstStrong = NULL;
  Variable_O * firstWeak = NULL;

  bool usedInDataflow = false;

  FOREACH_TAILLIST_NC(Variable_O, vars, iter) {
    Variable_O *var = iter.data();
    if (varUsedInDataflow(var)) usedInDataflow = true;

    // printf("## var %p %s: hasFuncDefn=%d, ignored=%d, weak=%d, used=%d\n",
    //        var, var->name,
    //        var->getHasFuncDefn(), funcIsIgnored(var), funcIsWeak(var),
    //        usedInDataflow);

    // count the number of definitions
    if (var->getHasFuncDefn()) {
      if (funcIsIgnored(var)) continue;
      if (funcIsWeak(var)) {
        if (!numDefsWeak++) { firstWeak = var; }
      } else {
        if (!numDefsStrong++) { firstStrong = var; }
      }
    }

    // we don't yet know whether there are strong symbols to override weak
    // symbols; so we add to varsToLink later.
  }

  // quarl 2006-06-12, 2006-06-21
  //    If the function is pure virtual, it is okay to have no definitions --
  //    we link all uses.  Note that it is possible for pure virtual functions
  //    to have definitions that get called; see Test/virtual3.cc
  //    (a09c81ae-0867-450a-9489-4a31932c25d9).
  if (varPureVirtual && numDefsStrong == 0 && numDefsWeak == 0) {
    varsToLink.appendAll(vars);
    return 0; // no errors
  }

  // quarl 2006-05-01
  //    If there are any strong symbols, there should be at most one strong
  //    symbol.  The strong symbol takes precedence over weak symbols if any.
  //    Otherwise, we use the first weak symbol.
  //
  //    Weak symbols are intended to be from libraries; they are indicated via
  //    control files.
  //
  // TODO: The real solution is to extract the linkage information from the
  // real linker and use that.

  // quarl 2006-05-03, 2006-06-07
  //    It's perfectly fine to have multiple implicit or
  //    template-instantiation definitions; just use the first.  For now we
  //    just assume the contents are the same.
  if (varImplicit || varInstantiation) {
    if (numDefsStrong > 1) numDefsStrong = 1;
    if (numDefsWeak > 1) numDefsWeak = 1;
  }

  int errors = 0;

  // function definitions to link; usually this will have 0 or 1 elements.
  Linker::VarList funcDefnsToLink;

  if (numDefsStrong == 0 && numDefsWeak == 0) {
    if (usedInDataflow) {
      // under-satisfied: there are uses but no definitions
      if (oinkCmd->report_link_errors) {
        userReportError(first->loc,
                        "Un-satisfied symbol '%s'",
                        first->fullyQualifiedMangledName0().c_str());
      }
      errors++;
    } else {
      // a function that was never used, i.e. an unused prototype.  Nothing to
      // link!
    }
  } else if (numDefsStrong == 0 && numDefsWeak > 0) {
    if (numDefsWeak > 1) {
      if (oinkCmd->report_link_errors) {
        userReportWarning
          (firstWeak->loc,
           "No strong definitions for '%s' but more than 1 weak definition; "
           "using the one at this location.",
           firstWeak->fullyQualifiedMangledName0().c_str());
      }
    }

    funcDefnsToLink.prepend(firstWeak);
  } else if (numDefsStrong > 1) {
    // over-satisfied: too many strong definitions
    if (oinkCmd->report_link_errors) {
      userReportError
        (first->loc,
         "Over-satisfied symbol '%s', has %d definitions (%d strong, %d weak):",
         first->fullyQualifiedMangledName0().c_str(),
         numDefsStrong+numDefsWeak, numDefsStrong, numDefsWeak);
      dumpLocationsOfDefinitions(vars);
    }

    // Link all strong definitions -- we may want to continue analysis despite
    // link error.
    FOREACH_TAILLIST_NC(Variable_O, vars, iter) {
      Variable_O *var = iter.data();
      if (var->getHasFuncDefn() && !funcIsIgnored(var) && !funcIsWeak(var)) {
        funcDefnsToLink.append(var);
      }
    }
    errors++;
  } else if (numDefsStrong == 1) {
    // Exactly one strong definition, great!  If there are weak symbols,
    // ignore them.

    funcDefnsToLink.append(firstStrong);
  } else {
    xassert(false);
  }

  varsToLink.appendAll(funcDefnsToLink);

  // Any use of the variable needs to be linked.  If a translation unit has a
  // use of a function and also a definition we don't want to link, clear the
  // definition.
  FOREACH_TAILLIST_NC(Variable_O, vars, iter) {
    Variable_O *var = iter.data();
    if (varUsedInDataflow(var)) {
      if (funcDefnsToLink.contains(var)) {
        // We've already added this variable to the beginning.
      } else {
        // (If it's not a function this is a no-op.)
        var->setHasFuncDefn(false);
        var->funcDefn = NULL;
        varsToLink.append(var);
      }
    }
  }

  return errors;
}

void Oink::linkCheckErrors(int linkErrors) {
  // check that main has a definition
  if (oinkCmd->report_link_errors) {
    if (!linker.getFunctionForLinking("F:main")) {
      userReportError
        (SL_UNKNOWN, "Un-satisfied symbol: no definition for 'main'");
      ++linkErrors;
    }
  }

  // report unused controls
  if (oinkCmd->report_unused_controls && controls) {
    if (!controls->allControlsUsed()) {
      userReportError(SL_UNKNOWN,
                      "Some controls were not used during link_stage");
      controls->printUnusedControls();
      throw UserError(LINK_ERROR_ExitCode);
    }
  }

  if (oinkCmd->report_link_errors && linkErrors) {
    userReportError
      (SL_UNKNOWN,
       "Link failed due to un-satisfied or over-satisfied symbols");
    throw UserError(LINK_ERROR_ExitCode);
  }
}

// return a list of variables to serialize.
void Linker::getOrderedExternVars
  (VarPredicateFunc *varPred, TailList<Variable_O> &externVars)
{
  xassert(haveLinked);
  if (sortNameMapDomainWhenSerializing) {
    for(StringRefMap<Linker::VarList>::SortedKeyIter iter(tolink_name2vars);
        !iter.isDone(); iter.adv()) {
      Linker::VarList *varlist = iter.value();
      xassert(varlist->isNotEmpty());
      Variable_O *var = varlist->first();
      xassert(var);
      xassert(var->getReal());
      xassert(streq(var->fullyQualifiedMangledName0(), iter.key()));
      if (!var->filteredOut() && (!varPred || varPred(var))) {
        // 04c1fde6-5c97-4f2c-a176-b8e4f2bc27de
        // c9e8f15c-0e47-4852-b7f1-1ede70a102dd
        // FIX: we should turn this back on or at least figure out why
        // it was failing
//       xassert(var->hasAbstrValue()
//               // we must have a value
//               && "c9e8f15c-0e47-4852-b7f1-1ede70a102dd");
        externVars.append(var);
      }
    }
  } else {
    xfailure("make sure this is off for now");
    for(PtrMap<char const, Linker::VarList>::Iter iter(linker.tolink_name2vars);
        !iter.isDone(); iter.adv()) {
      xfailure("this code should be the same as the above");
    }
  }
}

// **** serialization and deserialization

void Oink::initArchiveSrzManager(const char *ext_dir, const char *ext_zip)
{
  archiveSrzManager = new ArchiveSerializationManager(ext_dir, ext_zip);
}

static inline
std::string describe_data_architecture()
{
  // quarl 2006-11-12
  //     Definitions from endian.h.  Alternative is to use autoconf's
  //     endianness (AC_C_BIGENDIAN) checks.

  // FIX: restore this if we can get g++ to find endian.h
// #if BYTE_ORDER == BIG_ENDIAN
// #  define ENDIANNESS_STR "bigendian"
// #elif BYTE_ORDER == LITTLE_ENDIAN
// #  define ENDIANNESS_STR "littleendian"
// #else
// #  error "Unknown BYTE_ORDER"
// #endif

  std::string s;
  // FIX: dsw: see above
//   s = ENDIANNESS_STR;
  s = "unknown-byte-order";

  int const wordsize = 8 * sizeof(void*);

  if (wordsize == 32) {
    s += "-32bit";
  } else if (wordsize == 64) {
    s += "-64bit";
  } else {
    xfailure(stringc << "unknown word size "
             << wordsize
             << " (c312a595-7a96-483c-88f6-84a99ed2d6bc)");
  }

  return s;

// #if WORDSIZE == 64
// #  define WORDSIZE_STR "64bit"
// #elif WORDSIZE == 32
// #  define WORDSIZE_STR "32bit"
// #else
// #  error "Unknown word size"
// #endif

  // return (ENDIANNESS_STR "-" WORDSIZE_STR);
}

void Oink::srzFormat(ArchiveSrzFormat &srzfmt, bool writing)
{
  int serialization_format = -1;
  if (writing) {
    srzfmt.opt("serialization_format", SERIALIZATION_FORMAT);
  } else {
    std::string const &s_serialization_format =
      srzfmt.get("serialization_format");
    if (s_serialization_format.empty() || s_serialization_format[0] != 'v') {
      userFatalError
        (SL_UNKNOWN,
         "Error reading format file, invalid serialization_format '%s'",
         s_serialization_format.c_str());
    }

    serialization_format = atoi(s_serialization_format.c_str()+1);

    if (serialization_format == 16) {
      // OK
    } else if (serialization_format == 17) {
      // OK
    } else {
      userFatalError
        (SL_UNKNOWN,
         "Error reading format file, unsupported serialization_format '%s'",
         s_serialization_format.c_str());
    }
  }

  srzfmt.opt("backend", getBackendName());
  srzfmt.opt("handleExternInline_asPrototype",
             handleExternInline_asPrototype());

  if (writing || serialization_format >= 17) {
    // quarl 2006-11-12
    //     TODO: allow reading from different architectures.
    //
    //     In the distant future when we XMLify qual serialization, we won't
    //     have this problem anymore.
    srzfmt.opt("data_architecture", describe_data_architecture());
  }

}

// **** serialization

void Oink::serialize_results() {
  printStage("serialize_results");
  try {
    ArchiveSerializerP arc =
      archiveSrzManager->getArchiveSerializer(oinkCmd->srz.c_str());

    serialize_formatVersion(arc.get());
    serialize_files(arc.get());
    serialize_abstrValues(arc.get());

  } catch(ArchiveIOException & e) {
    userFatalError(SL_UNKNOWN, "Failed to serialize %s: %s",
                   oinkCmd->srz.c_str(), e.error.c_str());
  }
}

// **** serialization: format

void Oink::serialize_formatVersion(ArchiveSerializer * arc) {
  std::ostream & out = arc->output("format.txt");
  ArchiveSrzFormatWriter srzfmt(out);
  srzFormat(srzfmt, true /* writing */);
}

// **** serialization: files

void Oink::serialize_files(ArchiveSerializer * arc) {
  std::ostream& out = arc->output("files.xml");
  serialize_files_stream(out);
}

void Oink::serialize_files_stream(std::ostream &out) {
  bool indent = tracingSys("xmlPrintAST-indent");
  int depth = 0;                // shared depth counter between printers

  // serialize Files; If these are not in a canonical order, they should be
  IdentityManager idmgr;
  XmlFileWriter::XFW_SerializeOracle srzOracle;
  XmlFileWriter xmlFileWriter(idmgr, &out, depth, indent, &srzOracle);
  xmlFileWriter.toXml(sourceLocManager->serializationOnly_get_files());
}

// **** serialization: values

void Oink::serialize_abstrValues(ArchiveSerializer* arc) {
  // write to a string first, since we can't interleave output to two archive
  // files
  std::ostream& valueOut = arc->output("value.xml");

  bool indent = tracingSys("xmlPrintAST-indent");
  int depth = 0;              // shared depth counter between printers
  IdentityManager idmgr;
  // TODO: should this only serialize vars/values used in dataflow?
  XmlValueWriter::XVW_SerializeOracle srzOracle;
  XmlValueWriter valueWriter
    (idmgr,
     // NULL, NULL,
     (ASTVisitor*)NULL,
     (ValueVisitor*) NULL, &valueOut, depth, indent, &srzOracle);
  serialize_abstrValues_stream(valueWriter, NULL);
}

void Oink::serialize_abstrValues_stream
  (XmlValueWriter &valueWriter, VarPredicateFunc *varPred)
{
  // serialize the Linker contents; first we make a list and then
  // serialize that
  TailList<Variable_O> externVars;
  linker.getOrderedExternVars(varPred, externVars);
  valueWriter.toXml_externVars(&externVars);
}

// **** deserialization

// Makes sure that the top-level is exactly one XML tag and returns it.
void* Oink::expectOneXmlTag(XmlReaderManager &manager, int expectKind) {
  manager.parseOneTopLevelTag();
  if (manager.lexer.haveSeenEof()) {
    manager.xmlUserFatalError
      (stringc << "Did not get top-level tag, was expecting " << expectKind);
  }

  int lastKind = manager.getLastKind();
  if (lastKind != expectKind) {
    manager.xmlUserFatalError
      (stringc << "Illegal top-level tag " << lastKind
       << ", was expecting " << expectKind);
  }

  void * lastNode = manager.getLastNode();

  // complete the link graph
  // printStage("         satisfying links");

  manager.satisfyLinks();

  manager.parseOneTopLevelTag();
  if (!manager.lexer.haveSeenEof()) {
    manager.xmlUserFatalError("Too many top-level tags");
  }

  return lastNode;
}

// **** deserialization: format

void Oink::deserialize_formatVersion(ArchiveDeserializer * arc) {
  std::istream& in = arc->input("format.txt");

  ArchiveSrzFormatChecker srzfmt(in, arc->curFname());
  srzFormat(srzfmt, false /* reading */);
}

// **** deserialization: files

void Oink::deserialize_files
  (ArchiveDeserializer *arc, XmlReaderManager &manager)
{
  std::istream& in = arc->input("files.xml");
  deserialize_files_stream(manager, in, arc->curFname(), arc->archiveName());
}


void Oink::deserialize_files_stream
  (XmlReaderManager &manager, std::istream &in, char const *fname,
   char const *archiveName)
{
  // prevent the SourceLocManager from looking at files in the file
  // system
  Restorer<bool> res(sourceLocManager->mayOpenFiles, false);

  manager.lexer.inputFname = fname;
  manager.inputFname = fname;
  manager.lexer.restart(&in);

  // make file reader
  // use heap-allocated readers because if we get an exception, then the
  // underlying ASTList will 'delete' it upon stack unwinding.
  XmlFileReader *fileReader = new XmlFileReader;
  manager.registerReader(fileReader);

  if (oinkCmd->print_stages) {
    // printStage("      reading files");
  }
  ObjList<SourceLocManager::FileData> *files =
    (ObjList<SourceLocManager::FileData>*)
    expectOneXmlTag(manager, XTOK_List_files);

  FOREACH_OBJLIST_NC(SourceLocManager::FileData, *files, fileIter) {
    SourceLocManager::FileData *fileData = fileIter.data();

    if (!fileData->complete()) {
      manager.xmlUserFatalError("missing attributes to File tag");
    }

    char const *filename = fileData->name.c_str();

    if (sourceLocManager->isLoaded(filename)) {
      if (!isAbsolutePathname(filename)) {
        std::cout << "Warning: not reloading source locations for '"
                  << filename
                  << "', assuming they are the same as "
                  << "previously loaded version"
                  << std::endl;
      }
    } else {
      sourceLocManager->loadFile(fileData);
      OinkControlStrings::addArchiveName(filename, archiveName);
    }
  }

  // Note: 'files' owns the FileDatas so it will delete them for us.
  delete files;

  // no longer need file reader.
  manager.unregisterReader(fileReader);
}

// **** deserialization: values

void Oink::deserialize_abstrValues
  (ArchiveDeserializer *arc, XmlReaderManager &manager)
{
  // use heap-allocated readers because if we get an exception, then the
  // underlying ASTList will 'delete' it upon stack unwinding.
  XmlTypeReader *typeReader = new XmlTypeReader;
  manager.registerReader(typeReader);
  XmlValueReader *valueReader = new XmlValueReader(*typeReader);
  manager.registerReader(valueReader);

  deserialize_abstrValues_toLinker(arc, manager);

  manager.unregisterReader(typeReader);
  manager.unregisterReader(valueReader);
}

void Oink::deserialize_abstrValues_toLinker
  (ArchiveDeserializer *arc, XmlReaderManager &manager)
{
  // manager must already have XML readers registered
  std::istream& in = arc->input("value.xml");

  SObjList<Variable_O> *externVars =
    deserialize_abstrValues_stream(manager, in, arc->curFname());

  SFOREACH_OBJLIST_NC(Variable_O, *externVars, iter) {
    linker.add(iter.data());
  }
  delete externVars;           // this should be safe and saves memory
}

SObjList<Variable_O> *Oink::deserialize_abstrValues_stream
  (XmlReaderManager &manager, std::istream& in, const char* fname)
{
  manager.lexer.inputFname = fname;
  manager.inputFname = fname;
  manager.lexer.restart(&in);

  // the readers are expected to be registered with the manager before it is
  // passed in here.

  // printStage("      reading values");
  SObjList<Variable_O> *externVars = (SObjList<Variable_O> *)
    expectOneXmlTag(manager, XTOK_List_externVars);

  return externVars;
}

StringRef moduleForLoc(SourceLoc loc) {
  // loc must map to a file.
  USER_ASSERT(loc != SL_UNKNOWN, loc,
              "Cannot map an unknown location to a module.");
  USER_ASSERT(loc != SL_INIT, loc,
              "Cannot map an initial location to a module.");
  char const *filename = sourceLocManager->getFile(loc);
  StringRef module = file2module.queryif(filename);
  if (!module) {
    USER_ASSERT(defaultModule, loc,
                "Default module is needed for file '%s' but none was given.",
                filename);
    module = defaultModule;
  }
  return module;
}

Variable *typedefVarForType(Type *type) {
  if (!type->isCVAtomicType()) return NULL;
  CVAtomicType *cvat = type->asCVAtomicType();
  if (!cvat->atomic->isCompoundType()) return NULL;
  CompoundType *cpd = cvat->atomic->asCompoundType();
  Variable_O *typedefVar = asVariable_O(cpd->typedefVar);
  return typedefVar;
}

StringRef moduleForType
  (StringRefMap<char const> *classFQName2Module, Type *type)
{
  Variable *typedefVar = typedefVarForType(type);
  if (!typedefVar) return NULL;
  StringRef lookedUpModule = classFQName2Module->get
    (globalStrTable(typedefVar->fullyQualifiedMangledName0().c_str()));
  // FIX: should this be a user assert?
  xassert(lookedUpModule);
  return lookedUpModule;
}

StringRef funCallName_ifSimpleE_variable(E_funCall *obj) {
  Expression *func0 = obj->func->skipGroups();
  if (!func0->isE_variable()) return NULL;
  return func0->asE_variable()->name->getName();
}

// **** allocation query utility

// FIX: we should actually be checking the gcc __attribute__(()) for
// this instead of just looking at the function name; update: no such
// __attribute__(()) on BSD for allocators

bool isStackAllocator(StringRef funcName) {
  return streq(funcName, "alloca"); // NOTE: can return NULL
}

bool isHeapNewAllocator(StringRef funcName) {
  return false
    || streq(funcName, "malloc")
    || streq(funcName, "xmalloc") // malloc that exits rather than return NULL
    || streq(funcName, "calloc")
    || streq(funcName, "valloc") // seems to be BSD-specific
    ;
}

bool isHeapReAllocator(StringRef funcName) {
  return false
    || streq(funcName, "realloc")
    || streq(funcName, "reallocf") // BSD-specific
    ;
}

bool isHeapDeAllocator(StringRef funcName) {
  return streq(funcName, "free");
}

bool isHeapSizeQuery(StringRef funcName) {
  // these seem to be BSD-specific
  return false
    || streq(funcName, "malloc_size")
    || streq(funcName, "malloc_good_size")
    ;
}
