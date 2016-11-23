// mtype.cc
// code for mtype.h

// 2005-08-03: This code is now exercised to 100% statement coverage,
// except for the code that deals with MF_POLYMORPHIC and
// MF_ISOMORPHIC and the places annotated with gcov-ignore or
// gcov-begin/end-ignore, by in/t0511.cc.
//
// My plan is to re-do the coverage analysis using the entire test
// suite once mtype is hooked up to the rest of the program as the
// module to use for type comparison and matching.

#include "mtype.h"       // this module
#include "trace.h"       // tracingSys
#include "cc_env.h"      // Env::applyArgumentMap


string toString(MatchFlags flags)
{
  static char const * const map[] = {
    "MF_IGNORE_RETURN",
    "MF_STAT_EQ_NONSTAT",
    "MF_IGNORE_IMPLICIT",
    "MF_RESPECT_PARAM_CV",
    "MF_IGNORE_TOP_CV",
    "MF_COMPARE_EXN_SPEC",
    "MF_SIMILAR",
    "MF_POLYMORPHIC",
    "MF_DEDUCTION",
    "MF_UNASSOC_TPARAMS",
    "MF_IGNORE_ELT_CV",
    "MF_MATCH",
    "MF_NO_NEW_BINDINGS",
    "MF_ISOMORPHIC",
    "MF_IGNORE_FUNC_CV",
  };
  STATIC_ASSERT(TABLESIZE(map) == MF_NUM_FLAGS);
  return bitmapString(flags, map, MF_NUM_FLAGS, " ");
}


//-------------------------- Binding -------------------------
string toString_or_CV_NONE(CVFlags cv)
{
  if (cv == CV_NONE) {
    return "CV_NONE";
  }
  else {
    return toString(cv);
  }
}


bool IMType::Binding::operator== (Binding const &obj) const
{
  if (sarg.kind != obj.sarg.kind) {
    return false;
  }
  
  if (sarg.isType()) {
    // somewhat complicated case: we need to make sure the types are
    // the same, *ignoring* their cv-flags, and that the cv-flags in
    // the Binding objects themselves *are* equal
    return getType()->equals(obj.getType(), MF_IGNORE_TOP_CV) &&
           cv == obj.cv;
  }
  else {
    // easy, just STemplateArgument equality
    return sarg.equals(obj.sarg);
  }
}


string IMType::Binding::asString() const
{
  if (sarg.isType()) {
    return stringc << getType()->toString() << " with "
                   << toString_or_CV_NONE(cv);
  }
  else {
    return sarg.toString();
  }
}


// ------------------------- IMType -----------------------------
IMType::IMType()
  : bindings(),
    env(NULL),
    failedDueToDQT(false)
{}

IMType::~IMType()
{}


// --------------- IMType: AtomicType and subclasses ------------
STATICDEF bool IMType::canUseAsVariable(Variable *var, MatchFlags flags)
{
  xassert(var);

  if (!( flags & MF_MATCH )) {
    // we're not using anything as a variable
    return false;
  }

  if ((flags & MF_UNASSOC_TPARAMS) &&
      var->getParameterizedEntity() != NULL) {
    // we cannot use 'var' as a variable because, even though it is a
    // template parameter, it is associated with a specific template
    // and MF_UNASSOC_TPARAMS means we can only use *unassociated*
    // variables as such
    return false;
  }

  // no problem
  return true;
}


bool IMType::imatchAtomicType(AtomicType const *conc, AtomicType const *pat, MatchFlags flags)
{
  if (conc == pat) {
    return true;
  }

  if (pat->isTypeVariable() &&
      canUseAsVariable(pat->asTypeVariableC()->typedefVar, flags)) {
    return imatchAtomicTypeWithVariable(conc, pat->asTypeVariableC(), flags);
  }

  // Bind a TTP to a real template.
  //
  // Sort of a mix of the cases above and below.
  if (pat->isTemplateTypeVariable()) {
    TemplateTypeVariable const *ttv = pat->asTemplateTypeVariableC();
    if (canUseAsVariable(ttv->typedefVar, flags) &&
        conc->isCompoundType()) {
      // Laundering constness... below, I do the same thing more
      // sneakily by using getPrimary().  I don't think it is a
      // problem, but I just don't recall what the overall strategy
      // for constness was in here.
      CompoundType *concCT = const_cast<CompoundType*>(conc->asCompoundTypeC());

      TemplateInfo *concTI = concCT->typedefVar->templateInfo();
      if (concTI && concTI->isPrimary()) {
        return imatchAtomicTypeWithTTPVariable(concCT, ttv, flags);
      }
    }
  }

  if (conc->getTag() != pat->getTag()) {
    // match an instantiation with a PseudoInstantiation
    if ((flags & MF_MATCH) &&
        pat->isPseudoInstantiation() &&
        conc->isCompoundType() &&
        conc->asCompoundTypeC()->typedefVar->templateInfo() &&
        conc->asCompoundTypeC()->typedefVar->templateInfo()->isCompleteSpecOrInstantiation()) {
      TemplateInfo *concTI = conc->asCompoundTypeC()->typedefVar->templateInfo();
      PseudoInstantiation const *patPI = pat->asPseudoInstantiationC();

      // compare the template part of the PI
      if (patPI->primary->isTemplateTypeVariable()) {
        // match the TTP against the concrete type's template primary
        NamedAtomicType *concPrimary = concTI->getPrimary()->var->type->asNamedAtomicType();
        TemplateTypeVariable *ttv = patPI->primary->asTemplateTypeVariable();
        if (!imatchAtomicTypeWithTTPVariable(concPrimary, ttv, flags)) {
          return false;
        }
      }
      else {
        CompoundType *patPIprimaryCT = patPI->primary->asCompoundType();

        // these must be instantiations of the same primary
        if (concTI->getPrimary() != patPIprimaryCT->templateInfo()->getPrimary()) {
          return false;
        }
      }

      // compare arguments; use the args to the primary, not the args to
      // the partial spec (if any)
      return imatchSTemplateArguments(concTI->getArgumentsToPrimary(),
                                      patPI->args, flags);
    }

    return false;
  }

  // for the template-related types, we have some structural equality
  switch (conc->getTag()) {
    default: xfailure("bad tag");

    case AtomicType::T_SIMPLE:
    case AtomicType::T_COMPOUND:
    case AtomicType::T_ENUM:
      // non-template-related type, physical equality only
      return false;

    #define CASE(tag,type) \
      case AtomicType::tag: return imatch##type(conc->as##type##C(), pat->as##type##C(), flags) /*user;*/
    CASE(T_TYPEVAR, TypeVariable);
    CASE(T_PSEUDOINSTANTIATION, PseudoInstantiation);
    CASE(T_DEPENDENTQTYPE, DependentQType);
    #undef CASE
  }
}


