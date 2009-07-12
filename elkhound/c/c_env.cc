// c_env.cc            see license.txt for copyright and terms of use
// code for c_env.h

#include "c_env.h"       // this module
#include "trace.h"       // tracingSys
#include "ckheap.h"      // heapCheck
#include "strtable.h"    // StringTable
#include "cc_lang.h"     // CCLang


// --------------------- CFGEnv -----------------------
CFGEnv::CFGEnv()
{
  // make empty top frames
  pushNexts();
  pushBreaks();
}

CFGEnv::~CFGEnv()
{}


// -------- nexts -------
void CFGEnv::pushNexts()
{
  pendingNexts.push(new SObjList<Statement>());
}

void CFGEnv::addPendingNext(Statement *source)
{
  pendingNexts.top()->prepend(source);    // O(n)
}

void CFGEnv::popNexts()
{
  SObjList<Statement> *top = pendingNexts.pop();
  pendingNexts.top()->concat(*top);    // empties 'top'
  delete top;
}

void CFGEnv::clearNexts()
{
  pendingNexts.top()->removeAll();
}

void CFGEnv::resolveNexts(Statement *target, bool isContinue)
{
  SMUTATE_EACH_OBJLIST(Statement, *(pendingNexts.top()), iter) {
    iter.data()->next = makeNextPtr(target, isContinue);
  }
  clearNexts();
}


// -------- breaks --------
void CFGEnv::pushBreaks()
{
  breaks.push(new SObjList<S_break>());
}

void CFGEnv::addBreak(S_break *source)
{
  breaks.top()->prepend(source);     // O(n)
}

void CFGEnv::popBreaks()
{
  // all topmost breaks become pending nexts
  SMUTATE_EACH_OBJLIST(S_break, *(breaks.top()), iter) {
    addPendingNext(iter.data());
  }
  breaks.delPop();
}


// -------- labels --------
void CFGEnv::addLabel(StringRef name, S_label *target)
{
  labels.add(name, target);
}

void CFGEnv::addPendingGoto(StringRef name, S_goto *source)
{
  gotos.add(name, source);
}

void CFGEnv::resolveGotos()
{                                     
  // go over all the gotos and find their corresponding target
  for (StringSObjDict<S_goto>::Iter iter(gotos);
       !iter.isDone(); iter.next()) {    
    S_label *target = labels.queryif(iter.key().c_str());
    if (target) {
      iter.value()->next = makeNextPtr(target, false);
    }
    else {
      err(stringc << "goto to undefined label: " << iter.key());
    }
  }
  
  // empty both dictionaries
  labels.empty();
  gotos.empty();
}


// -------- switches --------
void CFGEnv::pushSwitch(S_switch *sw)
{
  switches.push(sw);
}

S_switch *CFGEnv::getCurrentSwitch()
{
  return switches.top();
}

void CFGEnv::popSwitch()
{
  switches.pop();
}


// --------- loops ----------
void CFGEnv::pushLoop(Statement *loop)
{
  loops.push(loop);
}

Statement *CFGEnv::getCurrentLoop()
{
  return loops.top();
}

void CFGEnv::popLoop()
{
  loops.pop();
}


// -------- end --------
void CFGEnv::verifyFunctionEnd()
{
  xassert(pendingNexts.count() == 1);
  xassert(pendingNexts.top()->count() == 0);
  
  xassert(breaks.count() == 1);
  xassert(breaks.top()->count() == 0);
  
  xassert(labels.size() == 0);
  xassert(gotos.size() == 0);

  xassert(switches.count() == 0);
  xassert(loops.count() == 0);
}


// --------------------- ScopedEnv ---------------------
ScopedEnv::ScopedEnv()
{}

ScopedEnv::~ScopedEnv()
{}


// --------------------------- Env ----------------------------
Env::Env(StringTable &table, CCLang &alang)
  : scopes(),
    typedefs(),
    compounds(),
    enums(),
    enumerators(),
    errors(0),
    warnings(0),
    compoundStack(),
    currentFunction(NULL),
    locationStack(),
    inPredicate(false),
    strTable(table),
    lang(alang)
{
  enterScope();

  #if 0
  declareVariable("__builtin_constant_p", DF_BUILTIN,
    makeFunctionType_1arg(
      getSimpleType(ST_INT),                          // return type
      CV_NONE,
      getSimpleType(ST_INT), "potentialConstant"),    // arg type
      true /*initialized*/);
  #endif // 0
}


