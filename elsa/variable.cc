// variable.cc            see license.txt for copyright and terms of use
// code for variable.h

#include "variable.h"      // this module
#include "template.h"      // Type, TemplateInfo, etc.
#include "trace.h"         // tracingSys
#include "mangle.h"        // mangle()

// dsw: need this for Oink; we'll figure out how to make this non-global later
bool variablesLinkerVisibleEvenIfNonStaticDataMember = false;

// ---------------------- SomeTypeVarNotInTemplParams_Pred --------------------

// existential search for a type variable that is not in the template
// parameters
class SomeTypeVarNotInTemplParams_Pred : public TypePred {
  TemplateInfo *ti;
  public:
  SomeTypeVarNotInTemplParams_Pred(TemplateInfo *ti0) : ti(ti0) {}
  virtual bool operator() (Type const *t);
  virtual ~SomeTypeVarNotInTemplParams_Pred() {}
};

bool SomeTypeVarNotInTemplParams_Pred::operator() (Type const *t)
{
  if (!t->isCVAtomicType()) return false;
  CVAtomicType const *cv = t->asCVAtomicTypeC();

  if (cv->isCompoundType()) {
    CompoundType const *cpd = cv->asCompoundTypeC();
    // recurse on all of the arugments of the template instantiation
    // if any
    if (cpd->templateInfo()) {
      FOREACH_OBJLIST_NC(STemplateArgument, cpd->templateInfo()->arguments, iter) {
        STemplateArgument *sta = iter.data();
        if (sta->isType()) {
          if (sta->getType()->anyCtorSatisfies(*this)) return true;
        }
      }
    }
    return false;
  }

  if (cv->isTypeOrTemplateTypeVariable()) {
    // check that this tvar occurs in the parameters list of the
    // template info
    Variable *tvar = cv->asTypeVariableC()->typedefVar;
    if (ti->hasSpecificParameter(tvar)) {
      return false;
    }
    return true;
  }

  return false;                 // some other type of compound type
}

size_t Variable::numVariables = 0;

// ---------------------- Variable --------------------
Variable::Variable(SourceLoc L, StringRef n, Type *t, DeclFlags f)
  : loc(L),
    name(n),
    type(t),
    flags(f),
    value(NULL),
    defaultParamType(NULL),
    funcDefn(NULL),
    overload(NULL),
    virtuallyOverride(NULL),
    scope(NULL),
    usingAlias_or_parameterizedEntity(NULL),
    templInfo(NULL)
{
  // the first time through, do some quick tests of the
  // encodings of 'intData'
  static int didSelfTest = false;
  if (!didSelfTest) {
    didSelfTest = true;

    setAccess(AK_PRIVATE);
    setScopeKind(SK_NAMESPACE);
    setParameterOrdinal(1000);

    xassert(getAccess() == AK_PRIVATE);
    xassert(getScopeKind() == SK_NAMESPACE);
    xassert(getParameterOrdinal() == 1000);

    // opposite order
    setParameterOrdinal(88);
    setScopeKind(SK_CLASS);
    setAccess(AK_PROTECTED);

    xassert(getAccess() == AK_PROTECTED);
    xassert(getScopeKind() == SK_CLASS);
    xassert(getParameterOrdinal() == 88);
  }

  setAccess(AK_PUBLIC);
  setScopeKind(SK_UNKNOWN);
  setParameterOrdinal(0);

  if (!isNamespace()) {
    xassert(type);
  }

  ++numVariables;
}

// ctor for de-serialization
Variable::Variable(XmlReader&)
  : loc(SL_UNKNOWN),
    name(NULL),
    type(NULL),
    flags(DF_NONE),
    value(NULL),
    defaultParamType(NULL),
    funcDefn(NULL),
    overload(NULL),
    virtuallyOverride(NULL),
    scope(NULL),
    usingAlias_or_parameterizedEntity(NULL),
    templInfo(NULL)
{
  ++numVariables;
}

Variable::~Variable()
{}


void Variable::setFlagsTo(DeclFlags f)
{
  // this method is the one that gets to modify 'flags'
  const_cast<DeclFlags&>(flags) = f;
}