bool IMType::imatchTypeVariable(TypeVariable const *conc, TypeVariable const *pat, MatchFlags flags)
{
  // 2005-03-03: Let's try saying that TypeVariables are equal if they
  // are the same ordinal parameter of the same template.
  Variable *cparam = conc->typedefVar;
  Variable *pparam = pat->typedefVar;

  return cparam->sameTemplateParameter(pparam);
}


bool IMType::imatchPseudoInstantiation(PseudoInstantiation const *conc,
                                       PseudoInstantiation const *pat, MatchFlags flags)
{
  if (conc->primary != pat->primary) {
    return false;
  }

  return imatchSTemplateArguments(conc->args, pat->args, flags);
}


bool IMType::imatchSTemplateArguments(ObjList<STemplateArgument> const &conc,
                                      ObjList<STemplateArgument> const &pat,
                                      MatchFlags flags)
{
  ObjListIter<STemplateArgument> concIter(conc);
  ObjListIter<STemplateArgument> patIter(pat);

  while (!concIter.isDone() && !patIter.isDone()) {
    STemplateArgument const *csta = concIter.data();
    STemplateArgument const *psta = patIter.data();
    if (!imatchSTemplateArgument(csta, psta, flags)) {
      return false;
    }

    concIter.adv();
    patIter.adv();
  }

  return concIter.isDone() && patIter.isDone();
}


bool IMType::imatchSTemplateArgument(STemplateArgument const *conc,
                                     STemplateArgument const *pat, MatchFlags flags)
{
  if (pat->kind == STemplateArgument::STA_DEPEXPR &&
      pat->getDepExpr()->isE_variable() &&
      canUseAsVariable(pat->getDepExpr()->asE_variableC()->var, flags)) {
    return imatchNontypeWithVariable(conc, pat->getDepExpr()->asE_variableC(), flags);
  }

  // dmandelin@mozilla.com fix for Elsa ticket #27
  // tested by t0580.cc
  // C++ Draft Standard 14.1.3 -- enums and ints
  if (pat->kind == STemplateArgument::STA_ENUMERATOR) {
    if (conc->kind == STemplateArgument::STA_INT)
      return conc->getInt() == pat->getEnumerator()->getEnumeratorValue();
    if (conc->kind == STemplateArgument::STA_ENUMERATOR)
      return conc->getEnumerator()->getEnumeratorValue() == pat->getEnumerator()->getEnumeratorValue();
    return false;
  }
  if (pat->kind == STemplateArgument::STA_INT) {
    if (conc->kind == STemplateArgument::STA_INT)
      return pat->getInt() == conc->getInt();
    if (conc->kind == STemplateArgument::STA_ENUMERATOR)
      return conc->getEnumerator()->getEnumeratorValue() == pat->getInt();
    return false;
  }

  if (conc->kind != pat->kind) {
    return false;
  }

  switch (conc->kind) {
    default: 
      xfailure("bad or unimplemented STemplateArgument kind");

    case STemplateArgument::STA_TYPE: {
      // For convenience I have passed 'STemplateArgument' directly,
      // but this module's usage of that structure is supposed to be
      // consistent with it storing 'Type const *' instead of 'Type
      // *', so this extraction is elaborated to make it clear we are
      // pulling out const pointers.
      Type const *ct = conc->getType();
      Type const *pt = pat->getType();
      return imatchType(ct, pt, flags);
    }

    case STemplateArgument::STA_REFERENCE:
      return conc->getReference() == pat->getReference();

    case STemplateArgument::STA_POINTER:
      return conc->getPointer() == pat->getPointer();

    case STemplateArgument::STA_MEMBER:
      return conc->getMember() == pat->getMember();

    case STemplateArgument::STA_DEPEXPR:
      return imatchExpression(conc->getDepExpr(), pat->getDepExpr(), flags);
      
    case STemplateArgument::STA_TEMPLATE:
      return imatchAtomicType(conc->getTemplate(), pat->getTemplate(), flags);
  }
}


bool IMType::imatchNontypeWithVariable(STemplateArgument const *conc,
                                       E_variable const *pat, MatchFlags flags)
{
  // 'conc' should be a nontype argument
  if (!conc->isObject()) {
    return false;
  }

  if (flags & MF_ISOMORPHIC) {
    // check that we are only binding to a variable; ideally, this
    // would only be an STA_DEPEXPR of an E_variable, but infelicities
    // in other parts of the template-handling code can let this be
    // an STA_REFERENCE of a Variable that is a template parameter..
    // probably, that will be fixed at some point and the STA_REFERENCE
    // possibility can be eliminated
    if (conc->isDepExpr() &&
        conc->getDepExpr()->isE_variable() &&
        conc->getDepExpr()->asE_variableC()->var->isTemplateParam()) {
      // ok
    }
    else if (conc->kind == STemplateArgument::STA_REFERENCE &&
             conc->getReference()->isTemplateParam()) {
      // ok, sort of
    }
    else {
      // not compatible with MF_ISOMORPHIC
      return false;
    }
  }

  StringRef vName = pat->name->getName();
  Binding *binding = bindings.get(vName);
  if (binding) {
    // check that 'conc' and 'binding->sarg' are equal
    return imatchSTemplateArgument(conc, &(binding->sarg), flags & ~MF_MATCH);
  }
  else {
    if (flags & MF_NO_NEW_BINDINGS) {
      return false;
    }

    // bind 'pat->name' to 'conc'
    binding = new Binding;
    binding->sarg = *conc;
    return addBinding(vName, binding, flags);
  }
}


bool IMType::addBinding(StringRef name, Binding * /*owner*/ value, MatchFlags flags)
{
  if (flags & MF_ISOMORPHIC) {
    // is anything already bound to 'value'?
    for (BindingMap::IterC iter(bindings); !iter.isDone(); iter.adv()) {
      if (value->operator==(*(iter.value()))) {
        // yes, something is already bound to it, which we cannot
        // allow in MF_ISOMORPHIC mode
        delete value;
        return false;
      }
    }
  }

  bindings.add(name, value);
  return true;
}