Env::~Env()
{
  // explicitly free things, for easier debugging of dtor sequence
  scopes.deleteAll();
  typedefs.empty();
  compounds.empty();
  enums.empty();
  enumerators.empty();
}


void Env::enterScope()
{
  scopes.prepend(new ScopedEnv());
}

void Env::leaveScope()
{
  scopes.deleteAt(0);
}


// NOTE: the name lookup rules in this code have not been
// carefully checked against what the standard requires,
// so they are likely wrong; I intend to go through and
// make them correct at some point


// ---------------------------- variables ---------------------------
void Env::addVariable(StringRef name, Variable *decl)
{
  // convenience
  Type const *type = decl->type;
  DeclFlags flags = decl->flags;

  // shouldn't be using this interface to add typedefs
  xassert(!(flags & DF_TYPEDEF));

  // special case for adding compound type members
  if (compoundStack.isNotEmpty()) {
    addCompoundField(compoundStack.first(), decl);
    return;
  }

  Variable *prev = getVariable(name, true /*inner*/);
  if (prev) {
    // if the old decl and new are the same array type,
    // except the old was missing a size, replace the
    // old with the new
    if (type->isArrayType() &&
        prev->type->isArrayType()) {
      ArrayType const *arr = &( type->asArrayTypeC() );
      ArrayType const *parr = &( prev->type->asArrayTypeC() );
      if (arr->eltType->equals(parr->eltType) &&
          arr->hasSize &&
          !parr->hasSize) {
        // replace old with new
        prev->type = type;
      }
    }

    // no way we allow it if the types don't match
    if (!type->equals(prev->type)) {
      errThrow(stringc
        << "conflicting declaration for `" << name
        << "'; previous type was `" << prev->type->toString()
        << "', this type is `" << type->toString() << "'");
    }

    // TODO: this is all wrong.. I didn't want more than one Variable
    // to exist for a given name, but this function allows more than
    // one....

    // but it's ok if:
    //   - both were functions, or
    //   - one is extern, or
    //   - both were static
    // (TODO: what are the real rules??)
    // and, there can be at most one initializer (why did I comment that out?)
    if ( ( type->isFunctionType() ||
           ((flags & DF_EXTERN) || (prev->flags & DF_EXTERN)) ||
           ((flags & DF_STATIC) && (prev->flags & DF_STATIC))
         )
         //&& (!prev->isInitialized() || !initialized)
       ) {
      // ok
      // at some point I'm going to have to deal with merging the
      // information, but for now.. skip it

      // merging for functions: pre/post
      // disabled: there's a problem because the parameter names don't
      // get symbolically evaluated right
      if (type->isFunctionType()) {
        FunctionType const *prevFn = &( prev->type->asFunctionTypeC() );
        FunctionType const *curFn = &( type->asFunctionTypeC() );

        // if a function has a prior declaration or definition, then
        // I want all later occurrences to *not* have pre/post, but
        // rather to inherit that of the prior definition.  this way
        // nobody gets to use the function before a precondition is
        // attached.  (may change this later, somehow)
        // update: now just checking..
        if (!curFn->precondition != !prevFn->precondition  || 
            !curFn->postcondition != !prevFn->postcondition) {
          warn("pre/post-condition different after first introduction");
        }

        // transfer the prior pre/post to the current one
        // NOTE: this is a problem if the names of parameters are different
        // in the previous declaration
        //decl->type = prev->type;     // since 'curFn' is const, just point 'decl' elsewhere..
      }
    }
    else {
      err(stringc << "duplicate variable decl: " << name);
    }
  }

  else /*not already mapped*/ {
    if (isGlobalEnv()) {
      decl->flags = (DeclFlags)(decl->flags | DF_GLOBAL);
    }

    scopes.first()->variables.add(name, decl);
  }
}


Variable *Env::getVariable(StringRef name, bool innerOnly)
{
  // TODO: add enums to what we search

  MUTATE_EACH_OBJLIST(ScopedEnv, scopes, iter) {
    Variable *v;
    if (iter.data()->variables.query(name, v)) {
      return v;
    }
    
    if (innerOnly) {
      return NULL;    // don't look beyond the first
    }
  }

  return NULL;
}


// ------------------- typedef -------------------------
void Env::addTypedef(StringRef name, Type const *type)
{       
  Type const *prev = getTypedef(name);
  if (prev) {
    // in C++, saying 'struct Foo { ... }' implicitly creates
    // "typedef struct Foo Foo;" -- but then in C programs
    // it's typical to do this explicitly as well.  apparently g++
    // does what I'm about to do: permit it when the typedef names
    // the same type
    if (lang.tagsAreTypes && prev->equals(type)) {
      // allow it, like g++ does
      return;
    }
    else {
      errThrow(stringc <<
        "conflicting typedef for `" << name <<
        "' as type `" << type->toCString() <<
        "'; previous type was `" << prev->toCString() <<
        "'");
    }
  }
  typedefs.add(name, const_cast<Type*>(type));
}


