// see License.txt for copyright and terms of use

#include "value.h"              // this module
#include "oink_global.h"        // tFac
#include "oink_util.h"          // USER_ERROR


bool value2typeIsOn = false;

// TODO: rename/refactor this, since Oink bool is really just another annotation
bool Value::allow_annotation = false;

Value *Expression::getAbstrValue() {
  if (!abstrValue) {
    xassert(type);
    // I think most expressions should have their abstract value set
    // during the ValueASTVisitor walk.  This must be here for an
    // exceptional circumstance.  Therefore, I don't think it will be
    // often that this unknown location will come up.
    abstrValue = vFac->buildValue(type, SL_UNKNOWN);
  }
  return abstrValue;
};

bool Expression::hasAbstrValue() {
  return abstrValue;
}

// ctor
Value::Value(SourceLoc loc0)
  : loc(loc0)
  , refToMe(NULL)
  , ptrToMe(NULL)
{
}

ReferenceValue *Value::asLval() {
  // we shouldn't be getting any error types at this stage
  xassert(!t()->isError());

  if (isReferenceValue()) {
    return this->asReferenceValue();
  }

  // be idempotent
  if (!refToMe) {
    ReferenceType *refType = tFac->makeReferenceType(t())->asReferenceType();
    refToMe = vFac->alloc_ReferenceValue();
    refToMe->init(refType, loc, this);
  }
  xassert(refToMe->atValue == this);
  return refToMe;
}

Value *Value::asRval() {
  if (isReferenceValue()) {
    xassert(asReferenceValue()->atValue->refToMe == this);
    return asReferenceValue()->atValue;
  }
  return this;
}

DOWNCAST_IMPL(Value, CVAtomicValue)
DOWNCAST_IMPL(Value, PointerValue)
DOWNCAST_IMPL(Value, ReferenceValue)
DOWNCAST_IMPL(Value, FunctionValue)
DOWNCAST_IMPL(Value, ArrayValue)
DOWNCAST_IMPL(Value, PointerToMemberValue)

void Value::setUsedInDataflow(bool usedInDataflow0) {
  xassert(!allow_annotation);

  ValueAnnotation_O *va = (ValueAnnotation_O*) getAnnotation0();
  va->usedInDataflow = usedInDataflow0;
}

bool Value::getUsedInDataflow() const {
  xassert(!allow_annotation);
  ValueAnnotation_O *va = (ValueAnnotation_O*) const_cast<Value*>(this)->getAnnotation0();
  return va->usedInDataflow;
}

// void Value::setAnnotation(ValueAnnotation *annotation0) {
//   xassert(allow_annotation);
//   xassert(!annotation);        // setting an annotation is idempotent
//   annotation = annotation0;
// }

// ValueAnnotation *Value::getAnnotation() const {
//   xassert(allow_annotation);
//   // NOTE: do not change this to make an annotation if it is missing;
//   // just let it return NULL
//   return annotation;
// }


// FIX: this is duplicated with some other function I think
void Value_gdb(Value *v) {
  printf("Value of type ");
  v->t()->gdb();
}

// ----------------------- CVAtomicValue ---------------------

bool CVAtomicValue::hasAtValue() {
  return type->isIntegerType() || type->isEllipsis();
}

Value *CVAtomicValue::getAtValue() {
  // this only makes sense for now if it is an integer being treated
  // as a pointer
  //
  // Note: an integer has an atValue of void, even if the void is not being
  // treated as an autounion.
  xassert(hasAtValue());
  if (!atVoid) {
    atVoid = vFac->makeAtVoid(this);
  }
  return atVoid;
}

