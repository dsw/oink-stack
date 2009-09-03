// see License.txt for copyright and terms of use

#include "qual_annot.h"         // this module
#include "cc_type.h"
#include "qual_value_children.h"
#include "oink_var.h"
#include "dataflow_ex.h"        // DataFlowEx
#include "qual_dataflow_ty.h"   // eDataFlow_normal
#include "oink_global.h"        // typecheckingDone
#include "qual_global.h"        // haveRunFinishQuals
#include "qual_cmd.h"           // QualCmd
#include "oink_util.h"
#include "qual_var.h"

string globalnessToString(Globalness globalness0) {
  switch(globalness0) {
  default: xfailure("illegal globalness"); break;
  case UNDEF_Globalness: return "UNDEF_Globalness";
  case GLOBAL_Globalness:  return "GLOBAL_Globalness:";
  case LOCAL_Globalness:  return "LOCAL_Globalness:";
  }
}

static int globalnessToInt(Globalness globalness0) {
  int globalness_int;
  switch(globalness0) {
  default: xfailure("illegal globalness"); break;
  case UNDEF_Globalness: globalness_int = 0; break; // non-var values for exprs are local
  case GLOBAL_Globalness: globalness_int = 1; break;
  case LOCAL_Globalness: globalness_int = 0; break;
  }
  return globalness_int;
}

static bool isPreferredValue(Value *t) {
  return t->isReferenceValue() ? 1 : 0;
}

// return true iff starts with '$!'
bool isSpecialQualifier(QLiterals *ql0) {
  char const *name = ql0->name;
  return name != NULL && name[0] == '$' && name[1] == '!';
}

QualAnnot::QualAnnot()
  :
#if !DELAY_SET_NAMES
  vname(NULL),
#endif
  qv0(NULL)
    // note: funk, edgesAdded, globalness all initialized to 0 via PackedWord
{}

LibQual::Type_qualifier *QualAnnot::qv() {
  if (qv0) return qv0;

  Value *value = v0();

  if (qualCmd->merge_ref_ptr_qvars) {
    Value *otherVal = NULL;
    if (value->isReferenceValue()) {
      otherVal = value->asReferenceValue()->atValue->ptrToMe;
    } else if (value->isPointerValue()) {
      otherVal = value->asPointerValue()->atValue->refToMe;
    }
    if (otherVal) {
      QualAnnot *otherAnnot = qa(otherVal);
      if (otherAnnot->qv0) {
        qv0 = otherAnnot->qv0;
        return qv0;
      }
    }
  }

  qv0 = make_qvar_CQUAL
    (
#if !DELAY_SET_NAMES
      vname,
#endif
      value->loc,
      isPreferredValue(value), // preferred
      globalnessToInt(this->getGlobalness()));
  return qv0;
}

bool QualAnnot::hasQv() {
  return qv0;
}

// quarl 2006-05-14
//    Update to point to ecr qvar.  This is necessary before calling
//    eliminate_quals_links().
void QualAnnot::ecrUpdate() {
  if (qv0) {
    // static int i = 0;
    // i++;
    // printf("## %d. ecrUpdate: value=%p, qa=%p, qv==%p (%p) %s\n",
    //        i,
    //        this->value, this, qv0, LibQual::ecr_qual_noninline(qv0),
    //        (qv0 != LibQual::ecr_qual_noninline(qv0)) ? " ***" : "");
    qv0 = LibQual::ecr_qual_noninline(qv0);
  }
}

// quarl 2006-05-15
//   If the qvar was killed in compaction, remove our pointer to it.
void QualAnnot::removeQvarIfDead() {
  // printf("## removeQvarIfDead: considering qvar %p for annot %p for value %p\n",
  //        qv0, this, this->value);
  if (qv0 != NULL) {
    if (LibQual::is_dead_qual(qv0)) {
      // TODO: make sure it was a good idea to remove it, e.g. not extern or
      // global
      // printf("## removeQvarIfDead: removing qvar %p for annot %p for value %p\n",
      //        qv0, this, this->value);

      qv0 = NULL;
    } else {
      xassert ( qv0 == LibQual::ecr_qual_noninline(qv0) );
    }
  }
}

