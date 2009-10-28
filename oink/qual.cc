// see License.txt for copyright and terms of use

#include "qual.h"
#include "qual_global.h"

#include "qual_dataflow_visitor.h" // dataFlow_refUnify
#include "qual_cmd.h"           // QualCmd
#include "qual_annot.h"         // findQualAndCheck, attachOneLiteralToQvar
#include "qual_value_children.h" // treeContainsExternQvars
#include "qual_libqual_iface.h" // init_libqual, LibQual::
#include "qual.gr.gen.h"        // CCParse_Qual
#include "oink_util.h"
#include "cc_print.h"

#include "xml_reader.h"         // XmlReaderManager, XmlLexer
#include "xml_type_reader.h"    // XmlTypeReader
#include "qual_xml_value_reader.h" // XmlValueReader_Q
#include "qual_xml_value_writer.h" // XmlValueWriter_Q
#include "xmlhelp.h"            // xmlPrintPointer

#include "sobjset.h"            // SObjSet

#include <sstream>
#include <memory>

#include "archive_srz.h"        // ArchiveSerializer/ArchiveDeserializer
#include "archive_srz_format.h"

// temporary: the qvars marked as extern. Only used for assertions, which will
// go away.
// static SObjSet<LibQual::Type_qualifier*> externVisitedSet_global;

// phony XML-alike tags as a sanity check that s18n is okay
static std::string const QUAL_BOF_MAGIC = "<QUAL>";
static std::string const QUAL_EOF_MAGIC = "</QUAL>";

// static utilities ****

static StringRef funCallName(E_funCall *obj) {
  Expression *func0 = obj->func->skipGroups();
  if (!func0->isE_variable()) return NULL;
  return func0->asE_variable()->name->getName();
}

// is this a function call to an allocator; FIX: we should actually be
// checking the gcc __attribute__(()) for this instead
static bool isAllocator(E_funCall *obj, SourceLoc loc) {
  StringRef funcName = funCallName(obj);
  if (!funcName) return false;
  if (streq(funcName, "alloca")) {
    userReportWarning(loc, "We can't handle alloca");
  }
  return
    streq(funcName, "malloc")  ||
    streq(funcName, "realloc") ||
    streq(funcName, "calloc")  ||
    streq(funcName, "strdup");
}

bool valueMatchesQSpec(Value *v, QSpec *qspec) {
  xassert(qspec->depth >= 0);
  // FIX: should I call asRval() here?
  for (int depth = qspec->depth; depth>0; --depth) {
    if (v->isPointerOrArrayValue()) v = v->getAtValue();
    else return false;
  }
  QualAnnot *qannot = qa(v);
  for(QLiterals *ql0=qannot->getQl(); ql0; ql0=ql0->next) {
    if (streq(ql0->name, qspec->qual)) return true;
  }
  return false;
}

// ****

void Qual::visitInSerializationOrder(ValueVisitor &visitor)
{
  IdentityManager idmgr;
  XmlValueWriter_Q::XVWQ_SerializeOracle srzOracle;
  int depth = 0;

  XmlValueWriter_Q valueVisitor(idmgr,
                                (ASTVisitor*)NULL,
                                &visitor, NULL, depth, false, &srzOracle);
  serialize_abstrValues_stream(valueVisitor, NULL);

  xassert(depth == 0);
}

// QualEcrUpdateVisitor **********

// class QualEcrUpdateVisitor : public ValueVisitor {

// public:
//   virtual bool preVisitValue(Value *obj) { qa(obj)->ecrUpdate();
//   return true; }
// };

// Update all qvars to point to their ECRs.  This is a necessary step
// before eliminate_quals_links().  (Previously it was okay to delay
// this step until serialization because eliminate_quals_links() did
// not actually deallocate any qvars.)
void Qual::updateEcrAll() {
  printStage("   updateEcrAll");
  // QualEcrUpdateVisitor visitor;
  // visit all nodes, including buckets
  // visitInSerializationOrder(visitor, true);

  // visit all values (order doesn't matter)

  FOREACH_VALUE(iter) {
    Value *v = iter.data<Value>();
    qa(v)->ecrUpdate();
  }
}

// quarl 2006-05-15
//   This visitor is not working because it doesn't visit everything the
//   pretty-printer visits.  See qa_removeDeadQvars in qual_annot.cc.

// // QualDeadQvarRemoveVisitor **********

// class QualDeadQvarRemoveVisitor : public ValueVisitor {

// public:
//   virtual bool preVisitValue(Value *obj) {
//   qa(obj)->removeQvarIfDead(); return true; }
// };

void Qual::removeDeadQvars() {
  printStage("   removeDeadQvars");

  // quarl 2006-05-15
  //   the pretty-printer traversal is hitting nodes that I can't reach
  //   through visitInSerializationOrder() for some reason; do this for now.
  //   Eventually we should get rid of this map so then we'll have to get the
  //   traversals to match up.

  // quarl 2006-05-19
  //   We now traverse the (newly introduced) list of all values
  //   Value::valueList, not the map.

  FOREACH_VALUE(iter) {
    Value *v = iter.data<Value>();
    qa(v)->removeQvarIfDead();
  }
}

// class FuncParamRetQual_ValueVisitor ****************

bool FuncParamRetQual_ValueVisitor::preVisitValue(Value *obj) {
  // idempotency
  if (visitedValue.contains(obj)) return true;
  visitedValue.add(obj);

//   QualAnnot *objAnnot = qa(asVariable_O(obj->var)->abstrValue());
  QualAnnot *objAnnot = qa(obj);
  FOREACH_ASTLIST(char, qualCmd->func_param_ret_qual, iter) {
    LibQual::Type_qualifier *qconst = findQualAndCheck(iter.data());
    // FIX: would be nice to make a "command line loc" rather than UNKNOWN
    objAnnot->attachOneLiteral(SL_UNKNOWN, qconst);
  }

  return true;
}


// Const when param ****************

// mark const when matches qspec
class MarkConst_ValueVisitor : public ValueVisitor {
  public:
  QSpec * const qspec;          // qspec to mark const
  MarkConst_ValueVisitor(QSpec * const qspec0) : qspec(qspec0) {}
  virtual bool preVisitValue(Value *obj);
};

bool MarkConst_ValueVisitor::preVisitValue(Value *obj) {
  if (valueMatchesQSpec(obj, qspec)) {
    QualAnnot *qannot = qa(obj);
    LibQual::Type_qualifier *qconst = findQualAndCheck("const");
    qannot->attachOneLiteral(obj->loc, qconst);
  }
  return true;
}

// implement const-when-param
class MarkConstWhenParam_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  QSpec * const qspec;          // qspec to mark const

  public:
  MarkConstWhenParam_ASTVisitor(QSpec * const qspec0)
    : loweredVisitor(this)
    , qspec(qspec0)
  {}

  virtual bool visitDeclarator(Declarator *obj);
};

bool MarkConstWhenParam_ASTVisitor::visitDeclarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);

  // can't decide if I should prune; don't think it matters
  if (var->filteredOut()) return false;

  if (var->hasFlag(DF_PARAMETER)) {
    MarkConst_ValueVisitor markConst(qspec);
    var->abstrValue()->traverse(markConst);
  }
  return true;
}


// Stackness analysis ****************

// Mark with REF-LEVEL qualifier $stack the values of variables
// allocated onto the stack.  Mark with REF-LEVEL qualifier $nonstack
// the values POINTED TO by variables on the heap.
// 
// Recall that a ref-level qualifier annotates the box the value comes
// in, not the value itself; that is, if we say 'int const x = 3', the
// box called x is const, not the value 3 in the box.
//
// Here by variable I mean any memory cell, whether or not it has a
// name.  Further when we annotate a variable with $stack or $nonstack
// we annotate variables that have the same contiguous storage as
// follows.
//
// 1 - For an array, also annotate its members, as long as it is a
// real array and not a pointer, such as it would be in a parameter
// list; they should have been lowered to pointers by Elsa when they
// are pointers.
//
// 2 - For a CompoundType (struct, union, class), also annotate all
// non-static members and all its super-classes.

// mark a value according to its stackness
class StacknessMarker {
  LibQual::Type_qualifier *stack_qconst;
  LibQual::Type_qualifier *nonstack_qconst;
  bool onStack;                 // true for stack, false for nonstack

  public:
  StacknessMarker(bool onStack0);

  void markValue(Value *v);
};

StacknessMarker::StacknessMarker(bool onStack0)
  : stack_qconst(findQualAndCheck("$stack"))
  , nonstack_qconst(findQualAndCheck("$nonstack"))
  , onStack(onStack0)
{
  USER_ASSERT(stack_qconst,    SL_UNKNOWN,
              "Literal '$stack' not in lattice");
  USER_ASSERT(nonstack_qconst, SL_UNKNOWN,
              "Literal '$nonstack' not in lattice");
}

void StacknessMarker::markValue(Value *v) {
  if (onStack) {
    // v is a value which is on the stack; mark it ias such
    //
    // stack variables of reference type do not allocate data on the
    // stack that can have its address taken (they are an
    // auto-dereferencing pointer; if you take their address, you just
    // get the addres of the variable they were initialized from, not
    // their actual address)
    if (v->isReferenceValue()) return;
    // I could also omit functions as well: they are basically
    // implicitly static.
    Value *rVal = v->asRval();
    qa(rVal)->attachOneLiteral(v->loc, stack_qconst);
  } else {
    // v is a value which is not on the stack; if it points to
    // something it should not be on the stack
    if (!v->hasAtValue()) return;
    Value *atVal = v->getAtValue();
    qa(atVal)->attachOneLiteral(atVal->loc, nonstack_qconst);
  }
}


// recurse on a value giving it and its members the appropriate
// stackness
class MarkStackness_ValueVisitor : public ValueVisitor {
  public:
  StacknessMarker &sm;

  public:
  MarkStackness_ValueVisitor(StacknessMarker &sm0)
    : sm(sm0)
  {}
  virtual ~MarkStackness_ValueVisitor() {}

  public:
  virtual bool preVisitValue(Value *obj);
  virtual bool preVisitVariable(Variable_O *var);

  private:
  void recurseOnNonStaticMembers(CVAtomicValue *cvat, CompoundType *ct);
};

void MarkStackness_ValueVisitor::recurseOnNonStaticMembers
  (CVAtomicValue *cvat, CompoundType *ct)
{
  SFOREACH_OBJLIST_NC(Variable, ct->dataMembers, iter) {
    Variable_O *var = asVariable_O(iter.data());
    if (var->hasFlag(DF_STATIC)) continue;
    string name0 = var->name;
    // FIX: Can I avoid creating this in the instance-sensitive case?
    Value *instSpecField = var->abstrValue();
    if (oinkCmd->instance_sensitive) {
      instSpecField = cvat->getInstanceSpecificValue(name0, instSpecField);
    }
    // recurse on that value
    instSpecField->traverse(*this);
  }
}

bool MarkStackness_ValueVisitor::preVisitValue(Value *obj) {
  if (obj->isCVAtomicValue()) {
    sm.markValue(obj);
    // recurse on non-static data members and superclasses of the
    // CompoundType if there are any
    CVAtomicValue *av = obj->asCVAtomicValue();
    if (av->type->atomic->isCompoundType()) {
      CompoundType *ct = av->type->atomic->asCompoundType();
      // mark own data members
      recurseOnNonStaticMembers(av, ct);
      // mark base class data memebers
      SObjList<BaseClassSubobj const> subobjs;
      ct->getSubobjects(subobjs);
      SFOREACH_OBJLIST(BaseClassSubobj const, subobjs, iter) {
        CompoundType *base = iter.data()->ct;
        if (base == ct) { continue; }
        recurseOnNonStaticMembers(av, base);
      }
    }
    return false;
  } else if (obj->isPointerValue()) {
    sm.markValue(obj);
    // thing pointed to does not have same stackness, so don't recurse
    return false;
  } else if (obj->isReferenceValue()) {
    // don't mark because references do not allocate their own storage
    // but refer to anothers; further thing pointed to does not have
    // same stackness, so don't recurse
    return false;
  } else if (obj->isFunctionValue()) {
    // don't mark because functions are static; further thing pointed
    // to does not have same stackness, so don't recurse
    return false;
  } else if (obj->isArrayValue()) {
    sm.markValue(obj);          // FIX: not sure what this means
    return true;                // recurse on array members
  } else if (obj->isPointerToMemberValue()) {
    sm.markValue(obj);
    // thing pointed to does not have same stackness, so don't recurse
    return false;
  } else {
    xfailure("can't happen: bad value RTTI");
  }
}

bool MarkStackness_ValueVisitor::preVisitVariable(Variable_O *obj) {
  // since we avoid functions, this should not happen
  xassert(!obj->hasFlag(DF_PARAMETER));
  return true;
}


// marking stack and non-stack Variables as such
class MarkVarsStackness_VisitRealVars : public VisitRealVars_filter {
  // data
  private:
  MarkStackness_ValueVisitor &stack_markStackness;
  MarkStackness_ValueVisitor &nonstack_markStackness;

  // ctor
  public:
  MarkVarsStackness_VisitRealVars
    (MarkStackness_ValueVisitor &stack_markStackness0,
     MarkStackness_ValueVisitor &nonstack_markStackness0);
  virtual ~MarkVarsStackness_VisitRealVars() {}

  // methods
  public:

  // only visits each Variable once
  virtual void visitVariableIdem(Variable *var);