bool Variable::isSemanticallyGlobal() const {
  switch(getScopeKind()) {
  case NUM_SCOPEKINDS:
    xfailure("can't happen");
    break;                      // silly gcc warning
  case SK_UNKNOWN:              // not yet registered in a scope
    // FIX: the global scope has ScopeKind SK_UNKNOWN
//     xfailure("got SK_UNKNOWN on a real variable");
    break;
  case SK_GLOBAL:               // toplevel names
    return true;
    break;
  case SK_FUNCTION:             // includes local variables
    if (hasFlag(DF_STATIC)) {
      return true;
    }
    break;
  case SK_CLASS:                // class member scope
    if (hasFlag(DF_STATIC)) {
      return true;
    }
    break;
  default:
    break;
  }
  return false;
}

bool Variable::inGlobalOrNamespaceScope() const {
  return scope && (scope->isGlobalScope() || scope->isNamespace());
}

bool Variable::linkerVisibleName() const {
  return linkerVisibleName(false);
}

bool Variable::linkerVisibleName(bool evenIfStaticLinkage) const {
//    bool oldAnswer;
//    if (scope) oldAnswer = scope->linkerVisible();
//    else oldAnswer = hasFlag(DF_GLOBAL);

  // do not consider templates
  if (isTemplate()) return false;

  // do not consider members of uninstantiated template primaries or
  // partial specializations
  if (isUninstTemplateMember()) return false;

  // it seems that we should not consider typedefs to be linker-visible
  if (hasFlag(DF_TYPEDEF)) return false;
  // nor namespaces, such as '<globalScope>'
  if (hasFlag(DF_NAMESPACE)) return false;

  // dsw: nothing starting with __builtin_va is linker-visible
  static char *builtin_va_prefix = "__builtin_va";
  static int builtin_va_prefix_len = strlen(builtin_va_prefix);
  if (name && 0==strncmp(name, builtin_va_prefix, builtin_va_prefix_len)) return false;

  // quarl 2006-06-03, 2006-07-11
  //     *In C++ mode only*, inline implies static.  In C mode, 'inline'
  //     functions can be non-static (see e.g. package gnutls11).
  //     isStaticLinkage() now handles this implication with help from
  //     typechecking.  A function can both be a static member and have static
  //     linkage (via inline).
  if (!evenIfStaticLinkage) {
    if (isStaticLinkage()) {
      return false;
    }
  }

  // FIX: what the heck was this?  Some attempt to treat struct
  // members as linkerVisibleName-s?  This doesn't work because there
  // is no well-defined name for an anonymous struct anyway.
  if (!scope) {
    // FIX: I hope this is right.
    // FIX: hmm, when else can this occur?
//      xassert(hasFlag(DF_PARAMETER));
    // this one fails for template parameters!?
//      xassert(!hasFlag(DF_GLOBAL));
    return false;
  }

  // quarl 2006-07-11
  //    Check for this even if not in class scope, e.g. a struct defined
  //    within a function (Test/struct_sizeof.c)
  if (!scope->linkerVisible()) {
    return false;
  }

  // dsw: I hate this overloading of the 'static' keyword.  Static members of
  // CompoundTypes are linker visible if the CompoundType is linkerVisible.
  // Non-static members are visible only if they are FunctionTypes.
  if (scope->isClassScope()) {
    if (!hasFlag(DF_MEMBER)) {
      return false;
    }

    // non-static members of a class
    if (!variablesLinkerVisibleEvenIfNonStaticDataMember) {
      if (!(type->asRval()->isFunctionType() || hasFlag(DF_STATIC))) {
        return false;
      }
    }
  }

  return true;
}


// 2005-08-15: Previously, this would return false for unions because
// I misunderstood the standard.  However, 9p4 is clear that unions
// are classes, and can have destructors, etc. (in/t0571.cc)
bool Variable::isClass() const
{
  return hasFlag(DF_TYPEDEF) && type->isCompoundType();
}


bool Variable::isUninstTemplateMember() const
{
  if (isTemplate() &&
      !templateInfo()->isCompleteSpecOrInstantiation()) {
    return true;
  }
  return scope && scope->isWithinUninstTemplate();
}


bool Variable::isUninstClassTemplMethod() const
{
  return hasFlag(DF_VIRTUAL) && type->isMethod() && isInstantiation() && !funcDefn;
}


bool Variable::isTemplate(bool considerInherited) const
{
  if (templateInfo() &&
      templateInfo()->hasParametersEx(considerInherited)) {
    return true;
  }

  // not sure if this is a good idea ...
  //
  // template type variable?
  if (isTemplateTypeVariable()) {
    return true;
  }

  return false;
}