void QualAnnot::setQv_deserialize(LibQual::Type_qualifier *qv1) {
  qv0 = qv1;
}

bool QualAnnot::qvarIsExtern() {
  return qv0 && LibQual::extern_qual(qv0);
}

Funky QualAnnot::getFunk() {
  // FIX: check that we are in a ctor state where the funk has
  // already been set.

  // TODO: memoize
  return attachFunky(v0()->loc);
}

Funky QualAnnot::attachFunky(SourceLoc loc) {
  Funky funk;
  for(QLiterals *ql0=getQl(); ql0; ql0=ql0->next) {
    Funky f0 = parseAsFunky(ql0->name, loc);
    // Is this a funky set qualifier?
    if (!f0.empty()) {
      if (funk.empty()) {
        funk = f0;
      } else {
        if (funk != f0) {
          // FIX: should this call report_qerror?
          userFatalError(loc, "More than one funky qualifier on a type");
        }
      }
    }
    // otherwise, it was a normal qualifier
  }
  setFunk0(funk.getRepr());
  return funk;
}

// attach our literals to another (possibly same) object
void QualAnnot::attachLiteralsTo(QualAnnot *target) {
  xassert(target);

  Value *value = v0();

  for(QLiterals *ql0=getQl(); ql0; ql0=ql0->next) {
    if (isFunkyQualifier(ql0, value->loc)) continue; // skip funky qualifiers
    if (isSpecialQualifier(ql0)) continue;
    LibQual::Type_qualifier *qconst = findQualAndCheck(ql0->name);
    target->attachOneLiteral(value->loc, qconst);
  }

  // special case qualifiers that are built into the language:
  if (isConstForInferencePurposes(value)) {
    target->attachOneLiteralIfInLattice(value->loc, "const");
  } else if (qualCmd->strict_const &&
             !value->isReferenceValue() // can't attach a level=ref qualifer to reference
             ) {
    target->attachOneLiteralIfInLattice(value->loc, "$nonconst");
  }
  if (isVolatileForInferencePurposes(value)) {
    target->attachOneLiteralIfInLattice(value->loc, "volatile");
    // NOTE 5 april 2003: I think this is all handled; volatile
    // inference was failing for a different reason.

    // FIX: Why isn't this working (here and above)?
    // since using monmorphic online backend and since attaching a
    // qualifier constant, I can actually check this right now
//      xassert(qv);
//      LibQual::Type_qualifier *volatile_const = findQualAndCheck("volatile");
//      xassert(LibQual::leq_qual(qv, volatile_const)); // both directions
//      xassert(LibQual::leq_qual(volatile_const, qv));
  }
}

void QualAnnot::attachLiterals() {
  attachLiteralsTo(this);
}

void QualAnnot::attachOneLiteralIfInLattice(SourceLoc locOfAttach,
                                            char const *qconst_name) {
  LibQual::Type_qualifier *qconst = LibQual::find_qual(qconst_name);
  if (qconst) attachOneLiteral(locOfAttach, qconst);
}

LibQual::Type_qualifier *QualAnnot::getQvarOfInterest(LibQual::Type_qualifier *qconst) {
  LibQual::Type_qualifier *qvarOfInterest = NULL;
  LibQual::level_qual_t qconst_level = LibQual::level_qual(qconst);
  // dsw: this actually works, but it freaks me out that it does; I am
  // thinking that some esoteric C++ name lookup rule is telling the
  // compiler to look into the LibQual:: namespace since the arugment
  // is from that namespace or something.
//      level_qual(qconst);

  Value *value = v0();

  if (qconst_level==LibQual::level_ref) {
    // get the qv of our refToMe
    if (value->isReferenceValue()) {
      // can't get a ref value of a ref value
      qvarOfInterest = NULL;
    } else {
      ReferenceValue *r = value->asLval();
      xassert(static_cast<Value*>(r) != value);
      qvarOfInterest = qa(r)->getQVar();
    }
  } else {
    xassert(qconst_level==LibQual::level_value);
    qvarOfInterest = getQVar();
  }
  return qvarOfInterest;
}