  private:
  void markStack(Variable_O *var);
  void markNonStack(Variable_O *var);
};

MarkVarsStackness_VisitRealVars::MarkVarsStackness_VisitRealVars
  (MarkStackness_ValueVisitor &stack_markStackness0,
   MarkStackness_ValueVisitor &nonstack_markStackness0)
    : VisitRealVars_filter(NULL)
    , stack_markStackness(stack_markStackness0)
    , nonstack_markStackness(nonstack_markStackness0)
{}

void MarkVarsStackness_VisitRealVars::markStack(Variable_O *var) {
  var->abstrValue()->traverse(stack_markStackness);
}

void MarkVarsStackness_VisitRealVars::markNonStack(Variable_O *var) {
  var->abstrValue()->traverse(nonstack_markStackness);
}

// mark variables as stack, non-stack, or don't mark
void MarkVarsStackness_VisitRealVars::visitVariableIdem(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  xassert(var->getReal());
  switch(var->getScopeKind()) {
  case NUM_SCOPEKINDS: xfailure("can't happen"); break; // silly gcc warning
  case SK_UNKNOWN:              // not yet registered in a scope
    // FIX: the global scope has ScopeKind SK_UNKNOWN
//     xfailure("got SK_UNKNOWN on a real variable");
    break;
  case SK_GLOBAL:               // toplevel names
    markNonStack(var);
    break;
  case SK_PARAMETER:            // parameter list
    markStack(var);
    break;
  case SK_FUNCTION:             // includes local variables
    if (var->hasFlag(DF_STATIC)) {
      markNonStack(var); // basically it is global
    } else {
      markStack(var);
    }
    break;
  case SK_CLASS:                // class member scope
    if (var->hasFlag(DF_STATIC)) {
      markNonStack(var);        // basically it is global
    } else {
      // do not mark as the storage-class depends on the containing
      // class; the containing variable will be reallocated onto the
      // heap if it is on the stack and not if it is not
    }
    break;
  case SK_TEMPLATE_PARAMS: // template paramter list (inside the '<' and '>')
  case SK_TEMPLATE_ARGS: // bound template arguments, during instantiation
    // not code
    xfailure("got template variable as a real variable");
    break;
  case SK_NAMESPACE:            // namespace
    // don't mark as can't contain data
    break;
  }
}


// visit all of the allocation points
class MarkAllocStackness_Visitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  MarkStackness_ValueVisitor &stack_markStackness;
  MarkStackness_ValueVisitor &nonstack_markStackness;

  // allocators that we saw
  SObjSet<E_funCall*> seenAllocators;
  // allocators that were inside a cast expression
  SObjSet<E_funCall*> castedAllocators;

  public:
  MarkAllocStackness_Visitor
    (MarkStackness_ValueVisitor &stack_markStackness0,
     MarkStackness_ValueVisitor &nonstack_markStackness0);

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual void postvisitExpression(Expression *);

  virtual bool subVisitE_funCall(E_funCall *);
  virtual bool subVisitE_cast(E_cast *);
  virtual bool subVisitE_new(E_new *);
};

MarkAllocStackness_Visitor::MarkAllocStackness_Visitor
  (MarkStackness_ValueVisitor &stack_markStackness0,
   MarkStackness_ValueVisitor &nonstack_markStackness0)
  : loweredVisitor(this)
  , stack_markStackness(stack_markStackness0)
  , nonstack_markStackness(nonstack_markStackness0)
{}

void MarkAllocStackness_Visitor::postvisitExpression(Expression *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case Expression::E_FUNCALL: subVisitE_funCall(obj->asE_funCall()); break;
  case Expression::E_CAST:    subVisitE_cast(obj->asE_cast());       break;
  case Expression::E_NEW:     subVisitE_new(obj->asE_new());         break;
  }
}

bool MarkAllocStackness_Visitor::subVisitE_funCall(E_funCall *obj) {
  if (isAllocator(obj, getLoc())) {
    // record the allocator so we can look for missed allocators later
    seenAllocators.add(obj);
  }
  return true;
}

bool MarkAllocStackness_Visitor::subVisitE_cast(E_cast *obj) {
  // see if it is a cast from an allocator
  Expression *expr0 = obj->expr->skipGroups();
  if (expr0->isE_funCall() && isAllocator(expr0->asE_funCall(), getLoc())) {
    // Attach the non-stack-ness to the cast expresion pointer; thanks
    // to Matt Harren for help on this.
    USER_ASSERT(obj->abstrValue->isPointerValue(), obj->abstrValue->loc,
                "cast from allocator (malloc-like function) "
                "is not of pointer type");
    if (obj->abstrValue->isPointerValue()) {
      obj->abstrValue->asPointerValue()->atValue->
        traverse(nonstack_markStackness);
    }
    // check off the allocator so we can look for missed allocators later
    castedAllocators.add(expr0->asE_funCall());
  }
  return true;
}

bool MarkAllocStackness_Visitor::subVisitE_new(E_new *obj) {
  // Attach the non-stack-ness to the expresion; thanks to Matt Harren
  // for help on this.
  USER_ASSERT(obj->abstrValue->isPointerValue(), obj->abstrValue->loc,
              "new expression is not of pointer type");
  if (obj->abstrValue->isPointerValue()) {
    obj->abstrValue->asPointerValue()->atValue->
      traverse(nonstack_markStackness);
  }
  return true;
}


// QualSerialization_ValueVisitor ****************

class QualSerialization_ValueVisitor : public ValueVisitor {
  std::ostream &mapOut;
  int numSerializedQvars;
  LibQual::s18n_context *serializationCtxt;

  // NOTE: only used for assertion
  SObjSet<LibQual::Type_qualifier*> serializedQvars;

  IdentityManager &idmgr;

  public:
  QualSerialization_ValueVisitor
    (IdentityManager &idmgr0,
     std::ostream &mapOut0, LibQual::s18n_context *serializationCtxt0)
    : mapOut(mapOut0)
    , numSerializedQvars(0)
    , serializationCtxt(serializationCtxt0)
    , idmgr(idmgr0)
  {}

  int getNumSerializedQvars() { return numSerializedQvars; }

  // serialize the map edge
  virtual bool preVisitValue(Value *obj);
  virtual bool preVisitVariable(Variable_O *var) {
    xassert(serializeVar_O(var));
    return true;
  }
};

bool QualSerialization_ValueVisitor::preVisitValue(Value *value) {
  QualAnnot *annot = qa(value);
  // LibQual::check_all_edge_sets();

  // count the qvars; ensure non-duplicate counting; NOTE: we have to
  // do this before mapping it through LibQual::ecr_qual_noninline()
  // otherwise the count doesn't correspond to what was marked as
  // extern
  xassert(annot->hasQv());
  LibQual::Type_qualifier *qv = annot->qv();

  // printf("## QualSerialization_ValueVisitor: value=%p, qa=%p, qv=%p
  // (%p) %s\n",
  //        value, annot, qv, LibQual::ecr_qual_noninline(qv), name_qual(qv));
  // fflush(stdout);

  // quarl: I think that it's OK to see qvar more than once: this can happen
  // if we're re-serializing an already serialized, compacted, deserialized
  // file, where quals have been merged.

  // this should have been done in updateEcrAll()
  xassert(qv == LibQual::ecr_qual_noninline(qv));

  // Check that it was marked extern earlier.  Note that extern_qual(qv)
  // doesn't do a proper subset check because it goes through ecr.

  // xassert(externVisitedSet_global.contains(qv) &&
  // "6578c45f-42a6-4bcb-a1cd-c9d49fb2b0aa");
  xassert(LibQual::extern_qual(qv) && "d61f1ed1-a219-4e5b-b6cb-7c11bea92d73");

  // dsw: this was inserted by Rob; I think it is just to check we are
  // not serializing a constant
  xassert(!LibQual::constant_qual(qv));

  // quarl 2006-06-26
  //    Check that the qvar wasn't compacted away, since we now serialize the
  //    map for non-extern qvars as well?
  xassert(!LibQual::is_dead_qual(qv));

  // check that the qvar was serialized earilier
  xassert(LibQual::s18n_lookup_serialized(serializationCtxt, qv));

  // LOUD-SERIALIZATION
//   printf("map edge annot->qv==%p\n", (void const*)(annot->qv()));
//   Value_gdb(value);

  // TODO: integrate into value.xml
  outputXmlPointer(mapOut, "VL", idmgr.uniqueId(value));
  mapOut << " -> ";
    // FIX: get rid of the reinterpret_cast when Rob implements unique
    // ids for us
  outputXmlPointer(mapOut, "QV", reinterpret_cast<xmlUniqueId_t>(qv));
  mapOut << '\n';

  ++numSerializedQvars;
  return true;
}


// QvarSerializationContextHolder ****************

class QvarSerializationContextHolder {
  public:
  LibQual::s18n_context *ctxt;

  QvarSerializationContextHolder(void *stream)
    : ctxt(LibQual::s18n_new_context_io_custom(stream, readf, writef))
  {}

  ~QvarSerializationContextHolder() {
    LibQual::s18n_destroy_context(ctxt);
  }

private:
  static int writef(void* stream, const void* buf, int len) {
    std::ostream& o = *( (std::ostream*)stream );
    o.write((const char*) buf, len);
    return o ? len : -1;
  }

  static int readf(void* stream, void* buf, int len) {
    std::istream&i = *( (std::istream*)stream );
    i.read((char*) buf, len);
    return i ? len : -1;
  }
};

// Module coloring ****************

static void colorWithModule_access
  (Expression *expr, Value *value, SourceLoc loc)
{
  xassert(value);
  StringRef module = moduleForLoc(loc);

  // get the qualifier
  stringBuilder qconstName("$");
  qconstName << module;
  qconstName << "_access";
  LibQual::Type_qualifier *qconst = LibQual::find_qual(qconstName.c_str());
  USER_ASSERT(qconst, value->loc, "Literal '%s' not in lattice",
              strdup(qconstName.c_str()));

  // tell the user what we are doing
  if (oinkCmd->report_colorings) {
    std::cout << sourceLocManager->getFile(loc) << ":" <<
      sourceLocManager->getLine(loc);
    std::cout << " Expression";
//     if (name) std::cout << " '" << name << "'";
//     else std::cout << " <no-name>";
    char *name = prettyPrintASTNode(expr);
    std::cout << " '" << name << "'";
    free(name);
    std::cout << " colored " << qconstName << std::endl;
  }

  // color the value with the module
  qa(value)->attachOneLiteral(value->loc, qconst);
}

static void colorWithModule_otherWrite
  (Value *value, SourceLoc loc, char const *name, char const *astNode)
{
  xassert(value);
  StringRef module = moduleForLoc(loc);

  // tell the user what we are doing
  if (oinkCmd->report_colorings) {
    std::cout << sourceLocManager->getFile(loc) << ":" <<
      sourceLocManager->getLine(loc);
    std::cout << " " << astNode;
    if (name) std::cout << " '" << name << "'";
    else std::cout << " <no-name>";
    std::cout << std::endl;
  }

  // for each source module not equal to us, make the write target
  SFOREACH_OBJLIST(char, moduleList, iter) {
    StringRef srcModule = iter.data();
    if (srcModule == module) continue;
    // double-check in case StringRef failed to be used correctly
    xassert(strcmp(srcModule, module) != 0);

    // get the qualifier
    stringBuilder qconstName("$");
    qconstName << srcModule;
    qconstName << "_otherWrite";
    LibQual::Type_qualifier *qconst = LibQual::find_qual(qconstName.c_str());
    USER_ASSERT(qconst, value->loc, "Literal '%s' not in lattice",
                strdup(qconstName.c_str()));

    // print out a line for each
    if (oinkCmd->report_colorings) {
      std::cout << " colored " << qconstName << std::endl;
    }

    // color the value with the module
    qa(value)->attachOneLiteral(value->loc, qconst);
  }
}

static void colorWithModule_otherAccess
  (Expression *expr, Value *value, SourceLoc loc)
{
  xassert(value);
  StringRef module = moduleForLoc(loc);

  // tell the user what we are doing
  if (oinkCmd->report_colorings) {
    std::cout << sourceLocManager->getFile(loc) << ":" <<
      sourceLocManager->getLine(loc);
    std::cout << " Expression";
//     if (name) std::cout << " '" << name << "'";
//     else std::cout << " <no-name>";
    char *name = prettyPrintASTNode(expr);
    std::cout << " '" << name << "'";
    free(name);
    std::cout << std::endl;
  }

  // for each source module not equal to us, make the write target
  SFOREACH_OBJLIST(char, moduleList, iter) {
    StringRef srcModule = iter.data();
    if (srcModule == module) continue;

    // get the qualifier
    stringBuilder qconstName("$");
    qconstName << srcModule;
    qconstName << "_otherAccess";
    LibQual::Type_qualifier *qconst = LibQual::find_qual(qconstName.c_str());
    USER_ASSERT(qconst, value->loc, "Literal '%s' not in lattice",
                strdup(qconstName.c_str()));

    // print out a line for each
    if (oinkCmd->report_colorings) {
      std::cout << " colored " << qconstName << std::endl;
    }

    // color the value with the module
    qa(value)->attachOneLiteral(value->loc, qconst);
  }
}

// Qual_ModuleAlloc_Visitor ****************

