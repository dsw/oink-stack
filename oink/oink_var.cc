// see License.txt for copyright and terms of use

#include "oink_var.h"           // this module
#include "cc_type.h"            // SPECIAL: oink_type.h is INCLUDED into cc_type.h
#include "oink_global.h"        // tunitToVars
#include "oink_util.h"


// Variable_O ****************

bool omitCheckAbstrValueOnlyOnRealVars = false;

Variable_O::Variable_O(XmlReader &xmlReader)
  : Variable(xmlReader)
  , abstrValue0(NULL)
{
  setHasFuncDefn(false);
}

Variable_O::Variable_O(SourceLoc L, StringRef n, Type *t, DeclFlags f)
  : Variable(L, n, t, f)
  , abstrValue0(NULL)
{
  setHasFuncDefn(false);
}

Variable_O::~Variable_O() {
  // FIX: do this some other way.  I think the global_done flag is
  // being destructed before the other global dtor that calls this.
//    if (!global_done) xfailure("why are we destructing a Variable_O before global_done?");
}

bool Variable_O::filteredOut() const {
  // any non-real, non-function variable is also not filtered out
  bool ret =
    getReal() &&
    type &&
    type->asRval()->isFunctionType() &&
    !getFilteredKeep();
  // if (streq(name,"operator new") ||
  //     streq(name,"operator new[]") ||
  //     streq(name,"operator delete") ||
  //     streq(name,"operator delete[]") ||
  //     streq(name,"__builtin_next_arg") ||
  //     streq(name,"__builtin_constant_p") ||
  //     streq(name,"__builtin_stdarg_start") ||
  //     streq(name,"__builtin_va_copy") ||
  //     streq(name,"__builtin_strchr") ||
  //     0)
  // {
  //   ret = false;
  // }
  if (oinkCmd->all_pass_filter) {
    USER_ASSERT(!ret, loc, stringc << "-fo-all-pass-filter is on but variable does not pass: " << name);
  }
  return ret;
}

// FIX: make this return a boolean and put the assertion at the use
// point and rename to okForDataflow(); I haven't done this because
// Karl has named the assertion below and I don't feel like finding
// out where it moves to.
void Variable_O::checkOkForDataflow() const {
  if (omitCheckAbstrValueOnlyOnRealVars) {
    // this is a best-effort check in the absence of the ability to
    // do the real check below
    xassert(!isTemplate());
    xassert(!isMemberOfTemplate());
  } else {
    // don't make Values for template vars
    xassert(getReal() && "a6587633-0ba2-452d-9d78-184adb1be79c");
    // nor non-filter-keep vars
    xassert(!filteredOut());
  }
}

Value *Variable_O::abstrValue() const {
  if (!abstrValue0) {
    xassert(type);
    checkOkForDataflow();
    abstrValue0 = vFac->buildValue(type, loc);
  }
  return abstrValue0;
}

void Variable_O::setAbstrValue(Value *t) {
  checkOkForDataflow();
  xassert(!abstrValue0);
  abstrValue0 = t;
}

StringRef Variable_O::fullyQualifiedName_StrRef() /*const*/ {
  return globalStrTable(fullyQualifiedName0());
}

StringRef Variable_O::mangledName_StrRef() /*const*/ {
  return globalStrTable(mangledName0());
}

StringRef Variable_O::fullyQualifiedMangledName_StrRef() /*const*/ {
  return globalStrTable(fullyQualifiedMangledName0());
}

// FIX: somebody check if this is right
bool Variable_O::canBeInherited() const {
  if (! (hasFlag(DF_MEMBER) && !hasFlag(DF_STATIC)) ) return false;
  if (type->isFunctionType()) {
    FunctionType *ft = type->asFunctionType();
    return !ft->isConstructor() && !ft->isDestructor() &&
      !(streq(name, operatorFunctionNames[OP_ASSIGN]));
  }
  return true;
}

void Variable_O::traverse(ValueVisitor &vis) {
  if (!vis.preVisitVariable(this)) return;
  abstrValue()->traverse(vis);
  vis.postVisitVariable(this);
}

// we wish to avoid serializing extern function variables that 1) have
// not participated in dataflow and 2) do not provide a definition so
// cannot later; since qvars are made lazily we simply check if there
// are any qvars attached to the abstract value tree of this variable
// for the first condition and just check for definitions for the
// second one; note that the fact that we only filter out functions is
// implicitly in 1) the way that linkerVisVarsToBeSrz is constructed,
// and 2) the fact that we give namespaces a 'by' here
//
// UPDATE: I think the comment about how we tell if something is used
// in the dataflow is no longer correct.
bool serializeVar_O(Variable *var0) {
  xassert(var0 && "19b23af4-9169-459b-a3c2-90c68ad0acc7");
  Variable_O *var = asVariable_O(var0);

  // do not serialize any function varibles that do not pass the filter
  if (var->filteredOut()) return false;

  // we have to serialize all templates as to not do so would leave
  // instantiations in an inconsistent state
  if (!var->getReal()) return true;

  // we only filter out (by leaving out of the list
  // linker.linkerVisVarsToBeSrz) linker-visible varibles; the rest
  // are always serialized, as they might for example be a function
  // parameter; it would cut things apart to do otherwise
  if (!var->linkerVisibleName()) return true;

  // keep even global namespaces
  if (var->hasFlag(DF_NAMESPACE)) return true;

  // also have to keep any variable that is used as an alias by
  // another
  if (var->getMaybeUsedAsAlias()) return true;

  // other globals keep them only if they made it onto the list
  return linker.linkerVisVarsToBeSrz.contains(var);
}