TemplateParameterKind Variable::getTemplateParameterKind() const
{
  xassert(isTemplateParam());

  if (isBoundTemplateParam()) {
    if (isType()) {
      xassert(type);
      if (type->isCVAtomicType()) {
        AtomicType *at = type->asCVAtomicTypeC()->atomic;
        if (at->isTemplateTypeVariable()) {
          // bound to a template template param; probably we're
          // still inside some surrounding template?  anyway,
          // if bound to TTP, then is TTP
          return TPK_TEMPLATE;
        }
        if (at->isCompoundType() &&
            at->asCompoundType()->isTemplate()) {
          // this is the usual "concrete" binding case
          return TPK_TEMPLATE;
        }
      }
      return TPK_TYPE;
    }
    else {
      return TPK_NON_TYPE;
    }
  }

  else {
    xassert(isAbstractTemplateParam());

    if (isType()) {
      xassert(type && type->isCVAtomicType());
      AtomicType *at = type->asCVAtomicTypeC()->atomic;
      if (at->isTypeVariable()) {
        return TPK_TYPE;
      }
      if (at->isTemplateTypeVariable()) {
        return TPK_TEMPLATE;
      }
      xfailure("unexpected type for an unbound template parameter");
    }

    return TPK_NON_TYPE;
  }
}


#if 0     // delete me
bool Variable::isTemplateTypeVariable() const
{
  return type &&
         type->isCVAtomicType() &&
         type->asCVAtomicTypeC()->atomic->isTemplateTypeVariable();
}
#endif // 0


bool Variable::isTemplateFunction(bool considerInherited) const
{
  return type &&
         type->isFunctionType() &&
         isTemplate(considerInherited) &&
         !hasFlag(DF_TYPEDEF);
}


bool Variable::isTemplateClass(bool considerInherited) const
{
  if (hasFlag(DF_TYPEDEF)) {
    if (type->isCompoundType() &&
        isTemplate(considerInherited)) {
      return true;
    }
    
    if (isTemplateTypeVariable()) {
      return true;
    }
  }
  
  return false;
}


bool Variable::isInstantiation() const
{
  return templInfo && templInfo->isInstantiation();
}


TemplateInfo *Variable::templateInfo() const
{
  // 2005-02-23: experiment: alias shares referent's template info
  return skipAliasC()->templInfo;
}

void Variable::setTemplateInfo(TemplateInfo *templInfo0)
{
  templInfo = templInfo0;

  // 2005-03-07: this assertion fails in some error cases (e.g., error
  // 1 of in/t0434.cc); I tried a few hacks but am now giving up on it
  // entirely
  //xassert(!(templInfo && notQuantifiedOut()));

  // complete the association
  if (templInfo) {
    // I am the method allowed to change TemplateInfo::var
    const_cast<Variable*&>(templInfo->var) = this;
  }
  else {
    // this happens when we're not in a template at all, but the
    // parser just takes the pending template info (which is NULL)
    // and passes it in here anyway
  }
}


bool Variable::notQuantifiedOut()
{
  TemplateInfo *ti = templateInfo();
  if (!ti) return false;
  SomeTypeVarNotInTemplParams_Pred pred(ti);
  return type->anyCtorSatisfies(pred);
}


void Variable::gdb() const
{
  cout << toString() << endl;
}

string Variable::toString() const
{
  if (Type::printAsML) {
    return toMLString();
  }
  else {
    return toCString();
  }
}


string Variable::toCString() const
{
  // as an experiment, I'm saying public(field) in the .ast file
  // in a place where the Variable* might be NULL, so I will
  // tolerate a NULL 'this'
  if (this == NULL) {
    return "NULL";
  }

  // The purpose of this method is to print the name and type
  // of this Variable object, in a debugging context.  It is
  // not necessarily intended to print them in a way consistent
  // with the C syntax that might give rise to the Variable.
  // If more specialized printing is desired, do that specialized
  // printing from outside (by directly accessing 'name', 'type',
  // 'flags', etc.).
  //
  // dsw: namespace variables have no type
  if (type) {
    return type->toCString(stringc << (name? name : "/*anon*/") << namePrintSuffix());
  } else {
    return name? name : "/*anon*/";
  }
}


string Variable::toQualifiedString() const
{
  if (isType()) {
    // I'm seeing printouts like "S1<T>::S2 S1<T>::S2", where the type
    // is printed twice.  I think for types we should just print the
    // type itself.
    return type->toCString();
  }

  string qname;
  if (name) {
    qname = fullyQualifiedName0();
  }
  else {
    qname = "/*anon*/";
  }
  return type->toCString(stringc << qname << namePrintSuffix());
}