Value *CVAtomicValue::getInstanceSpecificValue
  (string const &memberName0, Value *shouldBeValue)
{
  // FIX: memberName0 could be made into a StringRef if it weren't for
  // the fact that this method does double duty; it is used for both:
  //
  // 1) fields of structs where the member name passed in is a
  // Variable's name and therefore should already be a StringRef; this
  // should be checked before we depend on it though, and
  //
  // 2) "fields" of voids that are being an automatic union where the
  // name is mangled into existance.  It should be safe there as well
  // if mangleToStringRef_TypeEquiv() is used; this should be ok
  // already.
  StringRef memberName = globalStrTable(memberName0);

  // dump this thing out
//    ((Scope*) (this->atomic->asCompoundType()) ) ->dump();
  xassert(shouldBeValue);

  // methods are not instance-specific
  bool functionValueOk = isAutoUnionValue(this);
  if (!functionValueOk) {
    xassert(t()->isCompoundType());
    if (shouldBeValue->isFunctionValue()) {
      return shouldBeValue;
    }
  }

  xassert(oinkCmd->instance_sensitive);
//    if (!oinkCmd->instance_sensitive) {
//  //      // FIX: This doesn't work in the presence of inheritance
//  //      Value *t = atomic->asCompoundType()->rawLookupVariable(memberName)->abstrValue();
//  //      // methods are not instance-specific
//  //      xassert(!t->isFunctionValue());
//  //      xassert(shouldBeValue->equals(t));
//  //      return t;
//      return shouldBeValue;
//    }

  // FIX: for now we must not be in C++
  if (anyCplusplus) {
    userFatalError(SL_UNKNOWN, "Instance-sensitive analysis not implemented for C++");
  }

  // get the type for this name and this instance, creating lazily if
  // necessary
  xassert(memberName[0]);
  BucketOValues *bucket = getBucketCreate();
  Value *value = bucket->get(memberName);
  SourceLoc loc = SL_UNKNOWN;
  if (!value) {
    if (type->isCompoundType()) {
      Variable_O *mem = asVariable_O
        (type->atomic->asCompoundType()->rawLookupVariable(memberName));
      value = mem->abstrValue();
      xassert(shouldBeValue->t()->equals(value->t()));
      loc = mem->loc;
    } else if (isAutoUnionValue(this)) {
      xassert(!isAutoUnionValue(shouldBeValue));
      value = shouldBeValue;
      loc = shouldBeValue->loc;
    } else {
      xfailure(stringc << "getInstanceSpecificValue() called on a " << t()->toMLString());
    }
    value = vFac->buildValue(value->t(), loc);
    vFac->finishJustClonedMember(this, memberName, value); // fix the name
    bucket->put(memberName, value); // put it into the bucket
  }
  if (!functionValueOk) {
    // methods are not instance-specific
    xassert(!value->isFunctionValue());
  }

  return value;
}

// ------------------------- PointerValue ------------------------

// ------------------------- ReferenceValue ------------------------

// --------------------------- FunctionValue -----------------

int FunctionValue::maxNthParamIndex() {
  return 1                      // the return value
    + params->count()           // number of normal params
    + (hasEllipsis()?1:0)       // the ellipsis
    - 1;                        // minus 1 because we start counting at 0
}

Value *FunctionValue::nthParamAbstrValue(int n) {
  // NOTE: getInstanceSpecificValue() would be redundant here as there
  // is no chance a parameter is the member of a struct
  xassert(n>=0);
  Value *ret = NULL;
  if (n==0) {
    ret = retValue;
  }
  else if (n-1 < params->count()) {
    ret = params->nth(n-1)->abstrValue();
  } else if (n-1 == params->count()) {
    xassert(hasEllipsis());
    ret = getEllipsis()->abstrValue();
  } else xfailure("nthParamAbstrValue: n too big");
  return ret;
}

Variable_O *FunctionValue::getRetVar() {
  // is not well-defined for non-compound types
  xassert(retValue->t()->isCompoundType());
  // FIX: This is broken.  Turn this assertion on and remove the
  // conditional expression below.
//    xassert(retVar);
  if (!retVar) {
//      xfailure("why making a ret var?");
    retVar =
      asVariable_O(tFac->makeVariable
                   (loc,
                    globalStrTable("<retVar>"),
                    retValue->asLval()->t(),
                    DF_PARAMETER));
    retVar->setReal(true);
    retVar->setFilteredKeep(true);
  }
  return retVar;
}

void FunctionValue::registerRetVar(Variable *retVar0) {
  xassert(retVar0);
  retVar = asVariable_O(retVar0);
}

bool FunctionValue::hasEllipsis() const {
  // Note that hasEllipsis() can be false if 1) there is no
  // ellipsisPtr or if there is one but it is NULL; this can happen if
  // two FF_NO_PARAM_INFO FunctionValues have met and neither one had
  // an ellipsis variable at the time but they share ellipsisPtr; they
  // both may get one later.
  return const_cast<FunctionValue*>(this)->getEllipsis() != NULL;
}

Variable_O *FunctionValue::getEllipsis() {
  return (ellipsisHolder!=NULL) ? ellipsisHolder->ellipsis : NULL;
}

void FunctionValue::setEllipsis(Variable_O *ellipsis0) {
  if (!ellipsisHolder) ellipsisHolder = new FVEllipsisHolder;
  ellipsisHolder->ellipsis = ellipsis0;
}

Variable_O *FunctionValue::ensureEllipsis() {
  if (hasEllipsis()) return getEllipsis();
  Variable_O *ellipsisVar =
    asVariable_O
    (tFac->makeVariable
     (loc,
      NULL /*name*/,
      tFac->getSimpleType(ST_ELLIPSIS),
      DF_PARAMETER));
  ellipsisVar->setReal(true);
  ellipsisVar->setFilteredKeep(true);
  setEllipsis(ellipsisVar);
  return ellipsisHolder->ellipsis;
}

// is this function type from a K and R declaration (including
// definitions)
bool FunctionValue::isKandR() {
  // K and R definition
  if (type->hasFlag(FF_KANDR_DEFN)) return true;
  // K and R declaration
  if (type->hasFlag(FF_NO_PARAM_INFO) && !hasEllipsis()) return true;
  // neither
  return false;
}