void QualAnnot::attachOneLiteral(SourceLoc locOfAttach, LibQual::Type_qualifier *qconst) {
  xassert(!v0()->isReferenceValue()); // shouldn't attach any qualifier literals to a reference
  LibQual::Type_qualifier *qvarOfInterest = getQvarOfInterest(qconst);
  xassert(qvarOfInterest);
  attachOneLiteralToQvar(locOfAttach, qconst, qvarOfInterest);
}

static void unify_ptrToMe_and_refToMe(LibQual::Type_qualifier *ref_qv,
                                      LibQual::Type_qualifier *ptrToMe_qv,
                                      SourceLoc loc) {
  int edgeNumber = incEdgeNumber(ref_qv, ptrToMe_qv, loc);
  unify_qual_CQUAL(loc,
                   ref_qv, ptrToMe_qv,
                   edgeNumber
                   );
}

LibQual::Type_qualifier *QualAnnot::getQVar() {
  // don't do any analysis until typechecking is done
  xassertdb(typecheckingDone && elaboratingDone);

#if !DELAY_SET_NAMES
  // if we still don't have a name fix that
  if (!haveQaName()) {
    if (qualCmd->name_if_missing) {
      setNameMissing1();
    }
  }
#endif

  Value *value = v0();

  if (qualCmd->inference && !getEdgesAdded()) {
    xassert (!LibQual::is_dead_qual(qv()));
    setEdgesAdded(true);

    if (value->isReferenceValue()) {
      ReferenceValue *r = value->asReferenceValue();
      Value *ptq = r->getAtValue()->ptrToMe;
      if (ptq) {
        if (value != ptq) {
          LibQual::Type_qualifier *oqv = qa(ptq)->getQVar();
          xassert (!LibQual::is_dead_qual(oqv));
          if (qualCmd->merge_ref_ptr_qvars && !qv0) {
            qv0 = oqv;
          } else {
            unify_ptrToMe_and_refToMe(qv(), oqv, value->loc);
          }
        }
      }
    } else if (value->isPointerValue()) {
      PointerValue *r = value->asPointerValue();
      Value *rtq = r->getAtValue()->refToMe; // NOTE: NULL is an OK response here
      if (rtq) {
        if (value != rtq) {
          LibQual::Type_qualifier *oqv = qa(rtq)->getQVar();
          xassert (!LibQual::is_dead_qual(oqv));
          if (qualCmd->merge_ref_ptr_qvars && !qv0) {
            qv0 = oqv;
          } else {
            // NOTE: REVERSED
            unify_ptrToMe_and_refToMe(oqv, qv(), value->loc);
          }
        }
      }
    }

    attachLiterals();
  }

  return qv();
}

static char const *chopFirstCharIfDollar(char const *s) {
  xassert(s);
  if (s[0] == '$') return s+1;
  return s;
}

string QualAnnot::inferredQualifiersToString() {
//    if (!qv) return string("");

  // FIX: put a check in here that this is only called after finish_quals().
  stringBuilder s;
  LibQual::Type_qualifier *qconst;
  LibQual::qual_set_ST_scanner *qs;
  // NOTE: 'scan_qual_set' is a macro defined in libqual;
  // unfortunately namespaces do not qualify macros
  scan_qual_set(qconst, qs, LibQual::get_constant_quals()) {
    LibQual::Type_qualifier *qvarOfInterest = getQvarOfInterest(qconst);
    if (!qvarOfInterest) {
      xassert(v0()->isReferenceValue());
      continue;
    }
    int qv_leq_qconst = LibQual::leq_qual(qvarOfInterest, qconst);
    int qconst_leq_qv = LibQual::leq_qual(qconst, qvarOfInterest);
    char const *const_name = LibQual::name_qual(qconst);
    const_name = chopFirstCharIfDollar(const_name);
    if (qv_leq_qconst) {
      if (qconst_leq_qv) s << " __attribute__((EQ_" << const_name << ")) ";
      else s << " __attribute__((LE_" << const_name << ")) ";
    } else {
      if (qconst_leq_qv) s << " __attribute__((GE_" << const_name << ")) ";
      // else they are unrelated
    }
  }
  return s;
}