// visit all of the allocation points
class Qual_ModuleAlloc_Visitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  // map defined classes to their modules
  StringRefMap<char const> &classFQName2Module;
  // list of class typedef variables
  SObjList<Variable_O> &classVars;

  bool color_alloc;          // ref-level coloring of allocated memory
  bool color_otherControl;   // value-level coloring of other-control memory

  // allocators that we saw
  SObjSet<E_funCall*> seenAllocators;
  // allocators that were inside a cast expression
  SObjSet<E_funCall*> castedAllocators;

  public:
  Qual_ModuleAlloc_Visitor(StringRefMap<char const> &classFQName2Module0,
                           SObjList<Variable_O> &classVars0,
                           bool color_alloc0, bool color_otherControl0)
    : loweredVisitor(this)
    , classFQName2Module(classFQName2Module0)
    , classVars(classVars0)
    , color_alloc(color_alloc0)
    , color_otherControl(color_otherControl0)
  {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  void colorWithModule_alloc
  (Value *value, StringRef module, SourceLoc loc,
   char const *name, char const *astNode);
  void colorWithModule_otherControl
  (Value *value, StringRef module, SourceLoc loc,
   char const *name, char const *astNode);

  // color the data members of each class the same as their containing
  // class
  void colorClassMembers();

  virtual bool visitDeclarator(Declarator *);
  virtual void postvisitExpression(Expression *);

  virtual void subPostVisitE_funCall(E_funCall *);
  virtual void subPostVisitE_cast(E_cast *);
  virtual void subPostVisitE_new(E_new *);
};

void Qual_ModuleAlloc_Visitor::colorWithModule_alloc
  (Value *value, StringRef module, SourceLoc loc,
   char const *name, char const *astNode)
{
  xassert(value);
  if (!module) {
    module = moduleForLoc(loc);
  }

  // if the type is a compound type, check the module of the compound
  // type is the same as the current module
  Type *valueType = value->t();
  if (valueType->isCompoundType()) {
    CompoundType *ctype = valueType->asCompoundType();
    Variable_O *typedefVar = asVariable_O(ctype->typedefVar);
    StringRef cmodule =
      classFQName2Module.get
      (globalStrTable(typedefVar->fullyQualifiedMangledName0().c_str()));
    if (!cmodule) {
      userFatalError(loc, "class %s does not map to a module",
                     typedefVar->fullyQualifiedMangledName0().c_str());
    } else {
      if (module != cmodule) {
        userFatalError
          (loc, "class %s allocated in module %s but defined in module %s",
           typedefVar->fullyQualifiedMangledName0().c_str(),
           module, cmodule);
      }
    }
    // otherwise, we're ok
  }

  // get the qualifier
  stringBuilder qconstName("$");
  qconstName << module;
  qconstName << "_alloc";
  LibQual::Type_qualifier *qconst = LibQual::find_qual(qconstName.c_str());
  USER_ASSERT(qconst, value->loc, "Literal '%s' not in lattice",
              strdup(qconstName.c_str()));

  // tell the user what we are doing
  if (oinkCmd->report_colorings) {
    std::cout << sourceLocManager->getFile(loc) << ":"
              << sourceLocManager->getLine(loc);
    std::cout << " " << astNode;
    if (name) std::cout << " '" << name << "'";
    else std::cout << " <no-name>";
    std::cout << " colored " << qconstName << std::endl;
  }

  // color the value with the module
  qa(value)->attachOneLiteral(value->loc, qconst);

  // if the type is an array, recurse into the contained type
  if (value->isArrayValue()) {
    colorWithModule_alloc(value->getAtValue(), module, loc, name, astNode);
  }
}

void Qual_ModuleAlloc_Visitor::colorWithModule_otherControl
  (Value *value, StringRef module, SourceLoc loc,
   char const *name, char const *astNode)
{
  xassert(value);
  if (!module) {
    module = moduleForLoc(loc);
  }

  // tell the user what we are doing
  if (oinkCmd->report_colorings) {
    std::cout << sourceLocManager->getFile(loc) << ":" <<
      sourceLocManager->getLine(loc);
    std::cout << " " << astNode;
    if (name) std::cout << " '" << name << "'";
    else std::cout << " <no-name>";
    std::cout << std::endl;
  }

  // for each source module not equal to us, make the otherControl target
  SFOREACH_OBJLIST(char, moduleList, iter) {
    StringRef srcModule = iter.data();
    if (srcModule == module) continue;

    // get the qualifier
    stringBuilder qconstName("$");
    qconstName << srcModule;
    qconstName << "_otherControl";
    LibQual::Type_qualifier *qconst = LibQual::find_qual(qconstName.c_str());
    USER_ASSERT(qconst, value->loc, "Literal '%s' not in lattice",
                strdup(qconstName.c_str()));

    // print out a line for each
    if (oinkCmd->report_colorings) {
      std::cout << " colored " << qconstName << std::endl;
    }

    // NOTE: this is a value-level qualifier on a pointer
    xassert(value->isPointerValue());

    // color the value with the module
    qa(value)->attachOneLiteral(value->loc, qconst);
  }

  // if the type is an array, recurse into the contained type
  if (value->isArrayValue()) {
    colorWithModule_otherControl(value->getAtValue(), module, loc,
                                 name, astNode);
  }
}

void Qual_ModuleAlloc_Visitor::colorClassMembers() {
  SFOREACH_OBJLIST(Variable_O, classVars, iter) {
    Variable_O const *typedefVar = iter.data();
    StringRef module = classFQName2Module.get
      (globalStrTable(typedefVar->fullyQualifiedMangledName0().c_str()));
    xassert(module);
    Scope *cpdScope = typedefVar->type->asCVAtomicType()
      ->atomic->asCompoundType();
    xassert(cpdScope->scopeKind == SK_CLASS);
    for (StringRefMap<Variable>::Iter iter = cpdScope->getVariableIter();
         !iter.isDone(); iter.adv())
    {
      Variable_O *var = asVariable_O(iter.value());
      Value *varValue = var->abstrValue()->asRval();
      if (color_alloc) {
        colorWithModule_alloc(varValue, module, varValue->loc, var->name,
                              "Declarator");
      }
      if (color_otherControl) {
        if (varValue->isPointerValue()) {
          colorWithModule_otherControl(varValue, module, varValue->loc,
                                       var->name, "Declarator");
        }
      }
    }
  }
}

bool Qual_ModuleAlloc_Visitor::visitDeclarator(Declarator *obj) {
  // we handle E_new in a separate pass
  if (obj->context == DC_E_NEW) return true;

  Value *varValue = asVariable_O(obj->var)->abstrValue()->asRval();
  if (color_alloc) {
    colorWithModule_alloc(varValue, NULL, varValue->loc, obj->var->name,
                          "Declarator");
  }
  if (color_otherControl) {
    if (varValue->isPointerValue()) {
      colorWithModule_otherControl(varValue, NULL, varValue->loc,
                                   obj->var->name, "Declarator");
    }
  }
  return true;
}

void Qual_ModuleAlloc_Visitor::postvisitExpression(Expression *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case Expression::E_FUNCALL:
    subPostVisitE_funCall(obj->asE_funCall());
    break;
  case Expression::E_CAST:
    subPostVisitE_cast(obj->asE_cast());
    break;
  case Expression::E_NEW:
    subPostVisitE_new(obj->asE_new());
    break;
  }
}

void Qual_ModuleAlloc_Visitor::subPostVisitE_funCall(E_funCall *obj) {
  if (isAllocator(obj, getLoc())) {
    // record the allocator so we can look for missed allocators later
    seenAllocators.add(obj);
  }
}

void Qual_ModuleAlloc_Visitor::subPostVisitE_cast(E_cast *obj) {
  // see if it is a cast from an allocator
  Expression *expr0 = obj->expr->skipGroups();
  if (expr0->isE_funCall() && isAllocator(expr0->asE_funCall(), getLoc())) {
    if (color_alloc) {
      // This is subtle: attach the color to the 1) ref value of the
      // 2) thing pointed to by 3) the cast expresion; thanks to Matt
      // Harren for help on this.
      Value *castValue = obj->abstrValue
        ->getAtValue()  // color the value pointed-to, not the pointer
        ->asRval();
      colorWithModule_alloc(castValue, NULL, castValue->loc,
                            funCallName(expr0->asE_funCall()),
                            "E_cast-allocator");
    }
    if (color_otherControl) {
      // This is subtle: attach the color to the 1) pointer value of
      // the 2) the cast expresion; thanks to Matt for help on this.
      Value *castValue = obj->abstrValue->asRval();
      if (castValue->isPointerValue()) {
        colorWithModule_otherControl(castValue, NULL, castValue->loc,
                                     funCallName(expr0->asE_funCall()),
                                     "E_cast-allocator");
      }
    }
    // check off the allocator so we can look for missed allocators later
    castedAllocators.add(expr0->asE_funCall());
  }
}

void Qual_ModuleAlloc_Visitor::subPostVisitE_new(E_new *obj) {
  // attach the color to the expression ref value
  Value *value0 = obj->abstrValue->asRval();

  // Note: new is not malloc.  Malloc is just an allocator and so the
  // memory allocated should get colored with the module of the source
  // where it is; this forces a module to wrap ctors around its malloc
  // calls.  New on the other hand is an allocator *and* an
  // initializer, so should simply color the memory with the module
  // color of the class of the instances that it news, as long as that
  // class has a name; if the class is unnamed, then the semantics
  // should revert to that of malloc: coloring with the module that
  // calls the new.
  StringRef module = NULL;
  // type of a E_new is a pointer to the thing that it news, so we
  // have to get rid of one pointer layer
  Type *valueType = value0->getAtValue()->t();
  if (valueType->isCVAtomicType()) {
    CVAtomicType *cvatype = valueType->asCVAtomicType();
    AtomicType *atype = cvatype->atomic;
    if (atype->isCompoundType()) {
      CompoundType *ctype = atype->asCompoundType();
      if (ctype->keyword == CompoundType::K_STRUCT
          || ctype->keyword == CompoundType::K_CLASS) {
        Variable_O *typedefVar = asVariable_O(ctype->typedefVar);
        module = classFQName2Module.get
          (globalStrTable(typedefVar->fullyQualifiedMangledName0().c_str()));
        if (!module) {
          userFatalError(value0->loc, "class %s does not map to a module",
                         typedefVar->fullyQualifiedMangledName0().c_str());
        }
      }
    }
  }

  // FIX: could print the type name here
  if (color_alloc) {
    // This is subtle: attach the color to the 1) ref value of the 2)
    // thing pointed to by 3) the new expresion; thanks to Matt for
    // help on this.
    Value *newValue = obj->abstrValue
      ->getAtValue()    // color the value pointed-to, not the pointer
      ->asRval();
    colorWithModule_alloc(newValue, module, newValue->loc, NULL, "E_new");
  }
  if (color_otherControl) {
    xassert(value0->isPointerValue());
    colorWithModule_otherControl(value0, module, value0->loc, NULL, "E_new");
  }
}

// Qual_ModuleOtherWrite_Visitor ****************

// Write points:
// E_assign
//      =
//      *= and others
// E_effect
//      ++
//      --
// Initializer
//      IN_expr: int x = 3;
//      IN_compound: int x[] = { 1, 2 }
// * C++ only:
// Initializer
//      IN_ctor
// MemberInit
//      member initializers
// Handler
//      catch clause

// visit all of the write points
class Qual_ModuleOtherWrite_Visitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  public:
  Qual_ModuleOtherWrite_Visitor() : loweredVisitor(this) {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual bool visitDeclarator(Declarator *obj);
  virtual bool visitMemberInit(MemberInit *obj);
  virtual bool visitHandler(Handler *obj);
  virtual void postvisitExpression(Expression *);

  virtual bool subVisitE_effect(E_effect *);
  virtual bool subVisitE_assign(E_assign *);
};