bool IMType::imatchDependentQType(DependentQType const *conc,
                                  DependentQType const *pat, MatchFlags flags)
{ 
  // compare first components
  if (!imatchAtomicType(conc->first, pat->first, flags)) {
    return false;
  }

  // compare sequences of names as just that, names, since their
  // interpretation depends on what type 'first' ends up being;
  // I think this behavior is underspecified in the standard, but
  // it is what gcc and icc seem to do
  return imatchPQName(conc->rest, pat->rest, flags);
}

bool IMType::imatchPQName(PQName const *conc, PQName const *pat, MatchFlags flags)
{
  if (conc->kind() != pat->kind()) {
    return false;
  }

  ASTSWITCH2C(PQName, conc, pat) {
    // recursive case
    ASTCASE2C(PQ_qualifier, cq, pq) {
      // compare as names (i.e., syntactically)
      if (cq->qualifier != pq->qualifier) {
        return false;
      }

      // the template arguments can be compared semantically because
      // the standard does specify that they are looked up in a
      // context that is effectively independent of what appears to
      // the left in the qualified name (cppstd 3.4.3.1p1b3)
      if (!imatchSTemplateArguments(cq->sargs, pq->sargs, flags)) {
        return false;
      }

      // continue down the chain
      return imatchPQName(cq->rest, pq->rest, flags);
    }

    // base cases
    ASTNEXT2C(PQ_name, cn, pn) {
      // compare as names
      return cn->name == pn->name;
    }
    ASTNEXT2C(PQ_operator, co, po) {
      return co->o == po->o;      // gcov-ignore: can only match on types, and operators are not types
    }
    ASTNEXT2C(PQ_template, ct, pt) {
      // like for PQ_qualifier, except there is no 'rest'
      return ct->name == pt->name &&
             imatchSTemplateArguments(ct->sargs, pt->sargs, flags);
    }

    ASTDEFAULT2C {
      xfailure("bad kind");
    }
    ASTENDCASE2C
  }
}


// -------------- IMType: Type and subclasses -------------
bool IMType::imatchType(Type const *conc, Type const *pat, MatchFlags flags)
{
  if (pat->isReferenceType() &&         // if comparing reference
      !conc->isReferenceType() &&       // to non-reference
      (flags & MF_IGNORE_TOP_CV) &&     // at top of type
      (flags & MF_DEDUCTION)) {         // during type deduction
    // (in/t0549.cc) we got here because we just used a binding to
    // come up with a reference type; without that, we would have stripped
    // the reference-ness much earlier; but here we are, so strip it
    pat = pat->asRvalC();
  }

  if (pat->isTypeVariable() &&
      canUseAsVariable(pat->asTypeVariableC()->typedefVar, flags)) {
    return imatchTypeWithVariable(conc, pat->asTypeVariableC(),
                                  pat->getCVFlags(), flags);
  }

  if (pat->isTemplateTypeVariable() &&
      canUseAsVariable(pat->asTemplateTypeVariableC()->typedefVar, flags)) {
    return imatchTypeWithTTPVariable(conc, pat->asTemplateTypeVariableC(),
                                     pat->getCVFlags(), flags);
  }
                  
  // If the pattern is a DQT, resolve it.
  //
  // in/t0621.cc: Except, if concrete is also DQT, then don't.  In
  // such cases, it should be the case that the concrete type is a DQT
  // because it *cannot* be resolved, in which case the only option is
  // to match syntactically.  (Is this right?  Do I really resolve
  // DQTs in 'concrete'?)
  if ((flags & MF_MATCH) &&
      !(flags & MF_ISOMORPHIC) &&
      pat->isDependentQType() &&
      !conc->isDependentQType()) {
    // must resolve a DQT to use it
    return imatchTypeWithResolvedType(conc, pat, flags);
  }

  if (flags & MF_POLYMORPHIC) {
    if (pat->isSimpleType()) {
      SimpleTypeId objId = pat->asSimpleTypeC()->type;
      if (ST_PROMOTED_INTEGRAL <= objId && objId <= ST_ANY_TYPE) {
        return imatchTypeWithPolymorphic(conc, objId, flags);
      }
    }
  }

  if ((flags & MF_MATCH) &&
      conc->isArrayType() &&
      pat->isDependentSizedArrayType()) {
    ArrayType const *concAT = conc->asArrayTypeC();
    DependentSizedArrayType const *patAT = pat->asDependentSizedArrayTypeC();
                    
    // element types first
    if (!imatchType(concAT->eltType, patAT->eltType, flags)) {
      return false;
    }
    
    // sizes
    return imatchArraySizeWithExpression(concAT->size, patAT->sizeExpr, flags);
  }

  // further comparison requires that the types have equal tags
  Type::Tag tag = conc->getTag();
  if (pat->getTag() != tag) {
    return false;
  }

  switch (tag) {
    default: xfailure("bad type tag");

    #define CASE(tagName,typeName) \
      case Type::tagName: return imatch##typeName(conc->as##typeName##C(), pat->as##typeName##C(), flags) /*user;*/
    CASE(T_ATOMIC, CVAtomicType);
    CASE(T_POINTER, PointerType);
    CASE(T_REFERENCE, ReferenceType);
    CASE(T_FUNCTION, FunctionType);
    CASE(T_ARRAY, ArrayType);
    CASE(T_POINTERTOMEMBER, PointerToMemberType);
    CASE(T_DEPENDENTSIZEDARRAY, DependentSizedArrayType);
    #undef CASE
  }
}


bool IMType::imatchTypeWithVariable(Type const *conc, TypeVariable const *pat,
                                    CVFlags tvCV, MatchFlags flags)
{
  if ((flags & MF_ISOMORPHIC) &&
      !conc->isTypeVariable()) {
    return false;      // MF_ISOMORPHIC requires that we only bind to variables
  }

  StringRef tvName = pat->name;
  Binding *binding = bindings.get(tvName);
  if (binding) {
    // 'tvName' is already bound; compare its current
    // value (as modified by 'tvCV') against 'conc'
    return equalWithAppliedCV(conc, binding, tvCV, flags);
  }
  else {
    if (flags & MF_NO_NEW_BINDINGS) {
      // new bindings are disallowed, so unbound variables in 'pat'
      // cause failure
      return false;
    }

    // bind 'tvName' to 'conc'; 'conc' should have cv-flags
    // that are a superset of those in 'tvCV', and the
    // type to which 'tvName' is bound should have the cv-flags
    // in the difference between 'conc' and 'tvCV'
    return addTypeBindingWithoutCV(tvName, conc, tvCV, flags);
  }
}