// ----------------------- ArrayValue -----------------
// nothing for now

// ------------------------- PointerToMemberValue ------------------------
// nothing for now


// anyCtorSatisfies ****************

bool StatelessValuePred::operator() (Value const *t)
{
  return f(t);
}

bool Value::anyCtorSatisfiesF(ValuePredicateFunc *f) const
{
  StatelessValuePred stp(f);
  return anyCtorSatisfies(stp);
}

bool CVAtomicValue::anyCtorSatisfies(ValuePred &pred) const
{
  return pred(this);
}

bool PointerValue::anyCtorSatisfies(ValuePred &pred) const
{
  return pred(this) ||
         atValue->anyCtorSatisfies(pred);
}

bool ReferenceValue::anyCtorSatisfies(ValuePred &pred) const
{
  return pred(this) ||
         atValue->anyCtorSatisfies(pred);
}

// FIX: the exception spec should probably be included into the
// dataflow
//  bool FunctionValue::ExnSpec::anyCtorSatisfies(ValuePred &pred) const
//  {
//    SFOREACH_OBJLIST(Value, values, iter) {
//      if (iter.data()->anyCtorSatisfies(pred)) {
//        return true;
//      }
//    }
//    return false;
//  }

bool parameterListCtorSatisfies(ValuePred &pred,
                                SObjList<Variable_O> const &params)
{
  SFOREACH_OBJLIST(Variable_O, params, iter) {
    if (iter.data()->abstrValue()->anyCtorSatisfies(pred)) {
      return true;
    }
  }
  return false;
}

bool FunctionValue::anyCtorSatisfies(ValuePred &pred) const
{
  return pred(this) ||
         retValue->anyCtorSatisfies(pred) ||
         parameterListCtorSatisfies(pred, *params)
    // FIX: should include the exception spec into the dataflow
//      || (exnSpec && exnSpec->anyCtorSatisfies(pred))
    ;

  // FIX: FUNC TEMPLATE LOSS
  //
  // UPDATE: this is actually symmetric with the way that compound
  // type templates are dealt with, which is to say we do not recurse
  // on the parameters
//      || (templateInfo && templateInfo->anyParamCtorSatisfies(pred));
}

Variable_O *ArrayValue::getAtVar() {
  // FIX: maybe this variable should have flag DF_MEMBER
  // NOTE: I don't think this is one of those variables to put into builtinVars
  if (!atVar) {
    atVar = asVariable_O(tFac->makeVariable(loc, "<phantom Array::atVar>", atValue->t(), DF_NONE));
    atVar->setReal(true);
    atVar->setFilteredKeep(true);
    // NOTE: it is very important that we re-use the same abstrValue
    atVar->setAbstrValue(atValue);
  }
  return atVar;
}

bool ArrayValue::anyCtorSatisfies(ValuePred &pred) const
{
  return pred(this) ||
         atValue->anyCtorSatisfies(pred);
}

bool PointerToMemberValue::anyCtorSatisfies(ValuePred &pred) const
{
  return pred(this) ||
         atValue->anyCtorSatisfies(pred);
}


// traverse methods ****************

void CVAtomicValue::traverse(ValueVisitor &vis) {
  if (!vis.preVisitValue(this)) return;

  // for the int-as-void* feature, we need to treat any int as if it
  // were a void*, and therefore it needs to point at a void; this is
  // constructed lazily so may be NULL
  if (atVoid) atVoid->traverse(vis);

  // recall that a cvatomic of a void can act as a union of many
  // possible things and a struct/class has instance-specific fields
  //
  // FIX: include the traversal of the elements of the void
  //
  // FIX: include the traversal of the struct/class has
  // instance-specific fields

  vis.postVisitValue(this);
}

void PointerValue::traverse(ValueVisitor &vis) {
  if (!vis.preVisitValue(this)) return;
  atValue->traverse(vis);
  vis.postVisitValue(this);
}

void ReferenceValue::traverse(ValueVisitor &vis) {
  if (!vis.preVisitValue(this)) return;
  atValue->traverse(vis);
  vis.postVisitValue(this);
}

void FunctionValue::traverse(ValueVisitor &vis) {
  if (!vis.preVisitValue(this)) return;

  retValue->traverse(vis);

  if (retVar) {
    retVar->traverse(vis);
  }

  SFOREACH_OBJLIST_NC(Variable_O, *params, iter) {
    iter.data()->traverse(vis);
  }

  if (hasEllipsis()) {
    getEllipsis()->traverse(vis);
  }

  // FIX: do this someday when exception specs have values, but not
  // needed for C
//    ExnSpec *exnSpec;             // (nullable owner)

  vis.postVisitValue(this);
}