// ****************

QLiterals const *FunctionValue_QualAnnot::getThisQLiterals() {
  ReferenceValue *rt = asVariable_O(v()->type->getReceiverC())->abstrValue()->asReferenceValue();
  CVAtomicValue *at = rt->atValue->asCVAtomicValue();
  return qa(at->asCVAtomicValue())->getQl();
}

AdjList *FunctionValue_QualAnnot::getFunkyAdjList() {
  if (!funkyAdjList) {
    funkyAdjList = new AdjList(v());
  }
  return funkyAdjList;
}

void FunctionValue_QualAnnot::ensureFunkyInternalEdges
  (SourceLoc loc, DataFlowEx &dfe)
{
  // this function is idempotent
  if (funky_applied) return;

  if (treeContainsFunkyQliterals(v())) {
    for(ObjListIter<DeepValueNamePair> AdjList_iter(getFunkyAdjList()->getPairList());
        !AdjList_iter.isDone();
        AdjList_iter.adv()) {
      // add edge for this pair
      DeepValueNamePair const *p = AdjList_iter.data();

      FunctionValue *t0 = dynamic_cast<FunctionValue*>(this->v());
      xassert(t0);
      Value *tFirst = p->x->index(t0);
      xassert(tFirst);            // should never fail since calling on our own func
      if (tFirst->t()->isEllipsis()) continue; // skip ellipsis

      Value *tSecond = p->y->index(t0);
      xassert(tSecond);           // should never fail since calling on our own func
      if (tSecond->t()->isEllipsis()) continue; // skip ellipsis

      dfe.eDataFlow_normal(tFirst, tSecond, loc);
    }
  }

  xassert(!funky_applied);
  funky_applied = true;
}

// for ensuring funky internal edges after exploding an ellipsis into
// the ellipsis parameters that mirror the arguments that go into the
// ellipsis
void FunctionValue_QualAnnot::ensureFunkyInternalEllipsisEdges
  (SourceLoc loc, Value *ellipsis_param_value, DataFlowEx &dfe)
{
  for(ObjListIter<DeepValueNamePair> AdjList_iter(getFunkyAdjList()->getPairList());
      !AdjList_iter.isDone();
      AdjList_iter.adv())
  {
    // add edge for this pair
    DeepValueNamePair const *p = AdjList_iter.data();

    FunctionValue *t0 = dynamic_cast<FunctionValue*>(this->v());
    xassert(t0);
    Value *tFirst = p->x->index(t0);
    xassert(tFirst);            // should never fail since calling on our own func
    if (tFirst->t()->isEllipsis()) tFirst = ellipsis_param_value;

    Value *tSecond = p->y->index(t0);
    xassert(tSecond);           // should never fail since calling on our own func
    if (tSecond->t()->isEllipsis()) {
      xassert(tFirst != ellipsis_param_value); // shouldn't happen both times
      tSecond = ellipsis_param_value;
    }

    dfe.eDataFlow_normal(tFirst, tSecond, loc);
  }
}

// ********

#define VALUE_V_DEFN(CLS)                                         \
  CLS::MyValue *CLS::v() {                                        \
    return &MyStruct::getStructFrom2(this)->t1;                   \
  }                                                               \
  Value *CLS::v0() { return v(); }

FOREACH_CLASS_QualAnnot(VALUE_V_DEFN)

// nameTree ****************

#if !DELAY_SET_NAMES
char const *QualAnnot::getName() const {
  if (!qv0) return vname;
  return LibQual::name_qual(qv0);
}
#endif