// Like 'imatchTypeWithVariable', but where the type variable is
// a template template parameter (TTP).
//
// TODO: If this function can be essentially identical to the above,
// then collapse them into one.
bool IMType::imatchTypeWithTTPVariable(Type const *conc,
                                       TemplateTypeVariable const *pat,
                                       CVFlags tvCV, MatchFlags flags)
{
  if ((flags & MF_ISOMORPHIC) &&
      !conc->isTemplateTypeVariable()) {
    return false;
  }

  StringRef tvName = pat->name;
  Binding *binding = bindings.get(tvName);
  if (binding) {
    return equalWithAppliedCV(conc, binding, tvCV, flags);
  }
  else {
    if (flags & MF_NO_NEW_BINDINGS) {
      return false;
    }

    return addTypeBindingWithoutCV(tvName, conc, tvCV, flags);
  }
}


bool IMType::imatchTypeWithResolvedType(Type const *conc, Type const *pat,
                                        MatchFlags flags)
{
  // I need an Env here so I can call applyArgumentMap.
  if (!env) {
    // in/t0617.cc:  HACK:  I don't have an Env, and it's not going to
    // be easy to get ahold of one in the context where I need it, but
    // I can tell that DQT resolution will fail b/c of a special case.
    if (pat->isDependentQType()) {
      NamedAtomicType *qualifier = 
        pat->asCVAtomicTypeC()->atomic->asDependentQTypeC()->first;
      StringRef name = qualifier->asNamedAtomicTypeC()->name;
      if (bindings.get(name) == NULL) {
        // DQT resolution could not possibly succeed, because we
        // don't have a binding for this qualifier
        failedDueToDQT = true;
        return false;
      }
    }

    // If this assertion fails, it may be sufficient to use the
    // MType constructor that accepts an Env.
    xfailure("in MF_MATCH mode, need to resolve a DQT, so need an Env");
  }

  // this cast is justified by the private constructors of IMType
  MType &mtype = static_cast<MType&>(*this);

  // this cast is justified by the fact that applyArgumentMap is
  // careful not to modify its argument; it accepts a non-const ptr
  // only so that it can return it as non-const too (but I then
  // store the result in the const 'resolvedType', so there isn't
  // a loss of soundness there either)
  Type *patNC = const_cast<Type*>(pat);

  // The following call might need to query the bindings, and there
  // is considerable difficulty involved in pushing a soundness
  // argument through in the face of queried bindings.  So, I am
  // forced to correlate the capacity to resolve DQTs with supporting
  // the const interface.  That is why I have now removed setEnv().
  xassert(mtype.getAllowNonConst());

  Type const *resolvedType = env->applyArgumentMapToType_helper(mtype, patNC);
  if (resolvedType) {
    // take the resolved type and compare it directly to 'conc'
    return imatchType(conc, resolvedType, flags & ~MF_MATCH);
  }
  else {
    // failure to resolve, hence failure to match
    failedDueToDQT = true;
    return false;
  }
}


// Typical scenario:
//   conc is 'int const'
//   binding is 'int'
//   cv is 'const'
// We want to compare the type denoted by 'conc' with the type denoted
// by 'binding' with the qualifiers in 'cv' added to it at toplevel.
bool IMType::equalWithAppliedCV(Type const *conc, Binding *binding, CVFlags cv, MatchFlags flags)
{
  // turn off type variable binding/substitution
  flags &= ~MF_MATCH;

  if (binding->sarg.isType()) {
    Type const *t = binding->getType();

    // cv-flags for 't' are ignored, replaced by the cv-flags stored
    // in the 'binding' object
    cv |= binding->cv;

    return imatchTypeWithSpecifiedCV(conc, t, cv, flags);
  }

  if (binding->sarg.isAtomicType()) {
    if (!conc->isCVAtomicType()) {
      return false;
    }

    // compare the atomics
    if (!imatchAtomicType(conc->asCVAtomicTypeC()->atomic,
                          binding->sarg.getAtomicType(), flags)) {
      return false;
    }

    // When a name is bound directly to an atomic type, it is compatible
    // with any binding to a CVAtomicType for the same atomic; that is,
    // it is compatible with any cv-qualified variant.  So, if we
    // are paying attention to cv-flags at all, simply replace the
    // original (AtomicType) binding with 'conc' (a CVAtomicType) and
    // appropriate cv-flags.
    if (!( flags & MF_OK_DIFFERENT_CV )) {
      // The 'cv' flags supplied are subtracted from those in 'conc'.
      CVFlags concCV = conc->getCVFlags();
      if (superset(concCV, cv)) {
        // example:
        //   conc = 'struct B volatile const'
        //   binding = 'struct B'
        //   cv = 'const'
        // Since 'const' was supplied (e.g., "T const") with the type
        // variable, we want to remove it from what we bind to, so here
        // we will bind to 'struct B volatile' ('concCV' = 'volatile').
        concCV = (concCV & ~cv);
      }
      else {
        // example:
        //   conc = 'struct B volatile'
        //   binding = 'struct B'
        //   cv = 'const'
        // This means we are effectively comparing 'struct B volatile' to
        // 'struct B volatile const' (the latter volatile arising because
        // being directly bound to an atomic means we can add qualifiers),
        // and these are not equal.
        return false;
      }

      binding->setType(conc);
      binding->cv = concCV;
      return true;
    }

  }

  // I *think* that the language rules that prevent same-named
  // template params from nesting will have the effect of preventing
  // this code from being reached, but if it turns out I am wrong, it
  // should be safe to simply remove the assertion and return false.
  xfailure("attempt to match a type with a variable bound to a non-type");

  return false;      // gcov-ignore
}

bool IMType::imatchTypeWithSpecifiedCV(Type const *conc, Type const *pat, CVFlags cv, MatchFlags flags)
{
  // compare underlying types, ignoring first level of cv
  return imatchCVFlags(conc->getCVFlags(), cv, flags) &&
         imatchType(conc, pat, flags | MF_IGNORE_TOP_CV);
}