void ArrayValue::traverse(ValueVisitor &vis) {
  if (!vis.preVisitValue(this)) return;
  atValue->traverse(vis);
  vis.postVisitValue(this);
}

void PointerToMemberValue::traverse(ValueVisitor &vis) {
  if (!vis.preVisitValue(this)) return;
  atValue->traverse(vis);
  vis.postVisitValue(this);
}


// ParamIter ****************

ParamIter::ParamIter(FunctionValue *fv0)
  : iter(*fv0->params)
  , fv(fv0)
{}

bool ParamIter::isDone() const {
  return iter.isDone() && !fv->hasEllipsis();
}

// void ParamIter::adv() {
//   if (!iter.isDone()) {
//     iter.adv();
//   } else if (fv->hasEllipsis()) {
//     // we were on the ellipsis, signal we've moved off by nullifying
//     // it so isDone() will return true
//     //
//     // dsw: NOTE: if this can happen, have to add a bool flag
//     // somewhere as we should NOT alter the ellipsis of fv
//     xfailure("can this happen?");
// //     ellipsis = NULL;
//   } else {
//     xfailure("can't call adv() on isDone() iterator");
//   }
// }

void ParamIter::advanceButNotPastEllipsis() {
  if (!iter.isDone()) {
    iter.adv();
  } else {
    // better be on the ellipsis, otherwise we just
    // tried to advance an isDone() iterator
    xassert(fv->hasEllipsis());
  }
}

Variable_O *ParamIter::data() const {
  if (!iter.isDone()) return iter.data();
  else return fv->getEllipsis();
}


// ParamIter_NO_PARAM_INFO ****************

ParamIter_NO_PARAM_INFO::ParamIter_NO_PARAM_INFO(FunctionValue *fv0)
  : fv(fv0)
  , phantomVar(NULL)
{
  xassert(!fv->hasEllipsis());
}

void ParamIter_NO_PARAM_INFO::adv() {
  phantomVar = NULL;
}

Variable_O *ParamIter_NO_PARAM_INFO::data(Type *matchingParamType) {
  // WARNING: quadratic time append; FIX: make FunctionValue::param a
  // TailList<Variable_O> *params instead of the SObjList<Variable_O>*
  // that it is now; NOTE: that change involves adding TailList as a
  // new kind of list that can be serialized which is why I haven't
  // done it yet.
  if (!phantomVar) {
    phantomVar =
      asVariable_O(tFac->makeVariable
                   (fv->loc,
                    globalStrTable("<FF_NO_PARAM_INFO phantom var>"),
                    // this is from when we just made all phantom
                    // params 'int' but that doesn't work for matching
                    // parameters that are a CompoundType
//                     tFac->getSimpleType(ST_INT),
                    tFac->shallowCloneType(matchingParamType),
                    DF_PARAMETER));
    phantomVar->setReal(true);
    phantomVar->setFilteredKeep(true);
    fv->params->append(phantomVar);
  }
  return phantomVar;
}


// class BucketOValues ****************

// helper utilities

// BucketOValues *BucketOValues_factory() {
//   return new BucketOValues;
// }

// bool BucketOValues_chooseEnabled(BucketOValues *a, BucketOValues *b) {
//   if (a->isEnabled()) return true;
//   if (b->isEnabled()) return false;
//   xfailure("both disabled");
// }

// ****

BucketOValues::BucketOValues()
//  #if !defined(NDEBUG)
  : enabled(true)
//  #endif
{}

void BucketOValues::disable() {
//  #if !defined(NDEBUG)
  enabled = false;
//  #endif
}

Value *BucketOValues::get(string const &key) {
  xassert(enabled);
  return nameToValue.queryif(key);
}

Value *BucketOValues::getEvenThoughDisabled(string const &key) {
  return nameToValue.queryif(key);
}

void BucketOValues::put(string const &key, Value *value) {
  xassert(enabled);
  xassert(!key.empty());
  xassert(value);
  xassert(!nameToValue.isMapped(key));
  return nameToValue.add(key, value);
}

// for all the fields that the other has that we do not, copy them
// to us
void BucketOValues::getOthersExtraFields(BucketOValues &other) {
  // this action really must never be allowed if this->enabled is
  // false; see comments in the body of
  // DataFlowTy::tDataFlow_CVAtomicUnifyFields()
//   xassert(enabled);             // this assertion is critical to the correctness
  // I want to make sure that the xassert macro will never turn this
  // off, so I write it out by hand instead
  if (!enabled) {
    xfailure("BucketOValues::getOthersExtraFields() called while enabled is false");
    exit(INTERNALERROR_ExitCode);
  }

  for (StringSObjDict<Value>::Iter iter(other.nameToValue);
       !iter.isDone();
       iter.next()) {
    string const &key = iter.key();
    Value *value = nameToValue.queryif(key);
    if (!value) {
      Value *valueOther = other.nameToValue.queryif(key);
      xassert(valueOther);
      nameToValue.add(key, valueOther);
    }
  }
}