Type const *Env::getTypedef(StringRef name)
{
  Type *t;
  if (typedefs.query(name, t)) {
    return t;
  }
  else {
    return NULL;
  }
}


// ----------------------- compounds -------------------
CompoundType *Env::addCompound(StringRef name, CompoundType::Keyword keyword)
{
  if (name && compounds.isMapped(name)) {
    errThrow(stringc << "compound already declared: " << name);
  }

  CompoundType *ret = new CompoundType(keyword, name);
  //grabAtomic(ret);
  if (name) {
    compounds.add(name, ret);
  }

  return ret;
}


void Env::addCompoundField(CompoundType *ct, Variable *decl)
{
  if (ct->getNamedField(decl->name)) {
    errThrow(stringc << "field already declared: " << decl->name);
  }

  ct->addField(decl->name, decl->type, decl);
  decl->setFlag(DF_MEMBER);
}


CompoundType *Env::getCompound(StringRef name)
{
  CompoundType *e;
  if (name && compounds.query(name, e)) {
    return e;
  }
  else {
    return NULL;
  }
}


CompoundType *Env::getOrAddCompound(StringRef name, CompoundType::Keyword keyword)
{
  CompoundType *ret = getCompound(name);
  if (!ret) {
    return addCompound(name, keyword);
  }
  else {
    if (ret->keyword != keyword) {
      errThrow(stringc << "keyword mismatch for compound " << name);
    }
    return ret;
  }
}


// ---------------------- enums ---------------------
EnumType *Env::addEnum(StringRef name)
{
  if (name && enums.isMapped(name)) {
    errThrow(stringc << "enum already declared: " << name);
  }

  EnumType *ret = new EnumType(name);
  if (name) {
    enums.add(name, ret);
  }
  return ret;
}


EnumType *Env::getEnum(StringRef name)
{
  EnumType *ret;
  if (name && enums.query(name, ret)) {
    return ret;
  }
  else {
    return NULL;
  }
}


EnumType *Env::getOrAddEnum(StringRef name)
{
  EnumType *ret = getEnum(name);
  if (!ret) {
    return addEnum(name);
  }
  else {
    return ret;
  }
}


// ------------------ enumerators ---------------------
EnumType::Value *Env::addEnumerator(StringRef name, EnumType *et, int value,
                                    Variable *decl)
{
  if (enumerators.isMapped(name)) {
    errThrow(stringc << "duplicate enumerator: " << name);
  }

  EnumType::Value *ret = et->addValue(name, value, decl);
  enumerators.add(name, ret);
  return ret;
}


EnumType::Value *Env::getEnumerator(StringRef name)
{
  EnumType::Value *ret;
  if (enumerators.query(name, ret)) {
    return ret;
  }
  else {
    return NULL;
  }
}


// -------------------- type construction ------------------
CVAtomicType *Env::makeType(AtomicType const *atomic)
{
  return makeCVType(atomic, CV_NONE);
}


CVAtomicType *Env::makeCVType(AtomicType const *atomic, CVFlags cv)
{
  CVAtomicType *ret = new CVAtomicType(atomic, cv);
  grab(ret);
  return ret;
}


Type const *Env::applyCVToType(CVFlags cv, Type const *baseType)
{
  if (baseType->isError()) {
    return baseType;
  }

  if (cv == CV_NONE) {
    // keep what we've got
    return baseType;
  }

  // the idea is we're trying to apply 'cv' to 'baseType'; for
  // example, we could have gotten baseType like
  //   typedef unsigned char byte;     // baseType == unsigned char
  // and want to apply const:
  //   byte const b;                   // cv = CV_CONST
  // yielding final type
  //   unsigned char const             // return value from this fn

  // first, check for special cases
  switch (baseType->getTag()) {
    case Type::T_ATOMIC: {
      CVAtomicType const &atomic = baseType->asCVAtomicTypeC();
      if ((atomic.cv | cv) == atomic.cv) {
        // the given type already contains 'cv' as a subset,
        // so no modification is necessary
        return baseType;
      }
      else {
        // we have to add another CV, so that means creating
        // a new CVAtomicType with the same AtomicType as 'baseType'
        CVAtomicType *ret = new CVAtomicType(atomic);
        grab(ret);

        // but with the new flags added
        ret->cv = (CVFlags)(ret->cv | cv);

        return ret;
      }
      break;
    }

    case Type::T_POINTER: {
      // logic here is nearly identical to the T_ATOMIC case
      PointerType const &ptr = baseType->asPointerTypeC();
      if (ptr.op == PO_REFERENCE) {
        return NULL;     // can't apply CV to references
      }
      if ((ptr.cv | cv) == ptr.cv) {
        return baseType;
      }
      else {
        PointerType *ret = new PointerType(ptr);
        grab(ret);
        ret->cv = (CVFlags)(ret->cv | cv);
        return ret;
      }
      break;
    }

    default:    // silence warning
    case Type::T_FUNCTION:
    case Type::T_ARRAY:
      // can't apply CV to either of these (function has CV, but
      // can't get it after the fact)
      return NULL;
  }
}


