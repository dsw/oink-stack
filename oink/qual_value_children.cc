// see License.txt for copyright and terms of use

#include "qual_value_children.h" // this module
#include "qual_var.h"           // Variable_Q
#include "value.h"
#include "qual_annot.h"
#include "qual_global.h"
#include "oink_util.h"


// "Value_Q" methods ****************

// * qvars and qual annots

// static bool valueContainsQualAnnot(Value const *v) {
//   return hasQualAnnot(v);
// }

// bool treeContainsQualAnnots_Value_Q(Value *v) {
//   return v->anyCtorSatisfiesF(valueContainsQualAnnot);
// }

static bool containsQvars(Value const *v) {
  // if (!hasQualAnnot(v)) return false;
  return qa(v)->hasQv();
}
bool treeContainsQvars_Value_Q(Value const *v) {
  return v->anyCtorSatisfiesF(containsQvars);
}

static bool containsExternQvars(Value const *v) {
  // if (!hasQualAnnot(v)) return false;
  return qa(v)->qvarIsExtern();
}
bool treeContainsExternQvars(Value const *v) {
  return v->anyCtorSatisfiesF(containsExternQvars);
}

// * qliterals

static bool containsQliterals(Value const *v) {
  return v->t()->ql != NULL;
}
bool treeContainsQliterals(Value *v) {
  return v->anyCtorSatisfiesF(containsQliterals);
}

static bool containsFunkyQliterals(Value const *v) {
  return hasFunkyQualifiers(v->t()->ql, SL_UNKNOWN);
}
bool treeContainsFunkyQliterals(Value *v) {
  return v->anyCtorSatisfiesF(containsFunkyQliterals);
}

static bool containsUserQliterals(Value const *v) {
  return hasNonFunkyQualifiers(v->t()->ql, SL_UNKNOWN);
}
bool treeContainsUserQliterals(Value *v) {
  return v->anyCtorSatisfiesF(containsUserQliterals);
}

static bool ensureQvIfUserQliterals(Value const *v) {
  if (hasNonFunkyQualifiers(v->t()->ql, SL_UNKNOWN)) {
    qa(v)->getQVar();
  }
  return false;                 // true would prune the walk
}
void treeEnsureQvIfUserQliterals(Value *v) {
  v->anyCtorSatisfiesF(ensureQvIfUserQliterals);
}


// TypeFactory_Q ****************

Variable_O *TypeFactory_Q::new_Variable
  (SourceLoc L, StringRef n, Type *t, DeclFlags f)
{
  return new Variable_Q(L, n, t, f);
}

void TypeFactory_Q::applyCVToType_extra
  (SourceLoc loc, Type *baseType, Type *newBaseType, TypeSpecifier *syntax) {
  if (syntax) {
    // baseType should not have any funky qualifiers
    if (hasFunkyQualifiers(baseType->ql, syntax->loc)) {
      // FIX: should this call report_qerror?
      userFatalError(loc, "applyCVToType: baseType has funky qualifiers");
    }
    // FIX: I think we can just simplify it thus.
    // UPDATE: is that so? then do it.
//      if (!has_funky_qualifiers(syntax->ql, syntax->loc) &&
//          is_subset_of_qualifiers(syntax->ql, newBaseType->ql, syntax->loc)) {
//        return asType(newBaseType); // done
//      }
    // get any qualifiers that were typedef-ed onto baseType
    prependQLiterals(newBaseType->ql, deepClone(baseType->ql));
    // get any qualifiers on the syntax
    prependQLiterals(newBaseType->ql, deepClone(syntax->ql));
  }
}

void TypeFactory_Q::syntaxPointerType_extra(D_pointer *syntax, PointerType *ptq) {
  if (syntax) {
    xassert(!ptq->ql);
    ptq->ql = deepClone(syntax->ql);
  }
}

void TypeFactory_Q::makeTypeOf_receiver_extra(D_func *syntax, CVAtomicType *tmpcvat) {
  // sm: here, we're transferring qualifier literals attached to the
  // function declarator over to the 'this' parameter, which should
  // supercede what was being done with qualifiers in
  // syntaxFunctionType above, given that now the 'this' parameter is
  // part of the function type
  if (syntax) {
    xassert(!tmpcvat->ql);
    tmpcvat->ql = deepClone(syntax->ql);
  }
}

void TypeFactory_Q::makeSimilarFunctionType_extra(FunctionType *similar, FunctionType *ft) {
  xassert(!ft->ql);
  ft->ql = deepClone(similar->ql);
}

void TypeFactory_Q::finishCloningType(Type *tgt, Type *src) {
  xassert(!tgt->ql);
  tgt->ql = deepClone(src->ql);
}

// ValueFactory_Q ****************

// template <class T>
// static inline T *allocWithAnnot()
// {
//   return allocator.alloc<Struct<T,T::MyQualAnnot> >()->t1;
// }

#define DEFINE_ALLOC_VALUE_Q(CLS)                                 \
  CLS *ValueFactory_Q::alloc_##CLS() {                            \
    return &allocator.alloc<CLS##_Q::MyStruct>()->t1;             \
  }

#define DEFINE_GET_ANNOTATION_Q(CLSQ)                             \
  CLSQ::MyQualAnnot *CLSQ::getAnnotation() {                      \
    return &CLSQ::MyStruct::getStructFrom1(this)->t2;             \
  }

#define DEFINE_GETWHOLESIZE(CLSQ)                                 \
  size_t CLSQ::getWholeSize() {                                   \
    return sizeof(MyStruct);                                      \
  }

FOREACH_CLASS_Value(DEFINE_ALLOC_VALUE_Q)

FOREACH_CLASS_Value_Q(DEFINE_GET_ANNOTATION_Q)

FOREACH_CLASS_Value_Q(DEFINE_GETWHOLESIZE)

#undef DEFINE_ALLOC_VALUE
#undef DEFINE_GET_ANNOTATION_Q
#undef DEFINE_GETWHOLESIZE

// ****

// one-off for CVAtomicValue fixing the name after ctoring the 'void'
// that an int 'points to'.
CVAtomicValue *ValueFactory_Q::makeAtVoid(CVAtomicValue const *t) const {
  // delegate to superclass
  CVAtomicValue *ret = ValueFactory::makeAtVoid(t);

#if DELAY_SET_NAMES
  /// TODO XXX FIXME
#else
  if (qualCmd->name_expressions) {
    stringBuilder name0;
    name0 << "(" << qa(t)->getName() << " atVoid)";
    qa(ret)->nameTree(name0);
  }
#endif

  return ret;
}

// one-off for CVAtomicValue fixing the name after cloning a member
void ValueFactory_Q::finishJustClonedMember
  (CVAtomicValue *t, string memberName, Value *memberValue) {
  ValueFactory::finishJustClonedMember(t, memberName, memberValue);

#if DELAY_SET_NAMES
  /// TODO XXX FIXME
#else
  if (qualCmd->name_expressions) {
    // name the qualifier on the cloned value
    stringBuilder sb;
    sb << qa(t)->getName();
    sb << ".";
    sb << memberName;
//    cout << "name of just-cloned member: " << sb << endl;

    // NOTE: don't need a strdup here because nameTree is going to dup
    // whatever we hand to it in the stringBuilder ctor.
    qa(memberValue)->nameTree(sb);
  }
#endif
}