bool Qual_ModuleOtherWrite_Visitor::visitDeclarator(Declarator *obj) {
  Value *varValue = asVariable_O(obj->var)->abstrValue()->asRval();
  if (obj->init) {
    if (IN_ctor *ctor = dynamic_cast<IN_ctor*>(obj->init)) {
      if (!obj->ctorStatement) {
        // IN_ctor for a simple type that does not have an implicit ctor,
        // such as an int.
        xassert(!varValue->t()->isCompoundType());
        USER_ASSERT(ctor->args->count()==1, getLoc(),
                    "Non-compound type must have one-arg MemberInit"
                    " (e5a12447-7229-4e2c-bc23-db855954f06e)");
        colorWithModule_otherWrite(varValue, varValue->loc, obj->var->name,
                                   "Declarator-IN_ctor");
      }
      // otherwise, the ctorStatement was handled above.
    } else if (
               // get rid of gcc unused warning
//                 IN_expr *expr =
               dynamic_cast<IN_expr*>(obj->init)) {
      if (!obj->ctorStatement) {
        // **** from the dataflow analysis:
        //
        // This is wrong, it could be calling a ctor.  Check for the
        // ctor and do a function call instead here if there is one.
        // 9sep2003: update: Scott isn't sure about the semantics of
        // IN_expr; keep doing this for now
        //
        // UPDATE: the ctorStatement above handles this
        //
        // UPDATE: FIX: GATED_EDGE: I need to omit this edge for the
        // special case of a string literal being used to initialize
        // an array.  The real answer is that a constructor for the
        // array should be called with the string literal that
        // *copies* it to the array; note that while a string literal
        // is an array of 'char const'-s it can be used to initialize
        // an array of (nonconst) chars.  That is, this is fine;
        // 'fmt[0]' is writable:
        //
        //   static char fmt[80] = "abort %d (%x); MSCP free pool: %x;";
        // however this is not; it is unspecified what happens if you
        // write 'fmt[0]':
        //   static char *fmt = "abort %d (%x); MSCP free pool: %x;";

        //          if (! (expr->e->isE_stringLit() &&
        //                 varValue->isStringType()) ) {
        //          if (! (expr->e->isE_stringLit()) ) {
        //            dfe.eDataFlow_normal(expr->e->abstrValue, varValue, loc);
        //          }
        colorWithModule_otherWrite(varValue, varValue->loc, obj->var->name,
                                   "Declarator-IN_expr");
      }
    } else if (
               // get rid of gcc unused warning
//                 IN_compound *ci =
               dynamic_cast<IN_compound*>(obj->init)) {
      // **** from the dataflow analysis:
      //
      // FIX: I think it is more elegant and local here to use the
      // declType.
      // Scott puts this in, though he doesn't say that he knows its
      // right, so I'll let it go through.
      //        xassert(!obj->ctorStatement);

//       try {
//         compoundInit(&dfe, loc, varValue, ci,
//                      oneAssignmentDataFlow, reportUserErrorDataFlow);
//       } catch (UserError &err) {
//         // dsw: there are some compound initializers situations where
//         // gcc just gives a warning instead of an error, such as if
//         // you put too many elements into an array:
//         // oink/Test/too_many_initializers1.c; here I turn the error
//         // into a warning so we can at least process such files
//         userReportWarning
//           (loc, stringc << "above error turned into a warning; "
//            "this is an unsoundness in the dataflow");
//       }

      // FIX: we should actually set it to deeply write
      userReportWarning(getLoc(), "unimplemented: compound initializers"
                        " should be a deep write");
      colorWithModule_otherWrite(varValue, varValue->loc, obj->var->name,
                                 "Declarator-IN_compound");
    } else xfailure("can't happen");
  }
  return true;
}

bool Qual_ModuleOtherWrite_Visitor::visitMemberInit(MemberInit *obj) {
  if (!obj->ctorStatement) {
    // MemberInit for a simple type that does not have an implicit
    // ctor, such as an int.
    // FIX: make this instance-specific; (did I mean to imitate this?)

    // obj->receiver->type->asRval()->asCVAtomicValue()->
    //   getInstanceSpecificValue();

    Value *memberValue0 = asVariable_O(obj->member)->abstrValue();
    xassert(!memberValue0->t()->isCompoundType());
    USER_ASSERT(obj->args->count()==1, getLoc(),
                "Non-compound type must have one-arg MemberInit"
                " (7ec11921-4016-4649-8120-2ad50f9a73ce)");
    colorWithModule_otherWrite(memberValue0, memberValue0->loc,
                               obj->member->name, "MemberInit");
  }
  return true;
}

bool Qual_ModuleOtherWrite_Visitor::visitHandler(Handler *obj) {
  if (obj->globalVar) {
    Value *tgt0 = asVariable_O(obj->typeId->decl->var)->abstrValue()->asRval();
    colorWithModule_otherWrite(tgt0, tgt0->loc, obj->typeId->decl->var->name,
                               "Handler");
  } else {
    xassert(!obj->globalDtorStatement);
  }
  return true;
}

void Qual_ModuleOtherWrite_Visitor::postvisitExpression(Expression *obj) {
  switch(obj->kind()) {         // roll our own virtual dispatch
  default: break;               // expression kinds for which we do nothing
  case Expression::E_EFFECT: subVisitE_effect(obj->asE_effect()); break;
  case Expression::E_ASSIGN: subVisitE_assign(obj->asE_assign()); break;
  }
}

bool Qual_ModuleOtherWrite_Visitor::subVisitE_effect(E_effect *obj) {
  // NOTE: In the dataflow analysis, we flow from the
  // obj->expr->abstrValue to the obj->abstrValue; however it is the
  // obj->expr->abstrValue that is being mutated
  xassert(obj->expr->abstrValue->isReferenceValue());
  Value *value0 = obj->expr->abstrValue->asRval();
  char const *name0 = NULL;
  if (obj->expr->skipGroups()->isE_variable()) {
    name0 = obj->expr->skipGroups()->asE_variable()->name->getName();
  }
  colorWithModule_otherWrite(value0, value0->loc, name0, "E_effect");
  return true;
}

bool Qual_ModuleOtherWrite_Visitor::subVisitE_assign(E_assign *obj) {
  // should not get targets here which are compound types or
  // references to such, since it should have been resolved into a
  // call to an operator assign, which always exists implicitly.
  Value *tgt = obj->target->abstrValue;
  // remove l-value layer of indirection for ASSIGNMENT (not for
  // INITIALIZATION).
  USER_ASSERT(tgt->isReferenceValue(), getLoc(),
              "Left side of assignment must be an lvalue");
  Value *tgt1 = tgt->asRval();
  char const *name0 = NULL;
  if (obj->target->skipGroups()->isE_variable()) {
    name0 = obj->target->skipGroups()->asE_variable()->name->getName();
  }
  colorWithModule_otherWrite(tgt1, tgt1->loc, name0, "E_assign");
  return true;
}

// Qual_ModuleAccess_Visitor ****************

// an access point is any L-value expression (an expression with Ref
// type).

// visit all of the access points
class Qual_ModuleAccess_Visitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  bool otherAccess;             // color with our access or other accesses?

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  public:
  Qual_ModuleAccess_Visitor(bool otherAccess0)
    : loweredVisitor(this)
    , otherAccess(otherAccess0)
  {}

  virtual bool visitExpression(Expression *);
};

bool Qual_ModuleAccess_Visitor::visitExpression(Expression *obj) {
  // FIX: this is ad-hoc, however there are expressions without a type
  if (!obj->type) return true;

  // This computation decides if an expression is a method call
  bool isMethodCall = false;
  if (obj->isE_funCall()) {
    E_funCall *funCall = obj->asE_funCall();
    Expression *funcExpr = funCall->func->skipGroups();
    if (funcExpr->isE_fieldAcc()) {
      E_fieldAcc *e_field = funcExpr->asE_fieldAcc();
      Value *fun0 = asVariable_O(e_field->field)->abstrValue()->asRval();
      if (fun0->isFunctionValue() &&
          fun0->asFunctionValue()->type->isMethod()) {
        // method call
        isMethodCall = true;
        // the point of this is to not traverse funCall->func
        FAKELIST_FOREACH_NC(ArgExpression, funCall->args, iter) {
          iter->traverse(this->loweredVisitor);
        }
      }
    }
  }

  Value *exprValue = obj->abstrValue;
  if (exprValue->isReferenceValue()) {
    if (otherAccess) {
      colorWithModule_otherAccess(obj, exprValue->asRval(), exprValue->loc);
    } else {
      colorWithModule_access(obj, exprValue->asRval(), exprValue->loc);
    }
  }

  return !isMethodCall; // prune the subtree traversal if isMethodCall
}

// class MarkDecltorsInstanceSpecificVisitor ****************

#if DEBUG_INSTANCE_SPECIFIC_VALUES
bool MarkDecltorsInstanceSpecificVisitor::visitDeclarator(Declarator *obj) {
  Value *declValue = obj->abstrValue;
  if (!declValue->isFunctionValue()) {
    instanceSpecificValues.add(declValue);
    dataDeclaratorValues.add(declValue);
  }
  return true;
}
#endif

// class NameASTNodesVisitor ****************

bool NameASTNodesVisitor::visitExpression(Expression *ex) {
  // pretty print AST to name
  stringBuilder sb;
  StringBuilderOutStream out0(sb);
  CodeOutStream codeOut(out0);
  PrintEnv env(typePrinterOink, &codeOut);
  ex->print(env);
  codeOut.finish();

  // attach the name to the abstract value of the AST
  Value *v = ex->abstrValue;
  if (v) {
    qa(v)->nameTree(sb);
  }
  return true;                  // do the sub-expressions also
}

bool NameASTNodesVisitor::visitDeclarator(Declarator *obj) {
  Variable_O *var = asVariable_O(obj->var);
  return !var->filteredOut();
}

bool NameASTNodesVisitor::visitFunction(Function *obj) {
  Variable_O *var = asVariable_O(obj->nameAndParams->var);
  return !var->filteredOut();
}

// Qual ****************

void Qual::configure() {
  if (qualCmd->inference && !qualCmd->config) {
    userFatalError(SL_UNKNOWN, "Since you are doing inference, you "
                   "must supply a config file.\n");
  }
  init_libqual(qualCmd->config);
}


// Deserialization ****************

/* virtual */
void Qual::deserialize_1archive(ArchiveDeserializer *arc,
                                XmlReaderManager &manager)
{
  // NOTE: the XmlReaderManager is shared across both
  // de-serializations below: the deserialization of value_qual.map
  // needs the XmlReaderManager::id2obj.

  deserialize_formatVersion(arc);
  deserialize_files(arc, manager);
  deserialize_abstrValues(arc, manager);
  deserialize_valueQualMapAndQualGraph(arc, manager);
}

// virtual, overrides Oink
void Qual::printStats()
{
  LibQual::Qual_print_stats();

  std::cout << std::endl;

  Oink::printStats();

  size_t values = 0;
  size_t values_with_names = 0;
  size_t values_with_qvars = 0;

  FOREACH_VALUE(iter) {
    Value *v = iter.data<Value>();
    QualAnnot *a = qa(v);
#if !DELAY_SET_NAMES
    if (a->vname) ++values_with_names;
#endif
    if (a->hasQv()) ++values_with_qvars;
    ++values;
  }
  xassert(values == vFac->getNumValues());

#if DELAY_SET_NAMES
  extern size_t getNumQvarsWithNames(); // TODO XXX KLUDGE
  values_with_names = getNumQvarsWithNames();
#endif

  std::cout << "Values with names: " << values_with_names << std::endl;
  std::cout << "Values with qvars: " << values_with_qvars << std::endl;
}

// virtual, overrides Oink
void Qual::printSizes()
{
#define P(T) std::cout << "  sizeof(" #T ") = " << sizeof(T) << std::endl
  Oink::printSizes();

  std::cout << std::endl;

  std::cout << "Qual:\n";

  P(CVAtomicValue_Q);
  P(PointerValue_Q);
  P(ReferenceValue_Q);
  P(FunctionValue_Q);
  P(ArrayValue_Q);
  P(PointerToMemberValue_Q);

  std::cout << std::endl;

  P(CVAtomicValue_QualAnnot);
  P(PointerValue_QualAnnot);
  P(ReferenceValue_QualAnnot);
  P(FunctionValue_QualAnnot);
  P(ArrayValue_QualAnnot);
  P(PointerToMemberValue_QualAnnot);

  std::cout << std::endl;

  P(CVAtomicValue_Q::MyStruct);
  P(PointerValue_Q::MyStruct);
  P(ReferenceValue_Q::MyStruct);
  P(FunctionValue_Q::MyStruct);
  P(ArrayValue_Q::MyStruct);
  P(PointerToMemberValue_Q::MyStruct);

  std::cout << std::endl;

  LibQual::Qual_print_sizes();
#undef P
}

void Qual::srzFormat(ArchiveSrzFormat &srzfmt, bool writing)
{
  Oink::srzFormat(srzfmt, writing); // delegate to super class

  srzfmt.opt("instance_sensitive", qualCmd->instance_sensitive);
  srzfmt.opt("poly", qualCmd->poly);
}

void Qual::deserialize_abstrValues(ArchiveDeserializer *arc,
                                   XmlReaderManager &manager) {
  // use heap-allocated readers because if we get an exception, then the
  // underlying ASTList will 'delete' it upon stack unwinding.
  XmlTypeReader *typeReader = new XmlTypeReader;
  manager.registerReader(typeReader);
  XmlValueReader_Q *valueReader = new XmlValueReader_Q(*typeReader);
  manager.registerReader(valueReader);

  deserialize_abstrValues_toLinker(arc, manager);

  manager.unregisterReader(typeReader);
  manager.unregisterReader(valueReader);
}

// read " -> " from input stream; set badbit on error.
static inline std::istream& readArrow(std::istream &i) {
  char c1, c2;
  i >> std::ws >> c1 >> c2 >> std::ws;
  if (!i || c1 != '-' || c2 != '>') {
    i.setstate(std::ios::badbit);
  }
  return i;
}

// NOTE: if we ever want to read 64-bit files on 32-bit machines we'd
// have to change the type of qvarId to uint64_t, or not wrote
// pointers.

// Parse a line containing "valueId -> qvarId".  Throws exception on error.
static inline void parseValueQualMapLine
  (std::string const& line, std::string & valueId, unsigned long & qvarId)
{
  std::istringstream input(line);

  char c1, c2;
  input >> valueId >> readArrow >> c1 >> c2 >> qvarId;

  if (input.fail() || !input.eof() || c1 != 'Q' || c2 != 'V') {
    userFatalError(SL_UNKNOWN,
                   "parse error while reading value_qual.map line '%s'",
                   line.c_str());
  }
}

// Read a line. Returns true on success and false on eof.  Throws
// exception on read error.
static inline bool readValueQualMapLine(std::istream& in, std::string& line) {
  std::getline(in, line);

  if (in.eof() && line.empty()) {
    return false;
  }

  if (!in || line.empty()) {
    perror("failed while reading value_qual.map");
    throw UserError(USER_ERROR_ExitCode);
  }

  return true;
}