void QualAnnot::setName(char const * name0) {
#if !DELAY_SET_NAMES
  if (haveQaName()) return;
#endif

  if (qualCmd->name_with_loc
#if USE_SERIAL_NUMBERS
      || qualCmd->name_with_serialno
#endif
    ) {
    stringBuilder name1;

    Value *value = v0();

    // attach location
    if (qualCmd->name_with_loc) {
      name1 << ::toString(value->loc) << ' ';
    }

#if USE_SERIAL_NUMBERS
    // attach serialNumber
    if (qualCmd->name_with_serialno) {
      name1 << 'v' << value->serialNumber << '\\' << 'n';
    }
#endif

    name1 << name0;

    setName1(name1.c_str());
  } else {
    // optimization: don't need to create stringBuilder name1
    setName1(name0);
  }
}

#if DELAY_SET_NAMES
static const char *qvar_without_name = "???";
VoidPtrMap qvarNames;

extern void Qual_doAssignNames(); // XXX KLUDGE FIXME

size_t getNumQvarsWithNames()
{
  return qvarNames.getNumEntries();
}

namespace LibQual {
  void declare_qvar_need_name(qual qvar) {
    xassert(qvar == ecr_qual_noninline(qvar));
    if (qvarNames.get(qvar) == NULL) {
      qvarNames.add(qvar, (void*) qvar_without_name);
    }
  }


  void assign_qvar_names() {
    Qual_doAssignNames();

#if 0
    // check that we didn't miss any
    for (VoidPtrMap::Iter iter(qvarNames); !iter.isDone(); iter.adv()) {
      qual q = (qual) iter.key();
      xassert(q == ecr_qual_noninline(q));

      void *v = iter.value();
      xassert(v != NULL);
      xassert(v != qvar_without_name);
    }
#endif
  }

  char const *qvar_get_name(qual qvar) {
    char const *n = (char const*) qvarNames.get(qvar);

    // if it's NULL, that means declare_qvar_need_name wasn't called.
    xassert(n != NULL);

#if 1
    // quarl 2006-06-26
    //    If this translation unit was deserialized from an archive, then we
    //    won't necessarily have Values pointing to these qvars any more.
    if (n == qvar_without_name) {
      return "(non-extern)";
    }
#endif
    xassert(n != qvar_without_name);
    return n;
  }
}

void QualAnnot::maybeSetNameMissingIfMissing()
{
  if (qv0) {
    xassert(qv0 == LibQual::ecr_qual_noninline(qv0));
    if (qvarNames.get(qv0) == qvar_without_name) {
      setNameMissing1();
    }
  }
}
#else

void QualAnnot::maybeSetNameMissingIfMissing()
{
}

#endif

// quarl 2006-06-26
//    Actually set the name for a QualAnnot.  Input name0 is temporary so must
//    be copied if needed later.
void QualAnnot::setName1(char const *name0) {
#if DELAY_SET_NAMES
  if (qv0 && qvarNames.get(qv0) == qvar_without_name) {
    qvarNames.add(qv0, strdup(name0));
  }

#else
  // don't do this anymore
  //   LibQual::set_qual_name(qv, strdup(name1.c_str()));
  this->vname = strdup(name0);
#endif
}

void QualAnnot::setNameMissing1() {
  // FIX: This interacts badly with -fcc-print-trans-qual leading to
  // an infinite loop; UPDATE: not if you print the ML type; UPDATE:
  // with the new value printer hopefully that problem is gone
//      setName(strdup(value->toCString().c_str()));
  // these are more descriptive

  Value *value = v0();
  setName(value->t()->toMLString().c_str());
}

void CVAtomicValue_QualAnnot::nameTree(char const * name0) {
  setName(name0);
}

void PointerValue_QualAnnot::nameTree(char const * name0) {
  setName(name0);
  // this is a hack to prevent adding a "'" before the "_ret" from a
  // function
  Value *atValue = v()->atValue;
  if (atValue->isFunctionValue()) qa(atValue)->nameTree(name0);
  else qa(atValue)->nameTree(stringc << name0 << '\'');
}

void ReferenceValue_QualAnnot::nameTree(char const * name0) {
  setName(name0);
  // this is a hack to prevent adding a "'" before the "_ret" from a
  // function
  Value *atValue = v()->atValue;
  if (atValue->isFunctionValue()) qa(atValue)->nameTree(name0);
  else qa(atValue)->nameTree(stringc << name0 << '\'');
}