ArrayType const *Env::setArraySize(ArrayType const *type, int size)
{
  ArrayType *ret = new ArrayType(type->eltType, size);
  grab(ret);
  return ret;
}


Type const *Env::makePtrOperType(PtrOper op, CVFlags cv, Type const *type)
{
  if (type->isError()) {
    return type;
  }

  PointerType *ret = new PointerType(op, cv, type);
  grab(ret);
  return ret;
}


FunctionType *Env::makeFunctionType(Type const *retType/*, CVFlags cv*/)
{
  FunctionType *ret = new FunctionType(retType/*, cv*/);
  grab(ret);
  return ret;
}


#if 0
FunctionType *Env::makeFunctionType_1arg(
  Type const *retType, CVFlags cv,
  Type const *arg1Type, char const *arg1Name)
{
  FunctionType *ret = makeFunctionType(retType/*, cv*/);
  ret->addParam(new Parameter(arg1Type, arg1Name));
  return ret;
}    
#endif // 0


ArrayType *Env::makeArrayType(Type const *eltType, int size)
{
  ArrayType *ret = new ArrayType(eltType, size);
  grab(ret);
  return ret;
}

ArrayType *Env::makeArrayType(Type const *eltType)
{
  ArrayType *ret = new ArrayType(eltType);
  grab(ret);
  return ret;
}


void Env::checkCoercible(Type const *src, Type const *dest)
{
  if (dest->asRval()->isOwnerPtr()) {
    // can only assign owners into owner owners (whereas it's
    // ok to assign an owner into a serf)
    if (!src->asRval()->isOwnerPtr()) {
      err(stringc
        << "cannot convert `" << src->toString()
        << "' to `" << dest->toString());
    }
  }

  // just say yes
}

Type const *Env::promoteTypes(BinaryOp op, Type const *t1, Type const *t2)
{
  // yes yes yes
  return t1;
}


// --------------------- error/warning reporting ------------------
Type const *Env::err(char const *str)
{
  cout << ::toString(currentLoc()) << ": error: " << str << endl;
  errors++;
  return fixed(ST_ERROR);
}


void Env::warn(char const *str)
{
  cout << ::toString(currentLoc()) << ": warning: " << str << endl;
  warnings++;
}


void Env::errLoc(SourceLoc loc, char const *str)
{
  pushLocation(loc);
  err(str);
  popLocation();
}

void Env::warnLoc(SourceLoc loc, char const *str)
{
  pushLocation(loc);
  warn(str);
  popLocation();
}


void Env::errThrow(char const *str)
{
  err(str);
  THROW(XError(str));
}


void Env::errIf(bool condition, char const *str)
{
  if (condition) {
    errThrow(str);
  }
}


// ------------------- translation context -------------------
Type const *Env::getCurrentRetType()
{
  return currentFunction->nameParams->var->type
           ->asFunctionTypeC().retType;
}


void Env::pushLocation(SourceLoc loc)
{
  locationStack.push(loc);
}


SourceLoc Env::currentLoc() const
{
  if (locationStack.isEmpty()) {
    return SL_UNKNOWN;      // no loc info
  }
  else {
    return locationStack.top();
  }
}


// ---------------------- debugging ---------------------
string Env::toString() const
{
  stringBuilder sb;

  // for now, just the variables
  FOREACH_OBJLIST(ScopedEnv, scopes, sc) {
    StringSObjDict<Variable>::IterC iter(sc.data()->variables);
    for (; !iter.isDone(); iter.next()) {
      sb << iter.value()->toString() << " ";
    }
  }

  return sb;
}


void Env::selfCheck() const
{}