bool IMType::addTypeBindingWithoutCV(StringRef tvName, Type const *conc,
                                     CVFlags tvcv, MatchFlags flags)
{
  CVFlags ccv = conc->getCVFlags();

  if ((flags & MF_DEDUCTION) && (flags & MF_IGNORE_TOP_CV)) {
    // trying to implement 14.8.2.1p2b3
    ccv = CV_NONE;
  }

  if (tvcv & ~ccv) {
    // the type variable was something like 'T const' but the concrete
    // type does not have all of the cv-flags (e.g., just 'int', no
    // 'const'); this means the match is a failure
    if (flags & MF_DEDUCTION) {
      // let it go anyway, as part of my poor approximationg of 14.8.2.1
    }
    else {
      return false;
    }
  }

  // 'tvName' will be bound to 'conc', except we will ignore the
  // latter's cv flags
  Binding *binding = new Binding;
  binding->setType(conc);

  // instead, compute the set of flags that are on 'conc' but not
  // 'tvcv'; this will be the cv-flags of the type to which 'tvName'
  // is bound
  binding->cv = (ccv & ~tvcv);

  // add the binding
  return addBinding(tvName, binding, flags);
}


// check if 'conc' matches the "polymorphic" type family 'polyId'
bool IMType::imatchTypeWithPolymorphic(Type const *conc, SimpleTypeId polyId,
                                       MatchFlags flags)
{
  // check those that can match any type constructor
  if (polyId == ST_ANY_TYPE) {
    return true;
  }

  if (polyId == ST_ANY_NON_VOID) {
    return !conc->isVoid();
  }

  if (polyId == ST_ANY_OBJ_TYPE) {
    return !conc->isFunctionType() &&
           !conc->isVoid();
  }

  // check those that only match atomics
  if (conc->isSimpleType()) {
    SimpleTypeId concId = conc->asSimpleTypeC()->type;
    SimpleTypeFlags concFlags = simpleTypeInfo(concId).flags;

    // see cppstd 13.6 para 2
    if (polyId == ST_PROMOTED_INTEGRAL) {
      return (concFlags & (STF_INTEGER | STF_PROM)) == (STF_INTEGER | STF_PROM);
    }

    if (polyId == ST_PROMOTED_ARITHMETIC) {
      return (concFlags & (STF_INTEGER | STF_PROM)) == (STF_INTEGER | STF_PROM) ||
             (concFlags & STF_FLOAT);      // need not be promoted!
    }

    if (polyId == ST_INTEGRAL) {
      return (concFlags & STF_INTEGER) != 0;
    }

    if (polyId == ST_ARITHMETIC) {
      return (concFlags & (STF_INTEGER | STF_FLOAT)) != 0;
    }

    if (polyId == ST_ARITHMETIC_NON_BOOL) {
      return concId != ST_BOOL &&
             (concFlags & (STF_INTEGER | STF_FLOAT)) != 0;
    }
  }

  // polymorphic type failed to match
  return false;
}


bool IMType::imatchAtomicTypeWithVariable(AtomicType const *conc,
                                          TypeVariable const *pat,
                                          MatchFlags flags)
{
  if ((flags & MF_ISOMORPHIC) &&
      !conc->isTypeVariable()) {
    return false;      // MF_ISOMORPHIC requires that we only bind to variables
  }

  StringRef tvName = pat->name;
  Binding *binding = bindings.get(tvName);
  if (binding) {
    // 'tvName' is already bound; it should be bound to the same
    // atomic as 'conc', possibly with some (extra, ignored) cv flags
    if (binding->sarg.isType()) {
      Type const *t = binding->getType();
      if (t->isCVAtomicType()) {
        return imatchAtomicType(conc, t->asCVAtomicTypeC()->atomic, flags & ~MF_MATCH);
      }
      else {
        return false;
      }
    }
    else if (binding->sarg.isAtomicType()) {
      return imatchAtomicType(conc, binding->sarg.getAtomicType(), flags & ~MF_MATCH);
    }
    else {              // gcov-ignore: cannot be triggered, the error is
      return false;     // gcov-ignore: diagnosed before mtype is entered 
    }
  }
  else {
    if (flags & MF_NO_NEW_BINDINGS) {
      // new bindings are disallowed, so unbound variables in 'pat'
      // cause failure
      return false;
    }

    // bind 'tvName' to 'conc'
    binding = new Binding;
    binding->sarg.setAtomicType(conc);
    return addBinding(tvName, binding, flags);
  }
}


// Copy+paste from 'imatchAtomicTypeWithVariable'.
//
// TODO: If this works, collapse them.
//
// Hmmm... what about constness of 'conc'?  It is const in the
// other version, but does not seem to need to be here.
bool IMType::imatchAtomicTypeWithTTPVariable(NamedAtomicType *conc,
                                             TemplateTypeVariable const *pat,
                                             MatchFlags flags)
{
  if ((flags & MF_ISOMORPHIC) &&
      !conc->isTemplateTypeVariable()) {
    return false;
  }

  StringRef tvName = pat->name;
  Binding *binding = bindings.get(tvName);
  if (binding) {
    if (binding->sarg.isTemplate()) {
      // This part is slightly simpler than above b/c it is not
      // possible to attach CV to an uninstantiated template.
      NamedAtomicType *nat = binding->sarg.getTemplate();
      return imatchAtomicType(conc, nat, flags & ~MF_MATCH);
    }
    else if (binding->sarg.isAtomicType()) {
      return imatchAtomicType(conc, binding->sarg.getAtomicType(), flags & ~MF_MATCH);
    }
    else {
      return false;
    }
  }
  else {
    if (flags & MF_NO_NEW_BINDINGS) {
      return false;
    }

    binding = new Binding;
    binding->sarg.setTemplate(conc);
    return addBinding(tvName, binding, flags);
  }
}



bool IMType::imatchCVAtomicType(CVAtomicType const *conc, CVAtomicType const *pat,
                                MatchFlags flags)
{
  // compare cv-flags
  return imatchCVFlags(conc->cv, pat->cv, flags) &&
         imatchAtomicType(conc->atomic, pat->atomic, flags & MF_PROP);
}

  
bool IMType::imatchCVFlags(CVFlags conc, CVFlags pat, MatchFlags flags)
{
  if (flags & MF_OK_DIFFERENT_CV) {
    // anything goes
    return true;
  }
  else if (flags & MF_DEDUCTION) {
    // merely require that the pattern flags be a superset of
    // the concrete flags (in/t0315.cc, in/t0348.cc)
    //
    // TODO: this is wrong, as it does not correctly implement
    // 14.8.2.1; I am only implementing this because it emulates
    // what matchtype does right now
    if (superset(pat, conc)) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    // require equality
    return conc == pat;
  }
}


bool IMType::imatchPointerType(PointerType const *conc, PointerType const *pat, MatchFlags flags)
{
  // note how MF_IGNORE_TOP_CV allows *this* type's cv flags to differ,
  // but it's immediately suppressed once we go one level down; this
  // behavior is repeated in all 'match' methods

  return imatchCVFlags(conc->cv, pat->cv, flags) &&
         imatchType(conc->atType, pat->atType, flags & MF_PTR_PROP);
}