string Variable::toCStringAsParameter() const
{
  if (!global_mayUseTypeAndVarToCString) xfailure("suspended during CTypePrinter::print");
  stringBuilder sb;
  if (type->isTypeVariable()) {
    // type variable's name, then the parameter's name (if any)
    sb << type->asTypeVariable()->name;
    if (name) {
      sb << " " << name;
    }
  }
  else if (type->isTemplateTypeVariable()) {
    TemplateTypeVariable *ttv = type->asTemplateTypeVariable();
    sb << ttv->params.paramsToCString() << " class";
    if (name) {
      sb << " " << name;
    }
  }
  else {
    sb << type->toCString(name);
  }

  if (value) {
    sb << renderExpressionAsString(" = ", value);
  }
  return sb;
}


string Variable::toMLString() const
{
  stringBuilder sb;
  #if USE_SERIAL_NUMBERS
    printSerialNo(sb, "v", serialNumber, "-");
  #endif
  char const *name0 = "<no_name>";
  if (name) {
    name0 = name;
  }
  sb << "'" << name0 << "'->" << type->toMLString();
  return sb;
}


string Variable::fullyQualifiedName0() const
{
  // dsw: I don't think it makes sense to ask for the fully qualified
  // name of a Variable that does not reside in a permanent scope,
  // except for built-ins which are basically global.  Unfortunately,
  // Scott doesn't use it that way.
//   if (loc != SL_INIT) {
//     xassert(scope);
//     xassert(scope->hasName() || scope->isGlobalScope());
//   }

  if (isNamespace()) {
    if (scope->isGlobalScope()) {
      return "::";
    }
    else {
      return scope->fullyQualifiedCName();
    }
  }

  stringBuilder tmp;
  if (scope && !scope->isGlobalScope()) {
    tmp << scope->fullyQualifiedCName();
  }
  if (hasFlag(DF_SELFNAME)) {
    // don't need another "::name", since my 'scope' is the same
  }
  else {
    if (!tmp.empty()) {
      tmp << "::";
    }

    if (name) {
      if (streq(name, "conversion-operator")) {
        // special syntax for conversion operators; first the keyword
        tmp << "operator ";

        // then the return type and the function designator
        Type const *ret = type->asFunctionTypeC()->retType;
        tmp << ret->toString();
      }
      else {
        tmp << name;        // NOTE: not mangled
      }

      TemplateInfo *ti = templateInfo();
      if (ti) {
        // only print arguments that do not correspond to inherited params
        // of the thing of which this is an instance
        int numInh = 0;
        if (ti->instantiationOf) {
          numInh = ti->instantiationOf->templateInfo()->countInheritedParameters();
        }
        SObjList<STemplateArgument> const &allArgs = objToSObjListC(ti->arguments);
        SObjListIter<STemplateArgument> iter(allArgs);
        while (numInh && !iter.isDone()) {
          numInh--;
          iter.adv();
        }

        // any args left to print?
        if (!iter.isDone()) {
          tmp << sargsToString(iter);
        }
      }
    }
    else {
      tmp << "/*anon*/";
    }
  }
  return tmp;
}

void Variable::appendMangledness(stringBuilder &mgldName) {
  // for function types, be sure to use the mangled name of their
  // signature so that overloading works right
  if (type && type->isFunctionType() && !hasFlag(DF_EXTERN_C)) {
    mgldName << " SIG " << mangle(type); // this is passed through the globalStrTable below
  }
}

string Variable::mangledName0() {
  // dsw: what was I thinking here?  See assertion at the top of
  // fullyQualifiedName0()
  //
  // NOTE: This is a very important assertion
//   xassert(linkerVisibleName());

  stringBuilder mgldName;
  // FIX: should this be an assertion?  It can fail.
  if (name) mgldName << name;
  appendMangledness(mgldName);
  return mgldName;
}