void FunctionValue_QualAnnot::nameTree(char const * name0) {
  setName(name0);
  qa(v()->retValue)->nameTree(stringc << name0 << "_ret");
}

void ArrayValue_QualAnnot::nameTree(char const * name0) {
  setName(name0);
  qa(v()->atValue)->nameTree(stringc << name0 << '\'');
}

// sm: copied from PointerValue_QualAnnot::nameTree
void PointerToMemberValue_QualAnnot::nameTree(char const * name0) {
  setName(name0);
  qa(v()->getAtValue())->nameTree(stringc << name0 << '\'');
}

// setGlobalTree ****************

void QualAnnot::setGlobal(Globalness globalness) {
  xassert(globalness != UNDEF_Globalness);
  xassert(!haveRunFinishQuals); // it is too late after finish_quals().
  // don't do this anymore
//   LibQual::set_global_qual(qv, globalnessToInt(globalness));
  xassert(this->getGlobalness()==UNDEF_Globalness);
  this->setGlobalness(globalness);
}

void CVAtomicValue_QualAnnot::setGlobalTree(Globalness globalness) {
  setGlobal(globalness);
}

void PointerValue_QualAnnot::setGlobalTree(Globalness globalness) {
  setGlobal(globalness);
  qa(v()->getAtValue())->setGlobalTree(globalness);
}

void ReferenceValue_QualAnnot::setGlobalTree(Globalness globalness) {
  setGlobal(globalness);
  qa(v()->getAtValue())->setGlobalTree(globalness);
}

void FunctionValue_QualAnnot::setGlobalTree(Globalness globalness) {
  setGlobal(globalness);
  // return value defaults to local since it is not a variable
  // parameters are visited separately since they are variables in their own right
}

void ArrayValue_QualAnnot::setGlobalTree(Globalness globalness) {
  setGlobal(globalness);
  qa(v()->atValue)->setGlobalTree(globalness);
}

void PointerToMemberValue_QualAnnot::setGlobalTree(Globalness globalness) {
  setGlobal(globalness);
  qa(v()->getAtValue())->setGlobalTree(globalness);
}

// setExternTree ****************

bool QualAnnot_ExternVisit::preVisitValue(Value *v) {
  QualAnnot *annot = qa(v);
  LibQual::Type_qualifier *qv = annot->getQVar();

  LibQual::set_extern_qual(qv, externness ? 1 : 0);

  // important counter (don't remove); see uses of getCount()
  ++counter;

  // static int i = 0;
  // i++;
  // printf("## %d. markExternVars: value=%p, qv==%p (%p) %s\n",
  //        i,
  //        v, qv, LibQual::ecr_qual_noninline(qv),
  //        name_qual(qv));

  return true;
}

bool QualAnnot_ExternVisit::preVisitVariable(Variable_O *var) {
  return serializeVar_O(var);
}

// printDeepQualifiers ****************

// sm: added this
#if USE_SERIAL_NUMBERS
  #define GETSN(x) ((x)->serialNumber)
#else
  #define GETSN(x) 0
#endif

void CVAtomicValue_QualAnnot::printDeepQualifiers() {
  std::cout << "(atom " << GETSN(v()) << " " << ::toString(getQl()) << ")";
  // FIX: Don't know if I'm supposed to check the fields of a struct
  // or not.  Wrote Jeff 31 dec 2002.
}

void PointerValue_QualAnnot::printDeepQualifiers() {
  std::cout << "(ptr " << GETSN(v()) << " " << ::toString(getQl());
  std::cout << " at:";
  qa(v()->getAtValue())->printDeepQualifiers();
  std::cout << ")";
}

void ReferenceValue_QualAnnot::printDeepQualifiers() {
  std::cout << "(ref " << GETSN(v()) << " " << ::toString(getQl());
  std::cout << " at:";
  qa(v()->getAtValue())->printDeepQualifiers();
  std::cout << ")";
}