void indent(int indentation) {
  for (int i=0; i<indentation; ++i) {
    std::cout << "\t";
  }
}

//  void BucketOValues::dump(int indentation) {
//    indent(indentation);
//    printf("%p ", this);
//    cout << "{" << endl;
//    for (StringSObjDict<Value>::Iter iter(nameToValue);
//         !iter.isDone();
//         iter.next()) {
//      string const &key = iter.key();
//      Value *value = nameToValue.queryif(key);
//      string valueStr = "NULL";
//      if (value) valueStr = value->toMLString();
//      indent(indentation);
//      cout << "\t'" << key << "' -> '" << valueStr << "'(" << value << ")" << endl;
//    }
//    indent(indentation);
//    cout << "}" << endl;
//  }

// class BucketOValues::PairIter ****************

BucketOValues::PairIter::PairIter(BucketOValues &thisBucket0, BucketOValues &other)
  : thisBucket(thisBucket0)
  , otherIter(other.nameToValue)
  , otherBucket(other)
{
  // xassert(other.enabled);
  // we have to get up to the first element in the other that we
  // also have
  stopAtNextMatch();
}

void BucketOValues::PairIter::stopAtNextMatch() {
  while (!otherIter.isDone() && !thisBucket.nameToValue.queryif(otherIter.key())) {
    otherIter.next();
  }
  // check the current key is in both
  if (!otherIter.isDone()) {
    rostring key = otherIter.key();
    xassert(thisBucket.nameToValue.queryif(key));
    xassert(otherBucket.nameToValue.queryif(key));
  }
}

bool BucketOValues::PairIter::isDone() {
  return otherIter.isDone();
}

void BucketOValues::PairIter::adv() {
  // NOTE: you can do this with a do-while loop, but it is just
  // too funky
  otherIter.next();
  stopAtNextMatch();
}

string BucketOValues::PairIter::data() {
  string key = otherIter.key();
  // NOTE: this is expensive, but it will turn off when you turn off
  // debugging
  xassertdb(thisBucket.nameToValue.queryif(key));
  xassertdb(otherBucket.nameToValue.queryif(key));
  return key;
}


// ValueFactory methods ****************

// template <class T>
// static inline T *allocWithAnnot(IterableBlockAllocator &allocator)
// {
//   return &allocator.alloc<T::MyStruct>()->t1;
// }

#define DEFINE_ALLOC_VALUE(CLS)                                   \
  CLS *ValueFactory::alloc_##CLS() {                              \
    return &allocator.alloc<CLS::MyStruct>()->t1;                 \
  }

#define DEFINE_GET_ANNOTATION(CLS)                                \
  ValueAnnotation_O *CLS::getAnnotation() {                       \
    return &CLS::MyStruct::getStructFrom1(this)->t2;              \
  }

FOREACH_CLASS_Value(DEFINE_ALLOC_VALUE)

FOREACH_CLASS_Value(DEFINE_GET_ANNOTATION)

#undef DEFINE_ALLOC_VALUE
#undef DEFINE_GET_ANNOTATION

void CVAtomicValue::init(CVAtomicType *type0, SourceLoc loc0) {
  xassert(!type0->isError());
  type = type0;
  loc = loc0;
}

void PointerValue::init(PointerType *type0, SourceLoc loc0, Value *atValue0) {
  xassert(!type0->isError());
  type = type0;
  loc = loc0;
  atValue = atValue0;
  atValue->ptrToMe = this;
}

void ReferenceValue::init(ReferenceType *type0, SourceLoc loc0, Value *atValue0) {
  xassert(!type0->isError());
  type = type0;
  loc = loc0;
  atValue = atValue0;
  atValue->refToMe = this;
}

void FunctionValue::init(FunctionType *type0, SourceLoc loc0, Value *retValue0) {
  xassert(!type0->isError());
  type = type0;
  loc = loc0;
  retValue = retValue0;
}

// TODO: assert that atValue->ptrToMe == NULL ?
void ArrayValue::init(ArrayType *type0, SourceLoc loc0, Value *atValue0) {
  xassert(!type0->isError());
  type = type0;
  loc = loc0;
  atValue = atValue0;
  atValue->ptrToMe = this;
}

void PointerToMemberValue::init(PointerToMemberType *type0,
                                SourceLoc loc0, Value *atValue0) {
  xassert(!type0->isError());
  type = type0;
  loc = loc0;
  atValue = atValue0;
}

// union-find **********

// Union two keys (CVAtomicValues)
void CVAtomicValue::unifyKey(CVAtomicValue *tgt) {
  CVAtomicValue *src = this;
  xassert(src);
  xassert(tgt);
  CVAtomicValue *src0 = src->find();
  xassert(src0);
  CVAtomicValue *tgt0 = tgt->find();
  xassert(tgt0);
  if (src0 != tgt0) {
    // Arbitrarily choose src0
    src0->ufLinkForEquivRelOfDomOfMapToBucket = tgt0;
    // possible optimization: instead of src->find(), just replace the ufLinks
    // as they're being traversed
  }
}