string Variable::fullyQualifiedMangledName0() {
//    cout << "name '" << name;
//    if (scope) cout << "; has a scope" << endl;
//    else cout << "; NO scope" << endl;

  // dsw: what was I thinking here?  See assertion at the top of
  // fullyQualifiedName0()
  //
  // NOTE: dsw: This is a very important assertion
//   xassert(linkerVisibleName());

  stringBuilder fqName;

  // dsw: prepend with the filename if is global and static; this
  // ensures proper linking
  if (isStaticLinkage()) {
    fqName << "file:" << sourceLocManager->getFile(loc) << ";";
  }

  // quarl 2006-07-10
  //    Prepend with either "D:" or "F:" for data/function.  This way we
  //    imitate the real linker's "type system" for data and functions
  //    (e.g. "clog" the iostream data member and "clog" the math function can
  //    coexist).
  //
  //    Should we add more -- templates?

  if (type->isFunctionType()) {
    fqName << "F:";
  } else {
    fqName << "D:";
  }

  fqName << fullyQualifiedName0();
//    if (scope) fqName << scope->fullyQualifiedName();
//    fqName << "::" << mangledName0();
  appendMangledness(fqName);

  return fqName;
}


string Variable::namePrintSuffix() const
{
  return "";
}


OverloadSet *Variable::getOrCreateOverloadSet()
{
  xassert(type);
  xassert(type->isFunctionType());
  if (!overload) {
    overload = new OverloadSet;
    overload->addMember(this);
  }
  return overload;
}


void Variable::getOverloadList(SObjList<Variable> &set)
{
  if (overload) {
    set = overload->set;     // copy the elements
  }
  else {
    set.prepend(this);       // just put me into it
  }
}


int Variable::overloadSetSize() const
{
  return overload? overload->count() : 1;
}

bool Variable::isPureVirtualMethod() const
{
  // NOTE: use getHasValue() here instead of testing for the existance
  // of the value field.  The value field is AST and there are some
  // modes of usage where we serialize the typesystem but not the AST;
  // the behavior of this method before and after serialization would
  // alter in such a case.
  bool ret = type && type->isMethod() && getHasValue();
  // FIX: figure out how to add these assertions without having to
  // include cc_ast.h
//   if (ret) {
//     xassert(value->isE_intLit());
//     xassert(value->asE_intLit()->i == 0);
//   }
  return ret;
}

bool Variable::isMemberOfTemplate() const
{
  if (!scope) { return false; }
  if (!scope->curCompound) { return false; }

  if (scope->curCompound->isTemplate()) {
    return true;
  }

  // member of non-template class; ask if that class is itself
  // a member of a template
  return scope->curCompound->typedefVar->isMemberOfTemplate();
}


bool Variable::isTemplateTypeParam() const
{
  // The second part of this test is how we can distinguish type
  // parameters from non-type parameters whose type happens to be a
  // previous type parameter.  For example, in
  //   template <class T, T i>      // in/t0505.cc
  // 'T' is a type parameter, while 'i' is a non-type parameter but
  // its type is a type parameter.
  return hasFlag(DF_TYPEDEF) &&              // true only of 'T'
         type->isTypeVariable();             // true of both 'T' and 'i'
}


Variable *Variable::getUsingAlias() const
{
  if (!isTemplateParam()) {
    if (usingAlias_or_parameterizedEntity) {
      xassert(usingAlias_or_parameterizedEntity->getMaybeUsedAsAlias());
    }
    return usingAlias_or_parameterizedEntity;
  }
  else {
    return NULL;
  }
}

void Variable::setUsingAlias(Variable *target)
{
  xassert(!isTemplateParam());
  usingAlias_or_parameterizedEntity = target;
  if (usingAlias_or_parameterizedEntity) {
    usingAlias_or_parameterizedEntity->setMaybeUsedAsAlias(true);
  }
}


Variable *Variable::getParameterizedEntity() const
{
  if (isTemplateParam()) {
    return usingAlias_or_parameterizedEntity;
  }
  else {
    return NULL;
  }
}

void Variable::setParameterizedEntity(Variable *templ)
{
  xassert(isTemplateParam());
  usingAlias_or_parameterizedEntity = templ;
}


bool Variable::sameTemplateParameter(Variable const *other) const
{
  if (getParameterizedEntity() == NULL) {
    // I haven't been associated with a template yet, so don't
    // claim to be the same as anything else
    return false;
  }

  if (getParameterOrdinal() == other->getParameterOrdinal() &&
      getParameterizedEntity() == other->getParameterizedEntity()) {
    // same!
    return true;
  }

  return false;
}


// I'm not sure what analyses' disposition towards usingAlias ought to
// be.  One possibility is to just say they should sprinkle calls to
// skipAlias all over the place, but that's obviously not very nice.
// However, I can't just make the lookup functions call skipAlias,
// since the access control for the *alias* is what's relevant for
// implementing access control restrictions.  Perhaps there should be
// a pass that replaces all Variables in the AST with their skipAlias
// versions?  I don't know yet.  Aliasing is often convenient for the
// programmer but a pain for the analysis.