bool IMType::imatchReferenceType(ReferenceType const *conc, ReferenceType const *pat, MatchFlags flags)
{
  if (flags & MF_DEDUCTION) {
    // (in/t0114.cc, in/d0072.cc) allow pattern to be more cv-qualified;
    // this only approximates 14.8.2.1, but emulates current matchtype
    // behavior (actually, it emulates only the intended matchtype
    // behavior; see comment added 2005-08-03 to MatchTypes::match_ref)
    if (superset(pat->atType->getCVFlags(), conc->atType->getCVFlags())) {
      // disable further comparison of these cv-flags
      flags |= MF_IGNORE_TOP_CV;
    }
  }

  return imatchType(conc->atType, pat->atType, flags & MF_PTR_PROP);
}


bool IMType::imatchFunctionType(FunctionType const *conc, FunctionType const *pat, MatchFlags flags)
{
  // I do not compare 'FunctionType::flags' explicitly since their
  // meaning is generally a summary of other information, or of the
  // name (which is irrelevant to the type)

  if (!(flags & MF_IGNORE_RETURN)) {
    // check return type
    if (!imatchType(conc->retType, pat->retType, flags & MF_PROP)) {
      return false;
    }
  }

  if ((conc->flags | pat->flags) & FF_NO_PARAM_INFO) {
    // at least one of the types does not have parameter info,
    // so no further comparison is possible
    return true;            // gcov-ignore: cannot trigger in C++ mode
  }

  if (!(flags & MF_STAT_EQ_NONSTAT)) {
    // check that either both are nonstatic methods, or both are not
    if (conc->isMethod() != pat->isMethod()) {
      return false;
    }
  }

  // check that both are variable-arg, or both are not
  if (conc->acceptsVarargs() != pat->acceptsVarargs()) {
    return false;
  }

  // check the parameter lists (do not mask with MF_PROP here,
  // it happens inside 'imatchParameterLists')
  if (!imatchParameterLists(conc, pat, flags)) {
    return false;
  }

  if (flags & MF_COMPARE_EXN_SPEC) {
    // check the exception specs
    if (!imatchExceptionSpecs(conc, pat, flags & MF_PROP)) {
      return false;
    }
  }

  return true;
}

bool IMType::imatchParameterLists(FunctionType const *conc, FunctionType const *pat,
                                  MatchFlags flags)
{
  SObjListIter<Variable> concIter(conc->params);
  SObjListIter<Variable> patIter(pat->params);

  // skip the 'this' parameter(s) if desired, or if one has it
  // but the other does not (can arise if MF_STAT_EQ_NONSTAT has
  // been specified)
  {
    bool ignore = flags & MF_IGNORE_IMPLICIT;

    bool cm = conc->isMethod();
    CVFlags ccv = CV_NONE;

    bool pm = pat->isMethod();
    CVFlags pcv = CV_NONE;

    if (cm) {
      ccv = concIter.data()->type->asReferenceType()->atType->getCVFlags();
      if (!pm || ignore) {
        concIter.adv();
      }
    }

    if (pm) {
      pcv = patIter.data()->type->asReferenceType()->atType->getCVFlags();
      if (!cm || ignore) {
        patIter.adv();
      }
    }

    if (flags & MF_IGNORE_FUNC_CV) {
      // Ignore cv-flags.
    }
    else if ((flags & MF_STAT_EQ_NONSTAT) && (cm != pm)) {
      // The functions differ in their staticness, and that is a
      // difference that we have explicitly ignored.  In such a case,
      // looking at the cv-flags would again expose the difference in
      // staticness, so we don't.
      //
      // TODO: This feels wrong.  It seems like we should let the
      // differing cv-flags imply disequality.  But the mechanism for
      // determining signature equivalence when validating members of
      // an overload set currently requires it; see in/t0121.cc for
      // some examples of what this validation entails.  Probably if I
      // rewrite that code (which was written before function cv-flag
      // checking was separable from receiver object checking), then
      // this ugly exception to an otherwise straightforward rule
      // could be elimintated.
    }
    else {
      // Note that if we didn't skip the receivers, their cv-flags will
      // be compared again, below, but that should be harmless.
      //
      // Example of requiring cv-flag sensitivity: in/t0591.cc
      //
      if (ccv != pcv) {
        return false;
      }
    }
  }

  // this takes care of imatchFunctionType's obligation to suppress
  // non-propagated flags after consumption; it is masked *after*
  // we check MF_IGNORE_IMPLICIT
  flags &= MF_PROP;

  // allow toplevel cv flags on parameters to differ
  if (!( flags & MF_RESPECT_PARAM_CV )) {
    flags |= MF_IGNORE_TOP_CV;
  }

  for (; !concIter.isDone() && !patIter.isDone();
       concIter.adv(), patIter.adv()) {
    // parameter names do not have to match, but
    // the types do
    if (imatchType(concIter.data()->type, patIter.data()->type, flags)) {
      // ok
    }
    else {
      return false;
    }
  }

  return concIter.isDone() == patIter.isDone();
}


// almost identical code to the above.. list comparison is
// always a pain..
bool IMType::imatchExceptionSpecs(FunctionType const *conc, FunctionType const *pat, MatchFlags flags)
{
  if (conc->exnSpec==NULL && pat->exnSpec==NULL)  return true;
  if (conc->exnSpec==NULL || pat->exnSpec==NULL)  return false;

  // hmm.. this is going to end up requiring that exception specs
  // be listed in the same order, whereas I think the semantics
  // imply they're more like a set.. oh well
  //
  // but if they are a set, how the heck do you do matching?
  // I think I see; the matching must have already led to effectively
  // concrete types on both sides.  But, since I only see 'pat' as
  // the pattern + substitutions, it would still be hard to figure
  // out the correct correspondence for the set semantics.
  
  // this will at least ensure I do not derive any bindings from
  // the attempt to compare exception specs
  flags |= MF_NO_NEW_BINDINGS;

  SObjListIter<Type> concIter(conc->exnSpec->types);
  SObjListIter<Type> patIter(pat->exnSpec->types);
  for (; !concIter.isDone() && !patIter.isDone();
       concIter.adv(), patIter.adv()) {
    if (imatchType(concIter.data(), patIter.data(), flags)) {
      // ok
    }
    else {
      return false;
    }
  }

  return concIter.isDone() == patIter.isDone();
}


