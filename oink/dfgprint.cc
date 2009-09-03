// see License.txt for copyright and terms of use

#include "dfgprint.h"           // this module
#include "dfgprint_cmd.h"       // DfgPrintCmd
#include "dfgprint_global.h"
#include "dataflow_visitor.h"   // DataFlowVisitor
#include "strutil.h"            // quoted
#include "oink.gr.gen.h"        // CCParse_Oink
#include "oink_util.h"

class NameRealVars : public VisitRealVars_filter {
  PtrMap<Value, char const> &valueName;

  // tor
  public:
  explicit NameRealVars(PtrMap<Value, char const> &valueName0)
    : VisitRealVars_filter(NULL), valueName(valueName0)
  {}
  // methods
  virtual void visitVariableIdem(Variable *var); // only visits each Variable once
};

void NameRealVars::visitVariableIdem(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  xassert(var->getReal());
  if (var->hasFlag(DF_NAMESPACE)) return;
  Value *v = var->abstrValue();
  char const *name = NULL;
  if (var->linkerVisibleName()) {
    name = var->fullyQualifiedName0().c_str();
  } else {
    // dsw: for now we are asserting that if you take the
    // mangledName() of a variable that it is linker visible.
    //          name = var->mangledName0().c_str();
    name = var->name;
  }

  //      cout << "variable" << endl;
  //      printf("\t(%p) '", v);
  //      cout << v->toString() << "' -> '" << name << "'" << endl;
  if (name) {
    name = strdup(name);
  } else {
    name = "";
  }
  valueName.add(v, name);
}

// ****

void DfgPrint::uniValues_stage() {
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    // build a canonical rep for all the values that are unified
    DataFlowEx_ValueUnify dfetu(canonValue);
    DataFlowVisitor env(unit, dfetu, globalLang);
    unit->traverse(env.loweredVisitor);
  }
}

void DfgPrint::nameValues_stage() {
  NameRealVars nameReal(this->valueName); // VAR-TRAVERSAL
  visitVarsMarkedRealF_filtered(builtinVars, nameReal);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);

    // name each variable's value
    visitRealVarsF_filtered(unit, nameReal);

    // name each expression's value
    NameValuesVisitor v(canonValue, valueName);
    unit->traverse(v.loweredVisitor);
  }
}

// Please note that the graph layout output here is done immediately;
// however you could get a better result with 'dot' if you did it in
// two passes intead: use union find to compute the connected
// components, then print each one out separately wrapped in its own
// subgraph cluster.  In the second pass you could get 'dot' to order
// the connected components vertially instead of horizontally, which
// would make the output easier to read.  However, my goal here is to
// do a simple demo for getting the dataflow graph out of a program
// and that would make it more complex than needed.
void DfgPrint::printDfg_stage() {
  Restorer<bool> restorer(value2typeIsOn, true);
  std::cout << "digraph G {" << std::endl;
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    DataFlowEx_Print dfep(canonValue, valueName);
    DataFlowVisitor env(unit, dfep, globalLang);
    unit->traverse(env.loweredVisitor);
  }
  std::cout << "}" << std::endl;
}

// ****

void DataFlowEx_ValueUnify::eDataFlow_unify(Value *src, Value *tgt, SourceLoc loc) {
  // do not delegate to superclass as it is useless
//    DataFlowEx::eDataFlow_unify(src, tgt, loc);
//    cout << "eDataFlow_unify src " << src->serialNumber
//         << "; tgt " << tgt->serialNumber
//         << endl;
  canonValue.uni(src, tgt);
  canonValue.uni(src->asRval(), tgt->asRval());
}

void DataFlowEx_ValueUnify::eDataFlow_refUnify(Value *src, Value *tgt, SourceLoc loc) {
  // delegate to superclass
  DataFlowEx::eDataFlow_refUnify(src, tgt, loc);
  canonValue.uni(src, tgt);
}

// ****