void FunctionValue_QualAnnot::printDeepQualifiers() {
  std::cout << "(func " << GETSN(v()) << " " << ::toString(getQl());
  // FIX: how to deal with the implicit-this parameter?
  std::cout << " ret:";
  qa(v()->retValue)->printDeepQualifiers();
  std::cout << " args:";
  for(SObjListIterNC<Variable_O> params_iter(*v()->params);
      !params_iter.isDone();
      params_iter.adv()) {
    qa(params_iter.data()->abstrValue())->printDeepQualifiers();
  }
  std::cout << ")";
}

void ArrayValue_QualAnnot::printDeepQualifiers() {
  std::cout << "(array " << GETSN(v()) << " " << ::toString(getQl()) << ")";
  qa(v()->atValue)->printDeepQualifiers();
}

void PointerToMemberValue_QualAnnot::printDeepQualifiers() {
  std::cout << "(ptrtomem " << GETSN(v()) << " " << ::toString(getQl());
  std::cout << " at:";
  // FIX: if the inClass member is ever made to have qualifiers, need
  // to check it as well
  qa(v()->getAtValue())->printDeepQualifiers();
  std::cout << ")";
}

#undef GETSN

// ****************************************************************

// wrapper around raw find_qual that checks things for you
LibQual::Type_qualifier *findQualAndCheck(char const *name) {
  LibQual::Type_qualifier *qconst = LibQual::find_qual(name);
  if (!qconst) {
    // FIX: should this call report_qerror?
    printf("no such qualifier constant %s\n", // at %s:%d.\n",
           name
           // FIX: turn back on
           //               , loc.file->filename.c_str(), loc.line
           );
    throw UserError(USER_ERROR_ExitCode);
  }
  if (!LibQual::constant_qual(qconst)) {
    // FIX: should this call report_qerror?
    // FIX: this should not be able to happen, make yell louder
    printf("name does not specify a qualifier constant %s\n", // at %s:%d.\n",
           name
           // FIX: turn back on
           //               , loc.file->filename.c_str(), loc.line
           );
    throw UserError(USER_ERROR_ExitCode);
  }
  return qconst;
}

void attachOneLiteralToQvar(SourceLoc locOfAttach,
                            LibQual::Type_qualifier *qconst,
                            LibQual::Type_qualifier *qvarOfInterest)
{
  LibQual::sign_qual_t qconst_sign = LibQual::sign_qual(qconst);
  // FIX: not sure what to do with signs other than these.
  xassert(qconst_sign == LibQual::sign_pos ||
          qconst_sign == LibQual::sign_eq ||
          qconst_sign == LibQual::sign_neg);
  if (qconst_sign == LibQual::sign_pos || qconst_sign == LibQual::sign_eq) {
    mkLeq(qconst, qvarOfInterest, locOfAttach);
  }
  // reverse of above
  if (qconst_sign == LibQual::sign_neg || qconst_sign == LibQual::sign_eq) {
    mkLeq(qvarOfInterest, qconst, locOfAttach);
  }
}

void dumpQl(Value *t) {
  qa(t)->printDeepQualifiers();
  std::cout << " ql:";
  std::cout << ::toString(qa(t)->getQl());
  std::cout << std::endl;
}

bool isConstForInferencePurposes(Value *v) {
  if (v->isFunctionValue() || v->isArrayValue()) {
    // functions are const and an array is like a const pointer that
    // is elided away at compile time
    return true;
  }
  return v->t()->isConst();
}


bool isVolatileForInferencePurposes(Value *v) {
  switch(v->getTag()) {
  default:
    xfailure("bad tag");
  case Value::V_ATOMIC:
    return v->asCVAtomicValue()->type->isVolatile();
  case Value::V_POINTER:
    return v->asPointerValue()->type->isVolatile();
  case Value::V_REFERENCE:
    return v->asReferenceValue()->type->isVolatile();
  case Value::V_FUNCTION:
  case Value::V_ARRAY:
  case Value::V_POINTERTOMEMBER:
    // These don't have an isVolatile() method and I can't imagine
    // them being volatile anyway
    return false;
  }
}