CVAtomicValue *CVAtomicValue::find() {
  if (this->ufLinkForEquivRelOfDomOfMapToBucket == NULL) {
    // Optimization; 'this' is the representative; nothing to compress
    return this;
  }

  // find root
  CVAtomicValue *ufRoot = this->ufLinkForEquivRelOfDomOfMapToBucket;

  if (ufRoot->ufLinkForEquivRelOfDomOfMapToBucket == NULL) {
    // Optimization: this->ufLink is the representative; nothing to compress
    return ufRoot;
  }

  ufRoot = ufRoot->ufLinkForEquivRelOfDomOfMapToBucket;

  while (ufRoot->ufLinkForEquivRelOfDomOfMapToBucket != NULL)
    ufRoot = ufRoot->ufLinkForEquivRelOfDomOfMapToBucket;

  // compress path
  CVAtomicValue *cur = this;
  while (cur->ufLinkForEquivRelOfDomOfMapToBucket != ufRoot) {
    CVAtomicValue *next = cur->ufLinkForEquivRelOfDomOfMapToBucket;
    cur->ufLinkForEquivRelOfDomOfMapToBucket = ufRoot;
    cur = next;
  }

  return ufRoot;
}

void CVAtomicValue::makeRepresentative()
{
  if (this->ufLinkForEquivRelOfDomOfMapToBucket == NULL) {
    // this is the only item in its class, or it is already the root.
    return;
  } else {
    // make this CVAtomicValue the root.
    xassert(this->bucket == NULL);
    CVAtomicValue *root = find();
    this->bucket = root->bucket;
    root->bucket = NULL;
    root->ufLinkForEquivRelOfDomOfMapToBucket = this;
    this->ufLinkForEquivRelOfDomOfMapToBucket = NULL;
  }
}

const char *getValueName(Value *v);

BucketOValues *CVAtomicValue::unifyBuckets(CVAtomicValue *tgt)
{
  CVAtomicValue *src = this;
  xassert(src);
  xassert(tgt);
  // get the two eq-class reps before the union
  CVAtomicValue *srcRep = src->find();
  CVAtomicValue *tgtRep = tgt->find();
  // if they are the same, we are done
  // static int i = 0;
  // fprintf(stderr, "## %d. unifyBuckets: src=%p (%p %s), tgt=%p (%p %s)\n",
  //         ++i,
  //         src, srcRep, getValueName(src),
  //         tgt, tgtRep, getValueName(tgt));
  if (srcRep == tgtRep) {
    srcRep->getBucketCreate();  // they should both have a bucket
    xassert(tgtRep->getBucket());
    return NULL;
  }
  // do the union
  srcRep->unifyKey(tgtRep);
  // get the new rep for both
  xassert(srcRep->find() == src->find());
  CVAtomicValue *mergedRep = srcRep->find();
  CVAtomicValue *mergedOther = (mergedRep==srcRep ? tgtRep : srcRep);

  // get their old values
  BucketOValues *srcValue = srcRep->bucket;
  BucketOValues *tgtValue = tgtRep->bucket;
  // if a value is discarded, sort it here so we can return it to the
  // user in case there is more merging for the client to do
  BucketOValues *discardedValue = NULL;

  // merge their values
  if (tgtValue && srcValue) {
    if (tgtValue == srcValue) {
      // there is nothing to do
//          cout << "\tcase 1: nothing to do" << endl;
    } else {
//          cout << "\tcase 5: merge the two values, ";
      if (tgtValue->isEnabled()) {
        // tgtValue is chosen
//            cout << "tgtValue is chosen" << endl;
        mergedRep->bucket = tgtValue;
        discardedValue = srcValue;
        discardedValue->disable();
        xassert(srcRep == mergedOther);
        srcRep->bucket = NULL;
      } else if (srcValue->isEnabled()) {
        // srcValue is chosen
//            cout << "srcValue is chosen" << endl;
        mergedRep->bucket = srcValue;
        discardedValue = tgtValue;
        discardedValue->disable();
        xassert(tgtRep == mergedOther);
        tgtRep->bucket = NULL;
      } else {
        xfailure("both disabled");
      }
    }
  } else if (tgtValue && !srcValue) {
    // use tgtValue for both
//        cout << "\tcase 2: use tgtValue for both" << endl;
    mergedRep->bucket = tgtValue;
    mergedOther->bucket = NULL;
  } else if (!tgtValue && srcValue) {
    // use srcValue for both
//        cout << "\tcase 3: use srcValue for both" << endl;
    mergedRep->bucket = srcValue;
    mergedOther->bucket = NULL;
  } else if (!tgtValue && !srcValue) {
    // make a new one and share it for both
//        cout << "\tcase 4: make a new value and share it for both" << endl;
    mergedRep->bucket = new BucketOValues;
  } else {
    xfailure("internal error 30812c73-8559-4ae2-8c2d-1987ab959378");
  }

  xassert(srcRep->bucket == NULL && tgtRep->bucket != NULL ||
          srcRep->bucket != NULL && tgtRep->bucket == NULL);

  xassert(src->getBucket() && src->getBucket() == tgt->getBucket());
  return discardedValue;
}