void NameValuesVisitor::addName0(Value *v, char const *str) {
  v = canonValue.find(v);
//    printf("\t(%p) '", v);
//    cout << v->toString() << "' -> '" << str << "'" << endl;
  valueName.add(v, str);
}

void NameValuesVisitor::addName(Value *v, rostring origStr) {
  char const *str = toCStr(origStr);

  str = strdup(str);

  addName0(v, str);

  // derefed value also
  Value *t0 = v->asRval();
  if (t0 == v) return;
  addName0(t0, str);
}


bool NameValuesVisitor::visitFunction(Function *obj)
{
  // REMOVE_FUNC_DECL_VALUE: remove this whole body
  PQName *declId = obj->nameAndParams->getDeclaratorId();
  if (declId) {
    addName(obj->abstrValue, declId->toString());
  }
  return true;
}

bool NameValuesVisitor::visitExpression(Expression *obj)
{
//    cout << "visitExpression\n";
  if (!obj->abstrValue) {
    xassert(!obj->type);
    // FIX: work around a bug in Elsa that sometimes E_stringLit
    // doesn't get a type
    xassert(obj->isE_stringLit());
  } else {
    addName(obj->abstrValue, obj->exprToString());
  }
  return true;
}

bool NameValuesVisitor::visitDeclarator(Declarator *obj)
{
  // REMOVE_FUNC_DECL_VALUE: remove this whole body
  PQName *declId = obj->decl->getDeclaratorId();
  if (declId) {
    addName(obj->abstrValue, declId->toString());
  }
  return true;
}

// ****

char const *DataFlowEx_Print::getValueName(Value *v) {
  stringBuilder ret;
  char const *name = valueName.get(v);
  if (name) ret << name;
  else ret << "<" << v->t()->toMLString() << ">";
//    ret << "\n@";
  ret << "@";
  ret << toLCString(v->loc);
//    ret << locToStr(t->loc);
  return strdup(quoted(ret).c_str());
}

void DataFlowEx_Print::eDataFlow
  (Value *src, Value *tgt, SourceLoc loc,
   DataFlowKind dfk, bool mayUnrefSrc, bool cast,
   bool useMI, LibQual::polarity miPol, int fcId)
{
  xassert(src);
  xassert(tgt);
  src = canonValue.find(src->asRval());
  tgt = canonValue.find(tgt->asRval());
  // prevent self-loops
  if (src == tgt) return;
  // unification has already been done so it should not show up here
  xassert(dfk != UN_DataFlowKind);

  // special case: omit flow to and from void
  if (src->t()->isVoid() || tgt->t()->isVoid()) return;

  static int edgeNumber = 0;
  ++edgeNumber;
//    printf("eDataFlow src(%p) tgt(%p)\n", src, tgt);

  // FIX: get the names of src and tgt and print them out along with
  // the loc; use colors to indicate the DataFlowKind and cast; if
  // they are references, print out the unrefed edge also.
  std::cout << getValueName(src) << " -> " << getValueName(tgt);

  // annotate the edge
  std::cout << "[";
//    cout << "label=" << edgeNumber << ",";
  if (dfk == EQ_DataFlowKind) std::cout << "dir=both,";
  std::cout << "label="
//         << quoted(locToStr(loc))
       << quoted(toLCString(loc))
       << ",";
  if (useMI) {
    // NOTE: use fcId if you want to pair up function call edges and
    // function return edges: they all have the same id for the same
    // call site
    switch(miPol) {
    default: xfailure("illegal polarity");
    case LibQual::p_neg:        // function call: arg to parameter
      std::cout << "color=red,"; break;
    case LibQual::p_non:        // normal flow
      /*cout << "color=black,";*/ break;
    case LibQual::p_pos:        // function return
      std::cout << "color=blue,"; break;
    case LibQual::p_sub:        // I forget
      std::cout << "color=green,"; break;
    }
  }
  std::cout << "]" << std::endl;
}