Variable const *Variable::skipAliasC() const
{
  // tolerate NULL 'this' so I don't have to conditionalize usage
  if (this && getUsingAlias()) {
    return getUsingAlias()->skipAliasC();
  }
  else {
    return this;
  }
}


// this isn't right if either is a set of overloaded functions...
bool sameEntity(Variable const *v1, Variable const *v2)
{
  v1 = v1->skipAliasC();
  v2 = v2->skipAliasC();

  if (v1 == v2) {
    return true;
  }

  if (v1 && v2 &&                   // both non-NULL
      v1->name == v2->name &&       // same simple name
      v1->hasFlag(DF_EXTERN_C) &&   // both are extern "C"
      v2->hasFlag(DF_EXTERN_C)) {
    // They are the same entity.. unfortunately, outside this
    // rather oblique test, there's no good way for the analysis
    // to know this in advance.  Ideally the tchecker should be
    // maintaining some symbol table of extern "C" names so that
    // it could use the *same* Variable object for multiple
    // occurrences in different namespaces, but I'm too lazy to
    // implement that now.
    return true;
  }

  return false;
}


static bool namesTemplateFunction_one(Variable const *v)
{
  // we are using this to compare template arguments to the
  // preceding name, so we are only interested in the
  // template-ness of the name itself, not any parent scopes
  bool const considerInherited = false;

  return v->isTemplate(considerInherited) ||
         v->isInstantiation();     // 2005-08-11: in/t0545.cc
}

bool Variable::namesTemplateFunction() const
{
  if (isOverloaded()) {
    // check amongst all overloaded names; 14.2 is not terribly
    // clear about that, but 14.8.1 para 2 example 2 seems to
    // imply this behavior
    SFOREACH_OBJLIST(Variable, overload->set, iter) {
      if (namesTemplateFunction_one(iter.data())) {
        return true;
      }
    }
  }

  else if (namesTemplateFunction_one(this)) {
    return true;
  }

  return false;
}


int Variable::getEnumeratorValue() const
{
  EnumType *et = type->asCVAtomicType()->atomic->asEnumType();
  EnumType::Value const *val = et->getValue(name);
  xassert(val);    // otherwise the type information is wrong..
  return val->value;
}


void Variable::setBitfieldSize(int bits)
{
  xassert(isBitfield());

  setParameterOrdinal(bits);
}

int Variable::getBitfieldSize() const
{
  xassert(isBitfield());

  return getParameterOrdinal();
}


Scope *Variable::getDenotedScope() const
{
  if (isNamespace()) {
    return scope;
  }

  if (type->isCompoundType()) {
    CompoundType *ct = type->asCompoundType();
    return ct;
  }

  xfailure(stringc << "expected `" << name << "' to name a scope");
  return NULL;    // silence warning
}


void Variable::traverse(TypeVisitor &vis) {
  if (!vis.visitVariable(this)) {
    return;
  }
  if (type) {
    type->traverse(vis);
  }
  vis.postvisitVariable(this);
}


// --------------------- OverloadSet -------------------
OverloadSet::OverloadSet()
  : set()
{}

OverloadSet::~OverloadSet()
{}


void OverloadSet::addMember(Variable *v)
{
  // dsw: wow, this can happen when you import two names into a
  // namespace.  So the idea is we allow ambiguity and then only
  // report an error at lookup, which is The C++ Way.
//    xassert(!findByType(v->type->asFunctionType()));
  xassert(v->type->isFunctionType());
  set.prepend(v);
}


// The problem with these is they don't work for templatized types
// because they call 'equals', not MatchType.
//
// But I've re-enabled them for Oink ....
#if 1     // obsolete; see Env::findInOverloadSet

Variable *OverloadSet::findByType(FunctionType const *ft, CVFlags receiverCV)
{
  SFOREACH_OBJLIST_NC(Variable, set, iter) {
    FunctionType *iterft = iter.data()->type->asFunctionType();

    // check the parameters other than '__receiver'
    if (!iterft->equalOmittingReceiver(ft)) continue;

    // if 'this' exists, it must match 'receiverCV'
    if (iterft->getReceiverCV() != receiverCV) continue;

    // ok, this is the right one
    return iter.data();
  }
  return NULL;    // not found
}


Variable *OverloadSet::findByType(FunctionType const *ft) {
  return findByType(ft, ft->getReceiverCV());
}
#endif // obsolete; see Env::findInOverloadSet


// EOF