// Determine what match flags to propagate past an array type
// constructor, where 'eltType' is the type of the array elements and
// 'flags' are the match flags from above.
static MatchFlags propagateFlagsForArray(MatchFlags flags, Type *eltType)
{
  // what flags to propagate?
  MatchFlags propFlags = (flags & MF_PROP);

  if (flags & MF_IGNORE_ELT_CV) {
    if (eltType->isPDSArrayType()) {
      // propagate the ignore-elt down through as many ArrayTypes
      // as there are
      propFlags |= MF_IGNORE_ELT_CV;
    }
    else {
      // the next guy is the element type, ignore *his* cv only
      propFlags |= MF_IGNORE_TOP_CV;
    }
  }

  return propFlags;
}


bool IMType::imatchArrayType(ArrayType const *conc, ArrayType const *pat, MatchFlags flags)
{
  // what flags to propagate?
  MatchFlags propFlags = propagateFlagsForArray(flags, conc->eltType);

  if (!( imatchType(conc->eltType, pat->eltType, propFlags) &&
         conc->hasSize() == pat->hasSize() )) {
    return false;
  }

  // TODO: At some point I will implement dependent-sized arrays
  // (t0435.cc), at which point the size comparison code here will
  // have to be generalized.

  if (conc->hasSize()) {
    return conc->size == pat->size;
  }
  else {
    return true;
  }
}


bool IMType::imatchPointerToMemberType(PointerToMemberType const *conc,
                                       PointerToMemberType const *pat, MatchFlags flags)
{
  return imatchAtomicType(conc->inClassNAT, pat->inClassNAT, flags) &&
         imatchCVFlags(conc->cv, pat->cv, flags) &&
         imatchType(conc->atType, pat->atType, flags & MF_PTR_PROP);
}


bool IMType::imatchDependentSizedArrayType(DependentSizedArrayType const *conc,
                                           DependentSizedArrayType const *pat, MatchFlags flags)
{
  MatchFlags propFlags = propagateFlagsForArray(flags, conc->eltType);

  return imatchType(conc->eltType, pat->eltType, propFlags) &&
         imatchExpression(conc->sizeExpr, pat->sizeExpr, propFlags);
}


// -------------------- IMType: Expression ---------------------
// This is not full-featured matching as with types, rather this
// is mostly just comparison for equality.
bool IMType::imatchExpression(Expression const *conc, Expression const *pat, MatchFlags flags)
{
  if (conc->isE_grouping()) {
    return imatchExpression(conc->asE_groupingC()->expr, pat, flags);
  }
  if (pat->isE_grouping()) {
    return imatchExpression(conc, pat->asE_groupingC()->expr, flags);
  }

  // in/t0610.cc: We need to allow a type variable to bind to
  // another expression.
  if (pat->isE_variable()) {
    E_variable const *evar = pat->asE_variableC();
    if (evar->var->isTemplateParam() &&
        canUseAsVariable(evar->var, flags)) {
      // package 'conc' as an STemplateArgument so I can
      // re-use the existing binding code
      STemplateArgument sarg;

      // argh, more const problems in the template design...
      // I'm pretty sure that wrapping an expression in an
      // STemplateArgument does not jeopardize constness
      //
      // TODO: Fix this mess.  Why doesn't setDepExpr just
      // accept an Expression const *?
      sarg.setDepExpr(const_cast<Expression*>(conc));

      // call into said binding code
      return imatchNontypeWithVariable(&sarg, evar, flags);
    }
  }

  if (conc->kind() != pat->kind()) {
    return false;
  }

  // turn off variable matching for this part because expression
  // comparison should be fairly literal
  flags &= ~MF_MATCH;

  ASTSWITCH2C(Expression, conc, pat) {
    // only the expression constructors that yield integers and do not
    // have side effects are allowed within type constructors, so that
    // is all we deconstruct here
    //
    // TODO: should 65 and 'A' be regarded as equal here?  For now, I
    // do not regard them as equivalent...
    ASTCASE2C(E_boolLit, c, p) {
      return c->b == p->b;
    }
    ASTNEXT2C(E_intLit, c, p) {
      return c->i == p->i;
    }
    ASTNEXT2C(E_charLit, c, p) {
      return c->c == p->c;
    }
    ASTNEXT2C(E_variable, c, p) {
      if (c->var == p->var) {
        return true;
      }
      if (c->var->isTemplateParam() &&
          p->var->isTemplateParam()) {
        // like for matchTypeVariable
        return c->var->sameTemplateParameter(p->var);
      }
      return false;
    }
    ASTNEXT2C(E_sizeof, c, p) {
      // like above: is sizeof(int) the same as 4?
      return imatchExpression(c->expr, p->expr, flags);
    }
    ASTNEXT2C(E_unary, c, p) {
      return c->op == p->op &&
             imatchExpression(c->expr, p->expr, flags);
    }
    ASTNEXT2C(E_binary, c, p) {
      return c->op == p->op &&
             imatchExpression(c->e1, p->e1, flags) &&
             imatchExpression(c->e2, p->e2, flags);
    }
    ASTNEXT2C(E_cast, c, p) {
      return imatchType(c->ctype->getType(), p->ctype->getType(), flags) &&
             imatchExpression(c->expr, p->expr, flags);
    }
    ASTNEXT2C(E_cond, c, p) {
      return imatchExpression(c->cond, p->cond, flags) &&
             imatchExpression(c->th, p->th, flags) &&
             imatchExpression(c->el, p->el, flags);
    }
    ASTNEXT2C(E_sizeofType, c, p) {
      return imatchType(c->atype->getType(), p->atype->getType(), flags);
    }
    ASTNEXT2C(E_keywordCast, c, p) {
      return c->key == p->key &&
             imatchType(c->ctype->getType(), p->ctype->getType(), flags) &&
             imatchExpression(c->expr, p->expr, flags);
    }
    ASTDEFAULT2C {
      // For expressions that are *not* const-eval'able, we can't get
      // here because tcheck reports an error and bails before we get
      // a chance.  So, the only way to trigger this code is to extend
      // the constant-expression evaluator to handle a new form, and
      // then fail to update the comparison code here to compare such
      // forms with each other.
      xfailure("some kind of expression is const-eval'able but mtype "
               "does not know how to compare it");     // gcov-ignore
    }
    ASTENDCASE2C
  }
}