// Rob: "for each qualifier address on deserialized abstract values
// replace it using s18n_lookup_deserialized(...)"
void Qual::deserialize_valueQualMap_stream
  (std::istream& in, XmlReaderManager &manager,
   LibQual::s18n_context *serializationCtxt)
{
  std::string line;
  while(readValueQualMapLine(in, line)) {
    // read old value -> old qvar

    std::string oldValueId;
    unsigned long oldQvarPtr;
    parseValueQualMapLine(line, oldValueId, oldQvarPtr);

    // convert old value -> new value
    Value *value = (Value*) manager.getNodeById(oldValueId.c_str());
    if (!value) {
      printf("failed to find Value object while reading value_qual.map\n");
      breaker();
      throw UserError(USER_ERROR_ExitCode);
    }
    // FIX: should probably check that this is one of the kinds of Values
    //        int valueKind = manager.id2kind.get(oldValueId);

    // convert old qvar -> new qvar
    void *newQvarPtr = NULL;
    int foundIt = LibQual::s18n_lookup_deserialized
      (serializationCtxt, (void*) oldQvarPtr, &newQvarPtr);
    if (!foundIt) {
      userFatalError(SL_UNKNOWN,
                     "failed to find qvar %x while reading value_qual.map",
                     oldQvarPtr);
    }
    xassert(newQvarPtr);

    // make QualAnnot to mediate between Value and qvar and insert it
    // annotateWithQualAnnot(value);
    static_cast<QualAnnot*>(value->getAnnotation0())
      ->setQv_deserialize(static_cast<LibQual::Type_qualifier *>(newQvarPtr));
  }
}

// expect string EXPECTED, else set badbit on IN.
static inline std::istream& matchMarker(std::istream& in, std::string const& expected) {
  char * buf = (char*) alloca(expected.size());
  in.read(buf, expected.size());
  if (0 != memcmp(buf, expected.c_str(), expected.size())) {
    in.setstate(std::ios::badbit);
  }
  return in;
}

// expect string EXPECTED, else throw user error.
static inline void expectMarker
  (std::istream& in, std::string const& expected, const char* fname)
{
  if (!matchMarker(in, expected)) {
    userFatalError(SL_UNKNOWN,
                   "input error reading '%s', did not find %s marker",
                   fname, expected.c_str());
  }
}

static inline void expectEOF(std::istream& in, const char* fname) {
  in.peek(); // peek to force setting eofbit if we're at EOF.
  if (!in.eof()) {
    userFatalError(SL_UNKNOWN,
                   "input error reading '%s', did not reach end of file",
                   fname);
  }
}

void Qual::deserialize_valueQualMapAndQualGraph
  (ArchiveDeserializer* arc, XmlReaderManager &manager)
{
  // de-serialize the quals and the map

  std::istream& inValueQualMap = arc->input("value_qual.map");
  // std::stringstream tmpMapIn;
  // tmpMapIn.get(* inValueQualMap.rdbuf() );

  std::istream& inQual = arc->input("qual.dat");

  // check that we're reading the right file.
  expectMarker(inQual, QUAL_BOF_MAGIC, arc->curFname());

  // printStage("      reading quals");
  QvarSerializationContextHolder qctxt(&inQual);
  LibQual::merge_quals_graph(qctxt.ctxt); // merge in the file contents

  // printStage("      reading value-qual map");
  deserialize_valueQualMap_stream(inValueQualMap, manager, qctxt.ctxt);

  // check that we've read exactly what we expect to read.
  expectMarker(inQual, QUAL_EOF_MAGIC, arc->curFname());
  expectEOF(inQual, arc->curFname());

  // LibQual::check_all_edge_sets();
}


// Serialization ****************

// Mark the backend qualifier for all linker-visible abstract values as
// extern.  NOTE: This must be done before running compact_quals_graph().
void Qual::markExternVars() {
  printStage("markExternVars");
  // FIX: someday we should do the optimization where if there are no
  // other occurances of a variable in any other translation units
  // that it is no longer marked as external.

  // Remember: valueWriter is now a semi-generic visitor with a
  // slighly inaccurate name.

  // 1. mark everything as non-extern.
  LibQual::set_extern_all_qual(false);

  // 2. mark linker-visibles as extern.
  IdentityManager idmgr;
  QualAnnot_ExternVisit visitor(/* externness */ true);
  bool indent = false;
  int depth = 0;
  // "Serialize" the values.
  //
  // We DO NOT use a XVWQ_SerializeOracle because we are the ones
  // setting "externness", and XVWQ_SerializeOracle looks at whether
  // there are extern qvars.  However, we do need to avoid Variable-s
  // that are not getFilteredKeep(), so we use the
  // XVW_SerializeOracle.
  XmlValueWriter::XVW_SerializeOracle filterKeepSrzOracle;
  XmlValueWriter_Q valueWriter
    (idmgr,
     (ASTVisitor*)NULL,
     &visitor,
     NULL, // no serialization
     depth,
     indent,
     &filterKeepSrzOracle
     );

  TailList<Variable_O> externVars;

  linker.getOrderedExternVars(NULL, externVars);
  valueWriter.toXml_externVars(&externVars);

  xassert(depth == 0);

  this->numExternQvars = visitor.getCount();
}

void Qual::compactifyGraph() {
  // Compress the qualifier graph.  This reduces the size of the graph which
  // is good when serialized, though it requires more memory to do the
  // compaction.
  //
  // Compaction requires all externally visible qualifiers to be marked
  // 'extern'.

  printStage("   compact_quals_graph");
  LibQual::compact_quals_graph();

  // quarl 2006-05-14
  //    Update Type_qualifier pointers for all abstract values to point to
  //    their ECRs.  This is required before eliminate_quals_links().  This
  //    step used to be done during serialization after
  //    eliminate_quals_links() -- that was unclean and prevented
  //    eliminate_quals_links() from deleting anything; it is better to do it
  //    here, and it doesn't take much time to do an extra list traversal.
  //
  //    We really only need to do this for linker-visible values, but it's
  //    fast to update all.

  updateEcrAll();

  // After compaction, many qualifier nodes may be unified together.  This
  // passes over the graph, changing all references to those nodes to point
  // directly to the current ecr.
  printStage("   eliminate_quals_links");
  LibQual::eliminate_quals_links();
  // LibQual::check_all_edge_sets();

  // quarl 2006-05-15
  //    Remove pointers to the dead qvars so that the backend can get rid of
  //    them (currently it doesn't actually deallocate them, but it's still
  //    necessary because otherwise we'll try to use them in serialization,
  //    and their edge sets are no longer valid).
  removeDeadQvars();
}

int Qual::serialize_valuesAndMap
  (ArchiveSerializer* arc, LibQual::s18n_context *serializationCtxt)
{
  // write to a string first, since we can't interleave output to two archive
  // files
  std::ostream& mapOut = arc->outputAlt("value_qual.map");
  std::ostream& valueOut = arc->output("value.xml");

  IdentityManager idmgr;

  // set up to serialize the qvars as a side-effect
  QualSerialization_ValueVisitor qSerValVisitor(idmgr, mapOut,
                                                serializationCtxt);
  bool indent = tracingSys("xmlPrintAST-indent");
  int depth = 0;              // shared depth counter between printers

  XmlValueWriter_Q::XVWQ_SerializeOracle srzOracle;
  // serialize the values
  {
    XmlValueWriter_Q valueWriter(idmgr,
                                 (ASTVisitor*)NULL,
                                 &qSerValVisitor,
                                 &valueOut, depth, indent, &srzOracle);
    serialize_abstrValues_stream(valueWriter, serializeVar_O);
    xassert(depth == 0);
  }

  return qSerValVisitor.getNumSerializedQvars();
}

// actually serialize everything
void Qual::serialize_results() {
  printStage("serialize_results");
  // FIX: should be able to get rid of this one; it is only there so
  // you can ask if something has any qualifier literals that haven't
  // been attached; there shouldn't be any at this point
  Restorer<bool> restorer(value2typeIsOn, true);
  try {
    ArchiveSerializerP arc = archiveSrzManager->
      getArchiveSerializer(qualCmd->srz);

    serialize_formatVersion(arc.get());

    serialize_files(arc.get());

    // should have been set from result of markExternVars().
    xassert(this->numExternQvars != -1);

    std::ostream& outQual = arc->output("qual.dat");
    outQual << QUAL_BOF_MAGIC;

    QvarSerializationContextHolder qctxt(&outQual);
    // LibQual::check_all_edge_sets();
    printStage("   serialize_quals_graph");
    LibQual::serialize_quals_graph(qctxt.ctxt);

    // write a magic marker so we can check on deserialization
    outQual << QUAL_EOF_MAGIC;

    // LOUD-SERIALIZATION
//      printf("\nVALUE ****************\n");
    printStage("   serialize_valuesAndMap");
    int numSerializedQvars = serialize_valuesAndMap(arc.get(), qctxt.ctxt);

    // NOTE: this is a very important assertion; do not remove it.  If
    // this is failing, 1) print the qvars marked as extern by turning
    // on the print statement in QualAnnot_ExternVisit::setExtern, and
    // 2) the qvars serialized by turning on the print statement in
    // QualSerialization_ValueVisitor::preVisitValue; both are marked
    // LOUD-SERIALIZATION.
    if (numExternQvars != numSerializedQvars) {
      userFatalError(SL_UNKNOWN,
                     "INTERNAL ERROR: numExternQvars=%d, numSerializedQvars=%d",
                     numExternQvars, numSerializedQvars);
    }
    xassert(numExternQvars == numSerializedQvars);

  } catch(ArchiveIOException & e) {
    userFatalError(SL_UNKNOWN, "Failed to serialize %s: %s",
                   qualCmd->srz.c_str(), e.error.c_str());
  }
}

bool Qual::varUsedInDataflow(Variable_O *var) {
  bool myAnswer = asVariable_Q(var)->treeContainsQvars();
  // bool parentAnswer = Oink::varUsedInDataflow(var);

  // // myAnswer and parentAnswer should be the same.  run with both for a while
  // // to make sure; then we can drop this function altogether.
  // printf("## varUsedInDataflow: myAnswer=%d, parentAnswer=%d\n",
  //        myAnswer, parentAnswer);
  // xassert(myAnswer == parentAnswer);

  // it turns out treeContainsQvars() and Oink::varUsedInDataflow() are not
  // the same, because Oink::varUsedInDataflow() doesn't take into account
  // funky qualifiers.

  return myAnswer;
}

void Qual::unifyVars(Variable_O *v1, Variable_O *v2, SourceLoc loc) {
  Oink::unifyVars(v1, v2, loc); // delegate to superclass
  dfe.eDataFlow_refUnify(v1->abstrValue(), v2->abstrValue(), loc);
}


// Stages ****************

// callback for traversal using anyCtorSatisfiesF() to mark values as
// instance-specific
#if DEBUG_INSTANCE_SPECIFIC_VALUES
bool markValueInstanceSpecific(Value *t) {
//    std::cout << "Marking as instance-specific " << t->toString() << std::endl;
  instanceSpecificValues.add(t);
  return false; // keep traversing; a true would cause the search to prune
}
#endif

#if DEBUG_INSTANCE_SPECIFIC_VALUES
void markAsInstanceSpecific(Variable *var0) {
  Variable_Q *var = asVariable_Q(var0);
  if (!var->hasFlag(DF_TYPEDEF)) return;
  if (!var->type->isCompoundType()) return;
  CompoundType *ct = var->type->asCompoundType();
  for (StringRefMap<Variable>::Iter iter = ct->getVariableIter();
       !iter.isDone();
       iter.adv()) {
    Variable_O *var = asVariable_O(iter.value());
    if (!var->getReal()) continue; // only real vars get Values
    Value *v = var->abstrValue(); // NOTE: do NOT use getInstanceSpecificValue
    // I think namespaces don't have a type
    if (v && !v->isFunctionValue()) {
      // FIX: this is a violation of the notion of anyCtorSatisfies as
      // taking a predicate that does not modify the Value in
      // question.
      v->anyCtorSatisfiesF( (ValuePredicate*) markValueInstanceSpecific );
    }
  }
}
#endif

void Qual::markInstanceSpecificValues_stage() {
#if DEBUG_INSTANCE_SPECIFIC_VALUES
  printStage("markInstanceSpecificValues");
  Restorer<bool> restorer(value2typeIsOn, true);
  // NOTE: I don't think that I want to visit just the
  // non-filtered-out ones here; this marking is in support of an
  // assertion that I think should visit all the real vars.
  VisitRealVars visMarkInstSpec(markAsInstanceSpecific); // VAR-TRAVERSAL
  visitVarsMarkedRealF(builtinVars, visMarkInstSpec);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);

    // for every variable that is a CompoundType, mark as
    // instance-specific the value tree of all of its data members
    visitRealVarsF(unit, visMarkInstSpec);

    // for all the Declarators, mark its value instance-specific; we want
    // to make sure that we use the value of the var of the Declarator
    // instead
    MarkDecltorsInstanceSpecificVisitor env;
    unit->traverse(env.loweredVisitor);
  }
#endif
}

void Qual::moduleAlloc_stage() {
  printStage("moduleAlloc");
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    Qual_ModuleAlloc_Visitor env(*classFQName2Module,
                                 *classVars,
                                 true, // label alloc memory
                                 false // don't label other-control memory
                                 );
    env.colorClassMembers();
    unit->traverse(env.loweredVisitor);
    // check that we didn't hit any allocators that were not down
    // inside a cast
    for(SObjSetIter<E_funCall*> seenIter(env.seenAllocators);
        !seenIter.isDone(); seenIter.adv()) {
      E_funCall *call0 = seenIter.data();
      if (!env.castedAllocators.contains(call0)) {
        // this is just luck that I have this
        SourceLoc loc = call0->abstrValue->loc;
        userReportWarning
          (loc, "(access) allocator that was not inside a cast expression");
      }
    }
  }
}