// build value functions ****************

CVAtomicValue *ValueFactory::buildCVAtomicValue(CVAtomicType *src, SourceLoc loc) {
  CVAtomicValue *v = alloc_CVAtomicValue();
  v->init(src, loc);
  return v;
}

// TODO: is this inefficiently creating another value for the atValue?
PointerValue *ValueFactory::buildPointerValue(PointerType *src, SourceLoc loc) {
  PointerValue *v = alloc_PointerValue();
  v->init(src, loc, buildValue(src->getAtType(), loc));
  return v;
}

ReferenceValue *ValueFactory::buildReferenceValue(ReferenceType *src, SourceLoc loc) {
  ReferenceValue *v = alloc_ReferenceValue();
  v->init(src, loc, buildValue(src->getAtType(), loc));
  return v;
}

FunctionValue *ValueFactory::buildFunctionValue(FunctionType *src, SourceLoc loc) {
  FunctionValue *v = alloc_FunctionValue();
  v->init(src, loc, buildValue(src->retType, loc));

  for (SObjListIterNC<Variable> paramIter(src->params);
       !paramIter.isDone();
       paramIter.adv()) {
    Variable_O *paramClone = tFac->cloneVariable_O(asVariable_O(paramIter.data()));
    paramClone->setReal(true);
    paramClone->setFilteredKeep(true);
    paramClone->setFlag(DF_PARAMETER);
    v->params->append(paramClone);
  }

  if (src->hasFlag(FF_VARARGS)) {
    xassert(v->type->hasFlag(FF_VARARGS));
    xassert(!v->hasEllipsis());
    Variable_O *ellipsisVar =
      asVariable_O
      (tFac->makeVariable
       (loc,
        NULL /*name*/,
        tFac->getSimpleType(ST_ELLIPSIS),
        DF_PARAMETER));
    ellipsisVar->setReal(true);
    ellipsisVar->setFilteredKeep(true);
    v->setEllipsis(ellipsisVar);
  } else {
    // FIX: remove this
    xassert(!v->type->hasFlag(FF_VARARGS));
  }
  // FIX: remove this
  if (v->hasEllipsis()) {
    xassert(v->getEllipsis()->hasFlag(DF_PARAMETER));
  }

//    if (src->retVar) {
//      ret->retVar = cloneVariable_O(src->retVar);
//      ret->retVar->setReal(true);
//      ret->setFilteredKeep(true);
//      xassert(ret->retVar->hasFlag(DF_PARAMETER));
//    }

  // FIX: omit these for now.
//    ret->exnSpec = exnSpec->deepClone();
//    ret->templateParams = templateParams->deepClone();

  return v;
}

ArrayValue *ValueFactory::buildArrayValue(ArrayType *src, SourceLoc loc) {
  ArrayValue *v = alloc_ArrayValue();
  v->init(src, loc, buildValue(src->getAtType(), loc));
  return v;
}

PointerToMemberValue *ValueFactory::buildPointerToMemberValue
  (PointerToMemberType *src, SourceLoc loc) {
  PointerToMemberValue *v = alloc_PointerToMemberValue();
  v->init(src, loc, buildValue(src->getAtType(), loc));
  return v;
}

Value *ValueFactory::buildValue(Type *src, SourceLoc loc)
{
  // This is a ctor of Values from Types
  Value *ret = NULL;
  switch (src->getTag()) {
  default: xfailure("bad type tag"); break;
  case Type::T_ATOMIC:    ret = buildCVAtomicValue (src->asCVAtomicType(), loc);  break;
  case Type::T_POINTER:   ret = buildPointerValue  (src->asPointerType(), loc);   break;
  case Type::T_REFERENCE: ret = buildReferenceValue(src->asReferenceType(), loc); break;
  case Type::T_FUNCTION:  ret = buildFunctionValue (src->asFunctionType(), loc);  break;
  case Type::T_ARRAY:     ret = buildArrayValue    (src->asArrayType(), loc);     break;
  case Type::T_POINTERTOMEMBER:
    ret = buildPointerToMemberValue(src->asPointerToMemberType(), loc); break;
  }
  return ret;
}