bool IMType::imatchArraySizeWithExpression
  (int concSize, Expression *patSize, MatchFlags flags)
{
  if (concSize < 0) {
    // one of the unspecified array "sizes"; no match
    return false;
  }

  // apparently the grouping parens have been stripped
  xassert(!patSize->isE_grouping());

  // all that we can do is bind an E_variable
  if (patSize->isE_variable()) {
    E_variable *evar = patSize->asE_variable();
    if (evar->var->isTemplateParam() &&
        canUseAsVariable(evar->var, flags)) {
      // package 'concSize' as an STemplateArgument so I can
      // re-use the existing binding code
      STemplateArgument sarg;
      sarg.setInt(concSize);

      // call into said binding code
      return imatchNontypeWithVariable(&sarg, evar, flags);
    }
  }
  
  return false;
}


// -------------------------- MType --------------------------
MType::MType(bool allowNonConst_)
  : allowNonConst(allowNonConst_)
{}

MType::~MType()
{}


MType::MType(Env &e)
  : allowNonConst(true)
{
  env = &e;
}


bool MType::matchType(Type const *conc, Type const *pat, MatchFlags flags)
{
  // I can only uphold the promise of not modifying 'conc' and 'pat'
  // if asked to when I was created.
  xassert(!allowNonConst);

  return commonMatchType(conc, pat, flags);
}

bool MType::matchTypeNC(Type *conc, Type *pat, MatchFlags flags)
{
  return commonMatchType(conc, pat, flags);
}

bool MType::commonMatchType(Type const *conc, Type const *pat, MatchFlags flags)
{
  // 14.8.2.1p2b3
  if ((flags & MF_DEDUCTION) &&
      !pat->isReferenceType()) {
    flags |= MF_IGNORE_TOP_CV;
  }

  bool result = imatchType(conc, pat, flags);

  #ifndef NDEBUG
    static bool doTrace = tracingSys("mtype");
    if (doTrace) {
      std::ostream &os = trace("mtype");
      os << "conc=`" << conc->toString()
         << "' pat=`" << pat->toString()
         << "' flags={" << toString(flags)
         << "}; match=" << (result? "true" : "false")
         ;
         
      if (failedDueToDQT) {
        os << " (failedDueToDQT)";
      }

      if (result) {
        os << bindingsToString();
      }

      os << std::endl;
    }
  #endif // NDEBUG

  return result;
}


bool MType::matchSTemplateArguments(ObjList<STemplateArgument> const &conc,
                                    ObjList<STemplateArgument> const &pat,
                                    MatchFlags flags)
{
  xassert(!allowNonConst);

  return commonMatchSTemplateArguments(conc, pat, flags);
}

bool MType::matchSTemplateArgumentsNC(ObjList<STemplateArgument> &conc,
                                      ObjList<STemplateArgument> &pat,
                                      MatchFlags flags)
{
  return commonMatchSTemplateArguments(conc, pat, flags);
}

bool MType::commonMatchSTemplateArguments(ObjList<STemplateArgument> const &conc,
                                          ObjList<STemplateArgument> const &pat,
                                          MatchFlags flags)
{
  bool result = imatchSTemplateArguments(conc, pat, flags);

  #ifndef NDEBUG
    static bool doTrace = tracingSys("mtype");
    if (doTrace) {
      std::ostream &os = trace("mtype");
      os << "conc=" << sargsToString(conc)
         << " pat=" << sargsToString(pat)
         << " flags={" << toString(flags)
         << "}; match=" << (result? "true" : "false")
         ;

      if (result) {
        os << bindingsToString();
      }

      os << std::endl;
    }
  #endif // NDEBUG

  return result;
}


bool MType::matchAtomicType(AtomicType const *conc, AtomicType const *pat,
                            MatchFlags flags)
{
  xassert(!allowNonConst);
  return imatchAtomicType(conc, pat, flags);
}


bool MType::matchSTemplateArgument(STemplateArgument const *conc,
                                   STemplateArgument const *pat, MatchFlags flags)
{
  xassert(!allowNonConst);
  return imatchSTemplateArgument(conc, pat, flags);
}


bool MType::matchExpression(Expression const *conc, Expression const *pat, MatchFlags flags)
{
  xassert(!allowNonConst);
  return imatchExpression(conc, pat, flags);
}


string MType::bindingsToString() const
{
  // extract bindings
  stringBuilder sb;
  sb << "; bindings:";
  for (BindingMap::IterC iter(bindings); !iter.isDone(); iter.adv()) {
    sb << " \"" << iter.key() << "\"=`" << iter.value()->asString() << "'";
  }
  return sb;
}


int MType::getNumBindings() const
{
  return bindings.getNumEntries();
}


bool MType::isBound(StringRef name) const
{
  return !!bindings.getC(name);
}


STemplateArgument MType::getBoundValue(StringRef name, TypeFactory &tfac) const
{
  // you can't do this with the matcher that promises to
  // only work with const pointers; this assertion provides
  // the justification for the const_casts below
  xassert(allowNonConst);

  Binding const *b = bindings.getC(name);
  if (!b) {
    return STemplateArgument();
  }

  if (b->sarg.isAtomicType()) {
    // the STA_ATOMIC kind is only for internal use by this module;
    // we translate it into STA_TYPE for external use (but the caller
    // has to provide a 'tfac' to allow the translation)
    Type *t = tfac.makeCVAtomicType(
      const_cast<AtomicType*>(b->sarg.getAtomicType()), CV_NONE);
    return STemplateArgument(t);
  }

  if (b->sarg.isType()) {
    // similarly, we have to combine the type in 'sarg' with 'cv'
    // before exposing it to the user
    Type *t = tfac.setQualifiers(SL_UNKNOWN, b->cv,
                                 const_cast<Type*>(b->getType()), NULL /*syntax*/);
    return STemplateArgument(t);
  }

  // other cases are already in the public format
  return b->sarg;
}


void MType::setBoundValue(StringRef name, STemplateArgument const &value)
{
  xassert(value.hasValue());
  
  Binding *b = new Binding;
  b->sarg = value;
  if (value.isType()) {
    b->cv = value.getType()->getCVFlags();
  }

  bindings.addReplace(name, b);
}


// ---------------------- global funcs ------------------------
bool callMatchType(Type const *conc, Type const *pat, MatchFlags flags)
{
  MType match;
  return match.matchType(conc, pat, flags);
}

bool callMatchAtomicType(AtomicType const *conc, AtomicType const *pat,
                         MatchFlags flags)
{
  MType match;
  return match.matchAtomicType(conc, pat, flags);
}


// EOF