void Qual::moduleOtherControl_stage() {
  printStage("moduleOtherControl");
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    Qual_ModuleAlloc_Visitor env(*classFQName2Module,
                                 *classVars,
                                 false, // don't label alloc memory
                                 true // label other-control memory
                                 );
    env.colorClassMembers();
    unit->traverse(env.loweredVisitor);
    // check that we didn't hit any allocators that were not down inside
    // a cast
    for(SObjSetIter<E_funCall*> seenIter(env.seenAllocators);
        !seenIter.isDone(); seenIter.adv()) {
      E_funCall *call0 = seenIter.data();
      if (!env.castedAllocators.contains(call0)) {
        // this is just luck that I have this
        SourceLoc loc = call0->abstrValue->loc;
        userReportWarning(loc, "(trust) allocator that was not inside "
                          "a cast expression");
      }
    }
  }
}

void Qual::moduleOtherWrite_stage() {
  printStage("moduleOtherWrite");
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    Qual_ModuleOtherWrite_Visitor env;
    unit->traverse(env.loweredVisitor);
  }
}

void Qual::moduleAccess_stage() {
  printStage("moduleAccess");
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    Qual_ModuleAccess_Visitor env(false // color OUR accesses, NOT others
                                  );
    unit->traverse(env.loweredVisitor);
  }
}

void Qual::moduleOtherAccess_stage() {
  printStage("moduleOtherAccess");
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    Qual_ModuleAccess_Visitor env(true // color OTHER accesses, NOT ours
                                  );
    unit->traverse(env.loweredVisitor);
  }
}

void Qual::moduleAnalysis_stages() {
  if (moduleList.isNotEmpty()) {
    build_classFQName2Module();
  }

  // access analysis
  if (qualCmd->module_access) {
    if (qualCmd->module_write) { // exclusive with write analysis
      throw UserError
        (USER_ERROR_ExitCode,
         "module-access and module-write don't make sense together");
    }
    if (moduleList.isEmpty()) {
      throw UserError(USER_ERROR_ExitCode,
                      "to use module analyses, you must supply modules");
    }
    moduleAlloc_stage();
    moduleOtherAccess_stage();
  }

  // write analysis
  if (qualCmd->module_write) {
    xassert(!qualCmd->module_access); // exclusive with access analysis
    if (moduleList.isEmpty()) {
      throw UserError(USER_ERROR_ExitCode,
                      "to use module analyses, you must supply modules");
    }
    moduleAlloc_stage();
    moduleOtherWrite_stage();
  }

  // trust analysis
  if (qualCmd->module_trust) {
    if (moduleList.isEmpty()) {
      throw UserError(USER_ERROR_ExitCode,
                      "to use module analyses, you must supply modules");
    }
    moduleOtherControl_stage();
    moduleAccess_stage();
  }
}

static void setGlobalTreeOnVar(Variable *var) {
  asVariable_Q(var)->setGlobalTree();
}

void Qual::qualCompile_setGlobalness() {
  printStage("   setGlobalness");
  VisitRealVars_filter visSetGlobalTree(setGlobalTreeOnVar); // VAR-TRAVERSAL
  visitVarsMarkedRealF_filtered(builtinVars, visSetGlobalTree);
  // TODO: factor this loop into a macro
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    visitRealVarsF_filtered(unit, visSetGlobalTree);
  }
}

void Qual::qualCompile_nameExpressions() {
#if !DELAY_SET_NAMES
  if (qualCmd->name_expressions) {
    printStage("   nameExpressions");
    doNameExpressions();
  }
#endif
}

static void nameTreeOnVar(Variable *var) {
  xassert(!asVariable_O(var)->filteredOut());
  asVariable_Q(var)->nameTree();
}

void Qual::doNameVars()
{
  VisitRealVars_filter visNameTree(nameTreeOnVar); // VAR-TRAVERSAL
  visitVarsMarkedRealF_filtered(builtinVars, visNameTree);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    visitRealVarsF_filtered(unit, visNameTree);
  }
}

void Qual::doNameExpressions()
{
  // name the values that annotate the ast
  NameASTNodesVisitor nv;
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    unit->traverse(nv.loweredVisitor);
  }
}

void Qual::doNameUnnamed()
{
  // name things the nameTree pass didn't reach.
  FOREACH_VALUE(iter) {
    Value *v = iter.data<Value>();
    QualAnnot *a = qa(v);
    a->maybeSetNameMissingIfMissing();
  }
}

void Qual::qualCompile_nameVars()
{
#if !DELAY_SET_NAMES
  // name the values that annotate qvars
  if (qualCmd->name_vars) {
    printStage("   nameVars");
    doNameVars();
  }
#endif
}

// XXX FIXME KLUDGE
extern Qual *theQual;
void Qual_doAssignNames()
{
  theQual->doAssignNames();
}

void Qual::doAssignNames()
{
  printStage("      assign names");
  Restorer<bool> restorer(value2typeIsOn, true);
  doNameVars();
  doNameExpressions();
  doNameUnnamed();
}

// FIX: class VisitRealVars_ValueVisitor should go into value.cc/.h
// but I have trouble including cc_ast_aux.h into value.h; since this
// is the only place it is used I put it here for now.

// carry a ValueVisitor to all of the real Variables
class VisitRealVars_ValueVisitor : public VisitRealVars_filter {
  public:
  // data
  ValueVisitor *valueVis;

  // tor
  explicit VisitRealVars_ValueVisitor(ValueVisitor *valueVis0)
    : VisitRealVars_filter(NULL), valueVis(valueVis0)
  {}

  // only visits each Variable once
  virtual void visitVariableIdem(Variable *var0);
};

void VisitRealVars_ValueVisitor::visitVariableIdem(Variable *var0) {
  Variable_O *var = asVariable_O(var0);
  if (!var->hasFlag(DF_TYPEDEF)) return;
  Value *v = var->abstrValue();
  if (valueVis->preVisitValue(v)) {
    valueVis->postVisitValue(v);
  }
}

void Qual::qualCompile_qualVisitation() {
  printStage("   qualVisitation");
  supportedOperators.add("_op_deref", NULL);
  supportedOperators.add("_op_gt", NULL);

  // This has to wait until after the cqual infrastructure has been
  // initialized.
  thrownQv = make_qvar_CQUAL(
#if !DELAY_SET_NAMES
    "generic exception",
#endif
    SL_UNKNOWN, 0, 1);
  FOREACH_ASTLIST(char, qualCmd->catch_qual, iter) {
    LibQual::Type_qualifier *qconst = findQualAndCheck(iter.data());
    // FIX: would be nice to make a "command line loc" rather than UNKNOWN
    attachOneLiteralToQvar(SL_UNKNOWN, qconst, thrownQv);
  }

  // visit variables and mark their stackness
  if (qualCmd->stackness) {
    printStage("     stackness");
    StacknessMarker stackSM(true /*on stack*/);
    StacknessMarker nonstackSM(false /*not on stack*/);
    MarkStackness_ValueVisitor stack_markStackness(stackSM);
    MarkStackness_ValueVisitor nonstack_markStackness(nonstackSM);
    // visit the variables
    MarkVarsStackness_VisitRealVars visVarStackness
      (stack_markStackness, nonstack_markStackness); // VAR-TRAVERSAL
    // can't think of any reason to visit the built-ins
    //   visitVarsMarkedRealF_filtered(builtinVars, visVarStackness);
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      visitRealVarsF_filtered(unit, visVarStackness);
    }
    // visit the AST
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      MarkAllocStackness_Visitor env(stack_markStackness,
                                     nonstack_markStackness);
      unit->traverse(env.loweredVisitor);
      // check that we didn't hit any allocators that were not down
      // inside a cast
      for(SObjSetIter<E_funCall*> iter2(env.seenAllocators);
          !iter2.isDone(); iter2.adv()) {
        E_funCall *call0 = iter2.data();
        if (!env.castedAllocators.contains(call0)) {
          // this is just luck that I have this
          SourceLoc loc = call0->abstrValue->loc;
          userReportWarning(loc, "(stackness) allocator that was not "
                            "inside a cast expression");
        }
      }
    }
  }

  // Attach the parameters and return value literals to functions.
  //
  // FIX: I don't know if this has to come before the main value
  // visitation, but I'll do it anyway.
  //
  // FIX: I think this should be implemented as a special case of
  // special qualifiers below.
  if (qualCmd->func_param_ret_qual.count() > 0) {
    // set up to visit the values and mark them with the qualifiers
    FuncParamRetQual_ValueVisitor fprValueVis(dfe);
    // set up to visit all the top-level Declarator FunctionType
    // Variables in the AST
    FuncDeclVar_ASTVisitor fprFuncDeclVarASTVis(fprValueVis);
    // Note that I omit visiting the builtinVars; do each translation
    // unit
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      // This traversal visits variables in Declarators; this is a
      // more primitive way of visiting variables, as, while still a
      // lowered traversal, it does not taking into account filtering
      // etc.  Therefore this should probably be replaced by a proper
      // variable traversal.  However I don't need most variables to
      // be visited; therefore I'll leave it this way for now.
      unit->traverse(fprFuncDeclVarASTVis.loweredVisitor);
    }
  }

  // Implement special qualifiers.
  if (qualCmd->const_when_param.count() > 0) {
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      FOREACH_ASTLIST_NC(QSpec, qualCmd->const_when_param, iter) {
        QSpec *qspec = iter.data();
        MarkConstWhenParam_ASTVisitor cwpVis(qspec);
        unit->traverse(cwpVis.loweredVisitor);
      }
    }
  }

  // also do the compound containment pointers: to/from
  // struct/class/union members and their whole container
  StructuralFlow_ValueVisitor *sfValueVis = NULL;
  VisitRealVars_ValueVisitor *sfRealVarValueVis = NULL; // VAR-TRAVERSAL
  if (qualCmd->flow_compoundUp || qualCmd->flow_compoundDown ||
      qualCmd->flow_pointUp || qualCmd->flow_pointDown ||
      qualCmd->flow_refUp || qualCmd->flow_refDown) {

    // set up to visit the values in the AST
    sfValueVis = new StructuralFlow_ValueVisitor(dfe);
    sfValueVis->compoundUp   = qualCmd->flow_compoundUp;
    sfValueVis->compoundDown = qualCmd->flow_compoundDown;
    sfValueVis->pointUp      = qualCmd->flow_pointUp;
    sfValueVis->pointDown    = qualCmd->flow_pointDown;
    sfValueVis->refUp        = qualCmd->flow_refUp;
    sfValueVis->refDown      = qualCmd->flow_refDown;

    // set up to visit all the Variables
    sfRealVarValueVis = new VisitRealVars_ValueVisitor(sfValueVis);
    // visit the builtinVars
    visitVarsMarkedRealF_filtered(builtinVars, *sfRealVarValueVis);
  }

  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    // do qual visitation
    QualVisitor env(unit, *this, dfe);
    unit->traverse(env.loweredVisitor);
    // do structural flow visitation
    if (sfValueVis) {
      xassert(sfRealVarValueVis);
      // There are two kinds of traversal going on here because the
      // AST traversal just visits Values that are part of the AST
      // whereas the Variable traversal visits values that belong to
      // Variables.
      FindValueASTVisitor fvastvis(*sfValueVis);
      unit->traverse(fvastvis.loweredVisitor);
      visitRealVarsF_filtered(unit, *sfRealVarValueVis);
    }
  }

  if (sfValueVis) delete sfValueVis;
  if (sfRealVarValueVis) delete sfRealVarValueVis;
}


// Exclude data declarator qualifier combinations ****************

// exclude qualifier specifications in certain contexts in the data
// Value tree; by a 'data' value we mean to prune when we encounter a
// function
class ExcludeQualData_ValueVisitor : public ValueVisitor {
  public:
  // Exclude declarators containing the given qualifier.
  ASTList<char> &exclude;
  const char * const context;

  ExcludeQualData_ValueVisitor(ASTList<char> &exclude0, const char * const context0)
    : exclude(exclude0)
    , context(context0)
  {}

  public:
  virtual bool preVisitValue(Value *obj);
};

bool ExcludeQualData_ValueVisitor::preVisitValue(Value *obj) {
  // since this is a data-value walk prune the walk at function values
  if (obj->isFunctionValue()) return false;
  // otherwise check for the specified qualifier
  QualAnnot *qannot = qa(obj);
  for(QLiterals *ql0=qannot->getQl(); ql0; ql0=ql0->next) {
    FOREACH_ASTLIST(char, exclude, iter) {
      char const * const qualName = iter.data();
      if (streq(ql0->name, qualName)) {
        userInferenceError(obj->loc, "%s data qualified as %s",
                           context, qualName);
      }
    }
  }
  return true;
}

// find and exclude qualifiers in certain contexts
class ExcludeQual_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  ASTList<char> &exclude_global;
  ASTList<char> &exclude_cast;

  public:
  ExcludeQual_ASTVisitor(ASTList<char> &exclude_global0,
                         ASTList<char> &exclude_cast0)
    : loweredVisitor(this)
    , exclude_global(exclude_global0)
    , exclude_cast(exclude_cast0)
  {}

  virtual bool visitDeclarator(Declarator *obj);
  virtual bool visitExpression(Expression *obj);
};