CVAtomicValue *ValueFactory::makeAtVoid(CVAtomicValue const *t) const {
  // TODO: voidType and voidPtrType can be created once, statically
  Type *voidType = tFac->getSimpleType(ST_VOID);
  Type *voidPtrType = tFac->makePointerType(CV_NONE, voidType);

  // quarl 2006-05-22
  //   Build a 'void*' and then return the thing it's pointing at (the
  //   'void').  The reason for this indirection is to make sure the void has
  //   'ptrToMe' set, which isAutoUnionValue() requires.  Would it make sense
  //   to have ptrToMe be the 'int' ?
  //   TODO: This feels like a kludge; the parser should make it easier to
  //   distinguish the two.
  //   TODO: this doesn't handle the case "void x;".

  // return vFac->buildValue(voidType, t->loc)->asCVAtomicValue();

  Value *ptrToValue = vFac->buildValue(voidPtrType, t->loc);
  return ptrToValue->getAtValue()->asCVAtomicValue();
}


// debugging utilities as gdb has problems calling Value methods ****

#if USE_SERIAL_NUMBERS
int valSN(Value *value) {
  return value->serialNumber;
}
#endif

Type *valT(Value *value) {
  return value->t();
}

void valgdb(Value *value) {
  value->t()->gdb();
}

Value *valAsRval(Value *value) {
  return value->asRval();
}

Value *valAsLval(Value *value) {
  return value->asLval();
}

bool isAutoUnionValue(Value *value) {
  Type *t = value->t();
  // quarl 2006-05-22:
  //   The 'void' to which a 'void *' points is an auto union, but the 'void'
  //   return type is not an auto union.
  return (t->isVoid() && value->ptrToMe != NULL) || t->isEllipsis();
}

// #include "qual_annot.h"
const char *getValueName(Value *v)
{
  // return qa(v)->name.c_str();
  return "?";
}

void CVAtomicValue::debugDumpLinkBucket()
{
  if (this->getBucket()) {
    static int i = 0;
    fprintf(stderr, "    %s %2d. %p (%s): link=%p, bucket=%p%s, getBucket()=%p\n",
            (this->ufLinkForEquivRelOfDomOfMapToBucket == NULL ? "***" : "   "),
            ++i,
            this, getValueName(this), this->ufLinkForEquivRelOfDomOfMapToBucket,
            this->bucket, (this->bucket ? (this->bucket->isEnabled() ? " (enabled)"
                                           : " (disabled)")
                           : ""),
            this->getBucket());
  }

  if (this->ufLinkForEquivRelOfDomOfMapToBucket) {
    xassert(this->bucket == NULL /*|| ! this->bucket->isEnabled()*/);
  }
}

/* static */
void CVAtomicValue::dumpBuckets()
{
  fprintf(stderr, "## dumpBuckets:\n");

  // for (Value *v = Value::valueList; v != NULL; v = v->next)
  // {
  //   if (v->isCVAtomicValue()) {
  //     v->asCVAtomicValue()->debugDumpLinkBucket();
  //   }
  // }

  PtrMap<BucketOValues, PtrSet<CVAtomicValue> > sortedByBuckets;

  FOREACH_VALUE(iter) {
    Value *v = iter.data<Value>();
    if (v->isCVAtomicValue()) {
      CVAtomicValue *a = v->asCVAtomicValue();
      BucketOValues *b = a->getBucket();
      if (!b) continue;

      if (sortedByBuckets.get(b) == NULL) {
        sortedByBuckets.add(b, new PtrSet<CVAtomicValue> () );
      }

      sortedByBuckets.get(b)->add(a);
    }
  }

  for (PtrMap<BucketOValues, PtrSet<CVAtomicValue> >::Iter iter(sortedByBuckets);
       !iter.isDone(); iter.adv())
  {
    PtrSet<CVAtomicValue> *atoms = iter.value();
    for (PtrSet<CVAtomicValue>::Iter iter(*atoms); !iter.isDone(); iter.adv()) {
      CVAtomicValue *a = iter.value();
      a->debugDumpLinkBucket();
    }
    fprintf(stderr,"\n");
  }

}


// **** AST methods that I didn't have anywhere else to put

Variable_O *E_compoundLit::getVarForRval() {
  // FIX: maybe this variable should have flag DF_MEMBER
  // NOTE: I don't think this is one of those variables to put into builtinVars
  if (!cpdInitVarForRval) {
    xassert(abstrValue);
    cpdInitVarForRval = asVariable_O
      (tFac->makeVariable(abstrValue->loc, "<phantom E_compoundLit::cpdInitVarForRval>",
                          abstrValue->asRval()->t(), DF_NONE));
    cpdInitVarForRval->setReal(true);
    asVariable_O(cpdInitVarForRval)->setFilteredKeep(true);
    // NOTE: it is very important that we re-use the same abstrValue
    asVariable_O(cpdInitVarForRval)->setAbstrValue(abstrValue->asRval());
  }
  return asVariable_O(cpdInitVarForRval);
}