// ****

// quarl 2006-05-18
//    We used to use a PtrMap from Value to QualAnnot so that Oink did not
//    need to know about annotations, and it's easy to have multiple analyses
//    work together.  That was inefficient however, so now we have a pointer
//    from Value to QualAnnot.  When we do end up adding more analyses the
//    'annotation' variable can point to a struct composed of the structs
//    needed by the annotation.

// PtrMap<Value, QualAnnot> qualAnnotMap;

// // quarl 2006-05-18
// //    There are additional time and space optimizations possible: currently
// //    there are two parallel vtables, one for Value -> CVAtomicValue, and
// //    another for QualAnnot -> CVAtomicValue_QualAnnot.  We can get rid of
// //    this virtualness to save time and space.

// void annotateWithQualAnnot(Value *v) {
//   xassert(!v->getAnnotation());
// //   printf("annotating value %p\n", v);
//   fflush(stdout);
//   QualAnnot *annot = NULL;
//   switch(v->getTag()) {
//   default: xfailure("bad tag"); break;
//   case Value::V_ATOMIC:          annot = new CVAtomicValue_QualAnnot(v);        break;
//   case Value::V_POINTER:         annot = new PointerValue_QualAnnot(v);         break;
//   case Value::V_REFERENCE:       annot = new ReferenceValue_QualAnnot(v);       break;
//   case Value::V_FUNCTION:        annot = new FunctionValue_QualAnnot(v);        break;
//   case Value::V_ARRAY:           annot = new ArrayValue_QualAnnot(v);           break;
//   case Value::V_POINTERTOMEMBER: annot = new PointerToMemberValue_QualAnnot(v); break;
//   }
//   xassert(annot);
//   xassert(annot->value == v);
//   // qualAnnotMap.add(v, annot);
//   v->setAnnotation(annot);
// }

// // TODO: make this a Value member function.
// static QualAnnot *qa_internal(Value *v) {
//   xassert(v);
//   // QualAnnot *ret = qualAnnotMap.get(v);
//   QualAnnot *ret = static_cast<QualAnnot*>(v->getAnnotation());

//   if (!ret) {
//     annotateWithQualAnnot(v);
//     ret = static_cast<QualAnnot*>(v->getAnnotation());
//   }
//   xassert(ret);
//   return ret;
// }
// static QualAnnot *qa_internal(Value const *v) {
//   return qa_internal(const_cast<Value*>(v));
// }

QualAnnot *qa(Value const *v) {
  return static_cast<QualAnnot*>( const_cast<Value*>(v)->getAnnotation0() );
}

// // bool hasQualAnnot(Value const *v) { return qualAnnotMap.get(v)!=NULL; }
// bool hasQualAnnot(Value const *v) {
//   return v->getAnnotation() != NULL;
// }

// TODO: make these member functions

// #define DEFINE_QA(VALUENAME)
//   VALUENAME ## _QualAnnot        *qa(VALUENAME *v)
//   { return dynamic_cast<VALUENAME ## _QualAnnot*>(qa_internal(v)); }
//   VALUENAME ## _QualAnnot        *qa(VALUENAME const *v)
//   { return dynamic_cast<VALUENAME ## _QualAnnot*>(qa_internal(v)); }

#define DEFINE_QA(VALUENAME)                                      \
  VALUENAME##_QualAnnot *qa(VALUENAME##_Q const *v)               \
  {                                                               \
    return const_cast<VALUENAME##_Q*>(v)->getAnnotation();        \
  }                                                               \
                                                                  \
  VALUENAME##_QualAnnot *qa(VALUENAME const *v)                   \
  {                                                               \
    return qa(as##VALUENAME##_Q(const_cast<VALUENAME*>(v)));      \
  }

DEFINE_QA(CVAtomicValue)
DEFINE_QA(PointerValue)
DEFINE_QA(ReferenceValue)
DEFINE_QA(FunctionValue)
DEFINE_QA(ArrayValue)
DEFINE_QA(PointerToMemberValue)

#undef DEFINE_QA