bool ExcludeQual_ASTVisitor::visitDeclarator(Declarator *obj) {
  Variable *var = obj->var;
  xassert(var->getReal());
  // exclude qualifiers on globals
  if (obj->var->isSemanticallyGlobal()) {
    ExcludeQualData_ValueVisitor vis(exclude_global, "on global");
    asVariable_O(obj->var)->abstrValue()->traverse(vis);
  }
  return true;
}

bool ExcludeQual_ASTVisitor::visitExpression(Expression *obj) {
  if (obj->isE_cast() || obj->isE_keywordCast()) {
    // exclude qualifiers on casts
    ExcludeQualData_ValueVisitor vis(exclude_cast, "on cast expression");
    obj->abstrValue->traverse(vis);
  }
  return true;
}

class StackAllocVarLifecycle {
  public:
  Variable_O *var;
  bool matchesAsArray;
  int declPoint;
  int registerPoint;
  int unregisterPoint;

  public:
  StackAllocVarLifecycle(Variable_O *var0, bool matchesAsArray,
                         int declPoint0);

  bool isArray() { return var->abstrValue()->isArrayValue(); }
};

StackAllocVarLifecycle::StackAllocVarLifecycle
  (Variable_O *var0, bool matchesAsArray0, int declPoint0)
  : var(var0)
  , matchesAsArray(matchesAsArray0)
  , declPoint(declPoint0)
  , registerPoint(-1)
  , unregisterPoint(-1)
{
  xassert(var);
  xassert(declPoint >= 0);
}

class VariableUsedInRange_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  Statement *s;
  PtrMap<Variable_O, StackAllocVarLifecycle> &var2lifecycle;
  int iterPoint;

  public:
  VariableUsedInRange_ASTVisitor
    (Statement *s0, PtrMap<Variable_O, StackAllocVarLifecycle> &var2lifecycle0, int iterPoint0);
  virtual ~VariableUsedInRange_ASTVisitor() {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual bool visitExpression(Expression *obj);
};

VariableUsedInRange_ASTVisitor::VariableUsedInRange_ASTVisitor
  (Statement *s0, PtrMap<Variable_O, StackAllocVarLifecycle> &var2lifecycle0, int iterPoint0)
    : loweredVisitor(this)
    , s(s0)
    , var2lifecycle(var2lifecycle0)
    , iterPoint(iterPoint0)
{
  xassert(s);
  xassert(iterPoint > -1);
}

bool VariableUsedInRange_ASTVisitor::visitExpression(Expression *obj) {
  if (obj->isE_variable()) {
    // the specifics of the checking are inlined for speed
    Variable_O *var = asVariable_O(obj->asE_variable()->var);
    StackAllocVarLifecycle *lifecycle = var2lifecycle.get(var);
    if (lifecycle) {
      if (! (lifecycle->registerPoint <= iterPoint)) {
        userFatalError(getLoc(), "variable %s used before registeration",
                       var->name);
      }
      if (! (iterPoint <= lifecycle->unregisterPoint)) {
        userFatalError(getLoc(), "variable %s used after un-registeration",
                       var->name);
      }
    }
  }
  return true;
}

// Ensure that a stack-allocated QSPEC v follows the
// register-use-unregister discipline.
class RegStack_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  QSpec * const qspec; // qspec for stack variables which must be registered
  char * const regFunName;      // name of the registration function
  char * const unregFunName;    // name of the un-registration function
  char * const regArrayFunName; // name of the array registration function
  char * const unregArrayFunName; // name of the array un-registration function

  // map each variable that matches the qspec to a description of its
  // lifecycle
  PtrMap<Variable_O, StackAllocVarLifecycle> var2lifecycle;

  public:
  RegStack_ASTVisitor(QSpec * const qspec0
                      , char * const regFunName0
                      , char * const unregFunName0
                      , char * const regArrayFunName0
                      , char * const unregArrayFunName0)
    : loweredVisitor(this)
    , qspec(qspec0)
    , regFunName(regFunName0)
    , unregFunName(unregFunName0)
    , regArrayFunName(regArrayFunName0)
    , unregArrayFunName(unregArrayFunName0)
  {}

  public:
  // predicate tests that a statement registers a variable
  Variable_O *isRegStmt(Statement *s, bool &asArray, bool &start);

  virtual bool visitStatement(Statement *obj);
  virtual bool subVisitS_compound(S_compound *scpd);
};

// Does the statement 's' look like '(un)register(&var)' ?  If so,
// return the var; start is set to true if 'register' and false if
// 'unregister'; asArray flag indicates if the array registraton name
// matched instead of the normal registration.
Variable_O *RegStack_ASTVisitor::isRegStmt
  (Statement *s, bool &asArray, bool &start)
{
  // statement expression?
  if (!s->isS_expr()) return NULL;
  S_expr *sexpr = s->asS_expr();
  // direct function call?
  if (!sexpr->expr->expr->isE_funCall()) return NULL;
  E_funCall *efun = sexpr->expr->expr->asE_funCall();
  if (!efun->func->isE_variable()) return NULL;
  E_variable *efunVar = efun->func->asE_variable();
  if (!efunVar->name->isPQ_name()) return NULL;

  // of register, unregister, or neither; also, whether point or array
  // type?
  if (0) {                      // orthogonality
  } else if (streq(efunVar->name->asPQ_name()->name, regFunName)) {
    start = true;               // register
    asArray = false;
  } else if (streq(efunVar->name->asPQ_name()->name, regArrayFunName)) {
    start = true;               // register
    asArray = true;
  } else if (streq(efunVar->name->asPQ_name()->name, unregFunName)) {
    start = false;              // unregister
    asArray = false;
  } else if (streq(efunVar->name->asPQ_name()->name, unregArrayFunName)) {
    start = false;              // unregister
    asArray = true;
  } else {
    return NULL;
  }

  // if there is an argument; FIX: it is a bit odd, but only in the
  // case of a resistration function begin called with no arguments to
  // we rely on only the typesystem to find the error; we have no
  // variable to check, so this is ok and prevents me from having to
  // split the two cases of 1) finding a call to a registration
  // function and 2) extracting the variable that is the argument
  if (!efun->args->count()) return NULL;
  // get the first argument
  Expression *arg = efun->args->first()->expr;
  // if there is an addr-of expression then remove that layer; if this
  // is incompatible with asArray this will be found by the
  // typechecking or the client code when it matches the type of the
  // declaration with the purpose of the registration function name
  if (arg->isE_addrOf()) arg = arg->asE_addrOf()->expr;
  // of a variable expression?
  if (!arg->isE_variable()) return NULL;
  E_variable *evar = arg->asE_variable();
  // return the variable
  return asVariable_O(evar->var);
}

bool RegStack_ASTVisitor::visitStatement(Statement *obj) {
  if (obj->isS_compound()) {
    return subVisitS_compound(obj->asS_compound());
  }
  return true;
}

bool RegStack_ASTVisitor::subVisitS_compound(S_compound *scpd) {
  // * Pass 1, shallow - We want to recognize a stack allocated $js*
  // or $js*[].  We record the variable name and whether or not it is
  // an array and the iteration count where we found it.
  int iterPoint = 0;            // count the statements
  FOREACH_ASTLIST_NC(Statement, scpd->stmts, iter) {
    Statement *s = iter.data();
    if (s->isS_decl()) {
      Declaration *decl = s->asS_decl()->decl;
      FAKELIST_FOREACH_NC(Declarator, decl->decllist, iter) {
        Declarator *decl = iter;
        Variable_O *var = asVariable_O(decl->var);
        Value *v = var->abstrValue();
        bool matchesAsPoint = valueMatchesQSpec(v, qspec);
        bool matchesAsArray = v->isArrayValue() &&
          valueMatchesQSpec(v->getAtValue(), qspec);
        if (matchesAsPoint || matchesAsArray) {
          if (var2lifecycle.get(var)) {
            // this can't happen in code that typechecks
            userFatalError(s->loc, "Two declarations for variable %s",
                           var->name);
          }
          StackAllocVarLifecycle *lifecycle =
            new StackAllocVarLifecycle(var, matchesAsArray, iterPoint);
          var2lifecycle.add(var, lifecycle);
        }
      }
    }
    ++iterPoint;
  }

  // * Pass 2, shallow - Record all of the locations where any of the
  // variables of interest in the first pass have register(v) and
  // unregister(&v) called on them.  Report errors if these three
  // points are not in monotonic increasing order.
  iterPoint = 0;                // count the statements
  FOREACH_ASTLIST_NC(Statement, scpd->stmts, iter) {
    Statement *s = iter.data();
    // handle '(un)register(&v)' statements
    bool matchesAsArray; // true means the array registration was used
    bool start;     // true means 'register', false means 'unregister'
    Variable_O *var = isRegStmt(s, matchesAsArray, start);
    if (var) {
      StackAllocVarLifecycle *lifecycle = var2lifecycle.get(var);
      if (!lifecycle) {
        userFatalError(s->loc, "%s of non-qspec variable %s",
                       (start ? "Registration" : "Un-registration"),
                       var->name);
      }
      // check array-ness matches between registration and declaration
      if (matchesAsArray && !lifecycle->matchesAsArray) {
        userFatalError
          (s->loc,
           "Array %s used when variable %s matches qspec as non-array",
           (start ? "registration" : "un-registration"),
           var->name);
      }
      if (!matchesAsArray && lifecycle->matchesAsArray) {
        userFatalError
          (s->loc,
           "Non-array %s used when variable %s matches qspec as array",
           (start ? "registration" : "un-registration"),
           var->name);
      }
      // check startness and stopness
      if (start) {
        if (lifecycle->registerPoint != -1) {
          userFatalError(s->loc, "Double registration of qspec variable %s",
                         var->name);
        }
        lifecycle->registerPoint = iterPoint;
        xassert(lifecycle->declPoint > -1); // checked in StackAllocVarLifecycle ctor
        // this can't happen in code that typechecks
        if (! (lifecycle->registerPoint > lifecycle->declPoint)) {
          userFatalError(s->loc, "Somehow registration point not after "
                         "declaration point %s",
                         lifecycle->var->name);
        }
      } else {
        if (lifecycle->unregisterPoint != -1) {
          userFatalError(s->loc, "Double un-registration of qspec variable %s",
                         var->name);
        }
        lifecycle->unregisterPoint = iterPoint;
        if (! (lifecycle->registerPoint > -1)) {
          userFatalError(s->loc, "Un-registration point before "
                         "registration point %s",
                         lifecycle->var->name);
        }
        // this can't happen in code that typechecks
        if (! (lifecycle->unregisterPoint > lifecycle->registerPoint)) {
          userFatalError(s->loc, "Un-registration point not after "
                         "registration point %s",
                         lifecycle->var->name);
        }
      }
    }
    ++iterPoint;
  }

  // * Pass 3, variable - Check that each variable is both registered
  // and unregistered; that they are done in the right order is
  // checked above; this is only really necessary if the variable is
  // never used as otherwise it could be checked at the usage point
  for(PtrMap<Variable_O, StackAllocVarLifecycle>::Iter iter(var2lifecycle);
      !iter.isDone(); iter.adv()) {
    Variable_O *var = iter.key();
    StackAllocVarLifecycle *lifecycle = iter.value();
    xassert(var == lifecycle->var);
    if (! (lifecycle->registerPoint > -1)) {
      userFatalError(var->loc, "Qspec variable never registered %s",
                     var->name);
    }
    if (! (lifecycle->unregisterPoint > -1)) {
      userFatalError(var->loc, "Qspec variable never un-registered %s",
                     var->name);
    }
  }

  // * Pass 4, deep - Check that all useages of the variable are
  // between the register and unregister point (between means
  // inclusive-inclusive).
  iterPoint = 0;                // count the statements
  FOREACH_ASTLIST_NC(Statement, scpd->stmts, iter) {
    Statement *s = iter.data();
    // fundamental tradeoff between accumulation and later checking
    // and complexity: 1) slow to build a set, but no redundancy in
    // later checking; 2) fast to build a list, but redundancy in
    // later checking; 3) push the checking down into the iteration,
    // merging the two aspects and resulting in furthe compelxity; 4)
    // implement co-routines; I choose number 3.
    VariableUsedInRange_ASTVisitor varUsedVis(s, var2lifecycle, iterPoint);
    s->traverse(varUsedVis.loweredVisitor);
    ++iterPoint;
  }

  return true;
}

// find expressions in a given set of contexts and make a set of them
class FindExprInContext_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  PtrSet<Expression> *found;    // output: the found Expressions

  // find those expressions in:
  bool inFuncArgs;              // function arguments
  bool inDeref;                 // an E_deref
  bool inAddrOf;                // an E_addrOf
  bool inNonConstContext;       // a context where written-to
  bool inInitializer;           // an IN_expr

  public:
  FindExprInContext_ASTVisitor(PtrSet<Expression> *found0)
    : loweredVisitor(this)
    , found(found0)
    , inFuncArgs        (false)
    , inDeref           (false)
    , inAddrOf          (false)
    , inNonConstContext (false)
    , inInitializer     (false)
  {}

  // add an expression to the set of found expressions
  virtual void addExprToFound(Expression *expr);

  virtual bool visitInitializer(Initializer *obj);
  virtual bool visitExpression(Expression *obj);
};

void FindExprInContext_ASTVisitor::addExprToFound(Expression *expr) {
  if (found) found->add(expr);
}

bool FindExprInContext_ASTVisitor::visitInitializer(Initializer *obj) {
  if (inInitializer && obj->isIN_expr()) {
    addExprToFound(obj->asIN_expr()->e);
  }
  return true;
}

bool FindExprInContext_ASTVisitor::visitExpression(Expression *obj) {
  if (inFuncArgs && obj->isE_funCall()) {
    FAKELIST_FOREACH_NC(ArgExpression, obj->asE_funCall()->args, iter) {
      addExprToFound(iter->expr);
    }
  } else if (inDeref && obj->isE_deref()) {
    addExprToFound(obj->asE_deref()->ptr);
  } else if (inAddrOf && obj->isE_addrOf()) {
    addExprToFound(obj->asE_addrOf()->expr);
  } else if (inNonConstContext && obj->isE_assign()) {
    addExprToFound(obj->asE_assign()->target);
  } else if (inNonConstContext && obj->isE_effect()) {
    addExprToFound(obj->asE_effect()->expr);
  }

  return true;
}

// collect variables instead of expressions
class FindVarInContext_ASTVisitor : public FindExprInContext_ASTVisitor {
  public:
  PtrSet<Variable> *foundVar;   // output: the found variables
  FindVarInContext_ASTVisitor(PtrSet<Variable> *foundVar0)
    : FindExprInContext_ASTVisitor(NULL)
    , foundVar(foundVar0)
  {}
  virtual void addExprToFound(Expression *expr);
};

void FindVarInContext_ASTVisitor::addExprToFound(Expression *expr) {
  if (expr->isE_variable()) {
    if (foundVar) foundVar->add(expr->asE_variable()->var);
  }
}

class ExlPermAlias_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  PtrSet<Expression> &context;  // allowed context for the expression
  char const * const contextName; // the name of the context set
  QSpec * const qspec;      // exprs matching qspec must be in context

  public:
  ExlPermAlias_ASTVisitor
    (PtrSet<Expression> &context0, char const * const contextName0,
     QSpec * const qspec0)
      : loweredVisitor(this)
      , context(context0)
      , contextName(contextName0)
      , qspec(qspec0)
  {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual bool visitExpression(Expression *obj);
};

bool ExlPermAlias_ASTVisitor::visitExpression(Expression *obj) {
  // the asRval() is important
  if (valueMatchesQSpec(obj->abstrValue->asRval(), qspec)) {
    if (!context.contains(obj)) {
      userFatalError(getLoc(),
                     "expression of type %s not found in context of %s",
                     qspec->toString(), contextName);
    }
  }
  return true;
}

class ExclValueRefCall_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  QSpec * const qspec;     // check is relevant to vars matching qspec

  public:
  ExclValueRefCall_ASTVisitor(QSpec * const qspec0)
    : loweredVisitor(this)
    , qspec(qspec0)
  {}
  virtual ~ExclValueRefCall_ASTVisitor() {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual bool visitFullExpression(FullExpression *fexp);
};

bool ExclValueRefCall_ASTVisitor::visitFullExpression(FullExpression *fexp) {
  // visit the whole full expression and find all of the E_variable
  // expressions that are function arguments
  PtrSet<Variable> argVarsSet; // this would be better done as a list
  {
    FindVarInContext_ASTVisitor findArgVar(&argVarsSet);
    findArgVar.inFuncArgs = true;
    fexp->traverse(findArgVar.loweredVisitor);
  }

  // visit the whole full expression and find all of the E_variable
  // expressions that are inside an E_addrOf expression
  PtrSet<Variable> addrOfVarsSet;
  {
    FindVarInContext_ASTVisitor findAddrOfVar(&addrOfVarsSet);
    findAddrOfVar.inAddrOf = true;
    findAddrOfVar.inNonConstContext = true;
    fexp->traverse(findAddrOfVar.loweredVisitor);
  }

  // for all of those on both lists, make sure they do not match qspec
  for(PtrMap<Variable, Variable>::Iter iter(argVarsSet);
      !iter.isDone(); iter.adv()) {
    Variable *var = iter.key();
    if (addrOfVarsSet.contains(var)) {
      // in both lists
      if (valueMatchesQSpec(asVariable_O(var)->abstrValue(), qspec)) {
        userFatalError
          (getLoc(), "variable %s is 1) an R-value argument and also"
           " 2) in a reference context or has its address taken.\n",
           var->name);
      }
    }
  }

  // I suppose some FullExpressions that are nested should be checked
  return true;
}

// check that function arguments that match qspec are stack or
// parameter allocated variables
class ArgLocalVar_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  QSpec * const qspec;     // check is relevant to vars matching qspec

  public:
  ArgLocalVar_ASTVisitor(QSpec * const qspec0)
    : loweredVisitor(this)
    , qspec(qspec0)
  {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}
  // test the function argument meets the criterion
  virtual void testFuncArg(Expression *obj);
  virtual bool visitExpression(Expression *obj);
};

void ArgLocalVar_ASTVisitor::testFuncArg(Expression *obj) {
  // we are only interested in arguments that match qspec
  if (!valueMatchesQSpec(obj->abstrValue->asRval(), qspec)) return;
  // if so, then must be a stack- or parameter-allocated variable
  if (!obj->isE_variable()) {
    userFatalError(getLoc(),
                   "Function argument of type %s is not a variable\n",
                   qspec->toString());
  }
  Variable *var = obj->asE_variable()->var;
  ScopeKind varSK = var->getScopeKind();
  if (varSK == SK_PARAMETER || varSK == SK_FUNCTION) return;
  userFatalError(getLoc(),
                 "Function argument variable %s of type %s "
                 "is not stack- or parameter-allocated\n",
                 var->name, qspec->toString());
}

bool ArgLocalVar_ASTVisitor::visitExpression(Expression *obj) {
  if (obj->isE_funCall()) {
    FAKELIST_FOREACH_NC(ArgExpression, obj->asE_funCall()->args, iter) {
      testFuncArg(iter->expr);
    }
  }
  return true;
}

class TempImmediatelyUsed_ASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  PtrSet<Expression> &context;  // allowed context for the expression
  char const * const contextName; // the name of the context set
  QSpec * const qspec;      // exprs matching qspec must be in context

  public:
  TempImmediatelyUsed_ASTVisitor
    (PtrSet<Expression> &context0, char const * const contextName0,
     QSpec * const qspec0)
      : loweredVisitor(this)
      , context(context0)
      , contextName(contextName0)
      , qspec(qspec0)
  {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual bool visitExpression(Expression *obj);
};

bool TempImmediatelyUsed_ASTVisitor::visitExpression(Expression *obj) {
  if (valueMatchesQSpec(obj->abstrValue->asRval(), qspec)) {
    // obj is an expression matching qspec, so it had better be a
    // stack- or parameter-allocated variable or in the context set
    bool stackOrParamAllocVar = false;
    if (obj->isE_variable()) {
      Variable *var = obj->asE_variable()->var;
      ScopeKind varSK = var->getScopeKind();
      if (varSK == SK_PARAMETER || varSK == SK_FUNCTION) {
        stackOrParamAllocVar = true;
      }
    }
    if (! (context.contains(obj) || stackOrParamAllocVar) ) {
      userFatalError
        (getLoc(),
         "expression of type %s not a stack- or parameter-allocated variable "
         "and also not found in context of %s",
         qspec->toString(), contextName);
    }
  }
  return true;
}

// find and exclude certain combinations of DeclFlags and qualifiers
void Qual::exclude_qual_stage() {
  printStage("exclude");
  Restorer<bool> restorer(value2typeIsOn, true);

  // exclude qualifiers in particular contexts
  if (qualCmd->exclude_global.count() > 0 ||
      qualCmd->exclude_cast.count() > 0)
    {
    // implement exclude-global: exclude global declarators containing
    // the given qualifiers
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      ExcludeQual_ASTVisitor vis(qualCmd->exclude_global,
                                 qualCmd->exclude_cast);
      unit->traverse(vis.loweredVisitor);
    }
  }

  // MOZILLA: this is a mozilla-specific analysis; we ensure that a
  // stack-allocated QSPEC v follows the register-use-unregister
  // discipline
  if (qualCmd->reg_stack.count() > 0) {
    xassert(qualCmd->reg_stack_regfunc);
    xassert(qualCmd->reg_stack_un_regfunc);
    xassert(qualCmd->reg_stack_array_regfunc);
    xassert(qualCmd->reg_stack_array_un_regfunc);
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      FOREACH_ASTLIST_NC(QSpec, qualCmd->reg_stack, iter) {
        QSpec *qspec = iter.data();
        RegStack_ASTVisitor vis(qspec,
                                qualCmd->reg_stack_regfunc,
                                qualCmd->reg_stack_un_regfunc,
                                qualCmd->reg_stack_array_regfunc,
                                qualCmd->reg_stack_array_un_regfunc);
        unit->traverse(vis.loweredVisitor);
      }
    }
  }

  // MOZILLA: this is a mozilla-specific analysis; we ensure that all
  // expressions matching QSPEC are in the context of function
  // argument or a dereference
  if (qualCmd->exl_perm_alias.count() > 0) {
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      // collect the expressions that are in a function argument or
      // dereference context
      PtrSet<Expression> funcArgOrDerefExprSet;
      FindExprInContext_ASTVisitor findArgOrDerefVis(&funcArgOrDerefExprSet);
      findArgOrDerefVis.inFuncArgs = true;
      findArgOrDerefVis.inDeref = true;
      unit->traverse(findArgOrDerefVis.loweredVisitor);
      // for each QLIT, if an expression of type QLIT ** occurs, make
      // sure it is in the funcArgOrDerefExprSet
      FOREACH_ASTLIST_NC(QSpec, qualCmd->exl_perm_alias, iter) {
        QSpec *qspec = iter.data();
        ExlPermAlias_ASTVisitor vis
          (funcArgOrDerefExprSet, "function argument or pointer dereference",
           qspec);
        unit->traverse(vis.loweredVisitor);
      }
    }
  }

  // MOZILLA: this is a mozilla-specific analysis; we ensure that a
  // variable matching QSPEC cannot both occur as 1) an rvalue and 2)
  // an lvalue or pointer to it in the same FullExpression.
  if (qualCmd->exl_value_ref_call.count() > 0) {
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      FOREACH_ASTLIST_NC(QSpec, qualCmd->exl_value_ref_call, iter) {
        QSpec *qspec = iter.data();
        ExclValueRefCall_ASTVisitor vis(qspec);
        unit->traverse(vis.loweredVisitor);
      }
    }
  }

  // MOZILLA: this is a mozilla-specific analysis; check that function
  // arguments matching QSPEC are stack or parameter allocated
  // variables
  if (qualCmd->arg_local_var.count() > 0) {
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      FOREACH_ASTLIST_NC(QSpec, qualCmd->arg_local_var, iter) {
        QSpec *qspec = iter.data();
        ArgLocalVar_ASTVisitor vis(qspec);
        unit->traverse(vis.loweredVisitor);
      }
    }
  }

  // MOZILLA: this is a mozilla-specific analysis; check that an
  // expression matching QSPEC is a stack- or parameter-allocated
  // variable, or in the context of a dereference, or in the context
  // of an initializer
  if (qualCmd->temp_immediately_used.count() > 0) {
    foreachSourceFile {
      File *file = files.data();
      maybeSetInputLangFromSuffix(file);
      TranslationUnit *unit = file2unit.get(file);
      PtrSet<Expression> derefOrInitSet;
      FindExprInContext_ASTVisitor findDerefOrInit(&derefOrInitSet);
      findDerefOrInit.inDeref = true;
      findDerefOrInit.inInitializer = true;
      unit->traverse(findDerefOrInit.loweredVisitor);
      FOREACH_ASTLIST_NC(QSpec, qualCmd->temp_immediately_used, iter) {
        QSpec *qspec = iter.data();
        TempImmediatelyUsed_ASTVisitor vis
          (derefOrInitSet, "pointer dereference or initializer", qspec);
        unit->traverse(vis.loweredVisitor);
      }
    }
  }
}


// ****************

void Qual::qualCompile_stage() {
  printStage("qualCompile");
  Restorer<bool> restorer(value2typeIsOn, true);

  qualCompile_setGlobalness();
  qualCompile_nameExpressions();
  qualCompile_nameVars();
  qualCompile_qualVisitation();
  // optional backend integrity check
  // LibQual::check_all_edge_sets();
}

bool Qual::finishQuals_stage() {
  // NOTE: print quals graph gets done here by the backend if the flag
  // to do it was set during command line parsing
  printStage("finish quals");

#if DELAY_SET_NAMES
  // quarl 2006-06-26
  //    For DELAY_SET_NAMES, we need to run before finish_quals because it's
  //    going to request names.
  updateEcrAll();
#else
#endif
  bool qualifierInconsistency = LibQual::finish_quals();
  // updateEcrAll(); // see below markExtern_stage() why this is
  // commented for now
  haveRunFinishQuals = true;
  return qualifierInconsistency;
}

void Qual::markExtern_stage() {
  Restorer<bool> restorer(value2typeIsOn, true);
  markExternVars();
  // TODO: apparently this has to run after markExternVars, perhaps
  // because it is creating new variables?!
  updateEcrAll();
}

void Qual::compactify_stage() {
  if (qualCmd->doCompactifyGraph()) {
    printStage("compactify stage");
    compactifyGraph();
  }
}
