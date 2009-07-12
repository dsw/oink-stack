// tcheck.cc            see license.txt for copyright and terms of use
// implementation of typechecker over C ast

#include "c.ast.gen.h"      // C ast
#include "c_type.h"         // Type, AtomicType, etc.
#include "c_env.h"          // Env
#include "strutil.h"        // quoted
#include "trace.h"          // trace
#include "paths.h"          // printPaths
#include "cc_lang.h"        // CCLang

#define IN_PREDICATE(env) Restorer<bool> restorer(env.inPredicate, true)


void checkBoolean(Env &env, Type const *t, Expression const *e)
{
  // if a reference type is being used as an atomic, unwrap the reference
  t = t->asRval();

  if (t->isError() ||          // don't report errors for <error> type
      t->isIntegerType() ||
      t->isPointerType() ||
      t->isArrayType()) {    // what the hey..
    // ok
  }
  else {
    env.err(stringc << "expression " << e->toString()
                    << " has type " << t->toString()
                    << " but is used like a number or pointer");
  }
}


// verify 't' is an lval, and return the rvalue version
Type const *checkLval(Env &env, Type const *t, Expression const *e)
{
  if (!t->isError() && !t->isLval()) {
    env.err(stringc << "expression " << e->toString()
                    << " has type " << t->toString()
                    << " but is used like an lvalue");
    return fixed(ST_ERROR);
  }
  else {
    return t->asRval();
  }
}


// ------------------ TranslationUnit -----------------
void TranslationUnit::tcheck(Env &env)
{
  FOREACH_ASTLIST_NC(TopForm, topForms, iter) {
    iter.data()->tcheck(env);
  }
}


// --------------------- TopForm ---------------------
void TopForm::tcheck(Env &env)
{
  env.pushLocation(loc);
  itcheck(env);
  env.popLocation();
}


void TF_decl::itcheck(Env &env)
{
  decl->tcheck(env);
}


void TF_func::itcheck(Env &env)
{
  // we're the current function
  env.setCurrentFunction(this);

  Type const *r = retspec->tcheck(env);
  Type const *f = nameParams->tcheck(env, r, dflags);
  xassert(f->isFunctionType());

  // as a hack for my path-counting logic, make sure the
  // function doesn't end with a looping construct
  body->stmts.append(new S_skip(SL_UNKNOWN));

  // put parameters into the environment
  env.enterScope();
  {
    // dig inside the 'nameParams' to find the parameters
    // (the downcast will succeed because isFunctionType succeeded, above)
    D_func *fdecl = nameParams->decl->asD_func();

    FOREACH_ASTLIST(ASTTypeId, fdecl->params, iter) {
      Variable *var = iter.data()->decl->var;
      if (var->name) {
        env.addVariable(var->name, var);
      }
      params.prepend(var);
    }
    params.reverse();
  }

  // (TODO) verify the pre/post don't have side effects, and
  // limit syntax somewhat
  {
    IN_PREDICATE(env);

    // make bindings for precondition logic variables (they would
    // have been added to the environment when typechecked above, 
    // but then we closed that scope (why? not sure))
    FA_precondition *pre = ftype()->precondition;
    if (pre) {
      FOREACH_ASTLIST_NC(Declaration, pre->decls, iter) {
        FOREACH_ASTLIST_NC(Declarator, iter.data()->decllist, iter2) {
          Variable *var = iter2.data()->var;
          env.addVariable(var->name, var);
        }
      }

      //checkBoolean(env, pre->expr->tcheck(env), pre->expr);
    }
                            
    #if 0     // typechecking postconditions happens in D_func::tcheck
    // and the postcondition
    FA_postcondition *post = ftype()->postcondition;
    if (post) {
      env.enterScope();

      // example of egcs' failure to disambiguate C++ ctor/prototype:
      //Variable result(SL_UNKNOWN, env.strTable.add("result"),
      //                r, DF_LOGIC);

      // the postcondition has 'result' available, as the type
      // of the return value
      Variable *result = nameParams->decl->asD_func()->result;
      if (! ftype()->retType->isVoid()) {
        env.addVariable(result->name, result);
      }

      checkBoolean(env, post->expr->tcheck(env), post->expr);

      env.leaveScope();
    }
    #endif // 0
  }

  // may as well check that things are as I expect *before* I muck with them
  env.verifyFunctionEnd();

  // check the body in the new environment
  body->tcheck(env);

  // when the local are added, they are prepended
  locals.reverse();

  // clean up
  env.resolveGotos();
  env.resolveNexts(NULL /*target*/, false /*isContinue*/);
  env.leaveScope();

  // let the environment verify everything got cleaned up properly
  env.verifyFunctionEnd();

  // instrument AST with path information
  countPaths(env, this);

  if (tracingSys("printPaths")) {
    printPaths(this);
  }

  // ensure segfault if some later access occurs
  env.setCurrentFunction(NULL);
}


template <class T, class Y>      // Y is type of thing printed
void printSObjList(ostream &os, int indent, char const *label,
                   SObjList<T> const &list, Y (*map)(T const *t))
{
  ind(os, indent) << label << ":";
  SFOREACH_OBJLIST(T, list, iter) {
    os << " " << map(iter.data());
  }
  os << "\n";
}

StringRef varName(Variable const *v)
  { return v->name; }

string stmtLoc(Statement const *s)
{
  char const *fname;
  int line, col;
  sourceLocManager->decodeLineCol(s->loc, fname, line, col);

  return stringc << line << ":" << col;
}


void TF_func::printExtras(ostream &os, int indent) const
{
  printSObjList(os, indent, "params", params, varName);
  printSObjList(os, indent, "locals", locals, varName);
  printSObjList(os, indent, "globalRefs", globalRefs, varName);
  printSObjList(os, indent, "roots", roots, stmtLoc);
}


// ------------------- Declaration ----------------------
void Declaration::tcheck(Env &env)
{
  // check the declflags for sanity
  // (for now, support only a very limited set)
  {
    DeclFlags df = (DeclFlags)(dflags & DF_SOURCEFLAGS);
    if (df == DF_NONE ||
        df == DF_TYPEDEF ||
        df == DF_STATIC ||
        df == DF_EXTERN ||
        df == DF_UNIVERSAL ||
        df == DF_EXISTENTIAL ||
        df == DF_PREDICATE) {
      // ok
    }
    else {
      env.err(stringc << "unsupported dflags: " << toString(df));
      dflags = (DeclFlags)(dflags & ~DF_SOURCEFLAGS);
    }
  }

  // distinguish declarations of logic variables
  if (env.inPredicate) {
    dflags = (DeclFlags)(dflags | DF_LOGIC);
  }

  // compute the base type
  Type const *base = spec->tcheck(env);

  // apply this type to each of the declared entities
  FOREACH_ASTLIST_NC(Declarator, decllist, iter) {
    Declarator *d = iter.data();
    d->tcheck(env, base, dflags);
    
    // we just declared a local variable, if we're in a function
    TF_func *func = env.getCurrentFunction();
    if (func) {
      func->locals.prepend(d->var);
    }
  }
}


// --------------------- ASTTypeId ---------------------
Type const *ASTTypeId::tcheck(Env &env)
{
  Type const *s = spec->tcheck(env);
  return type = decl->tcheck(env, s, DF_NONE);
}


// ------------------- TypeSpecifier -------------------
Type const *TypeSpecifier::applyCV(Env &env, Type const *base)
{
  Type const *ret = env.applyCVToType(cv, base);
  env.errIf(!ret, stringc << "can't apply " << toString(cv)
                          << " to " << base->toString());

  return ret;
}


Type const *TS_name::tcheck(Env &env)
{
  Type const *base = env.getTypedef(name);
  env.errIf(!base, stringc << "unknown typedef " << name);

  return applyCV(env, base);
}


Type const *TS_simple::tcheck(Env &env)
{
  return applyCV(env, &CVAtomicType::fixed[id]);
}


Type const *TS_elaborated::tcheck(Env &env)
{
  AtomicType *ret;
  if (keyword != TI_ENUM) {
    ret = env.getOrAddCompound(name, (CompoundType::Keyword)keyword);
    env.errIf(!ret, stringc << name << " already declared differently");
  }
  else {
    ret = env.getOrAddEnum(name);
  }

  return env.makeCVType(ret, cv);
}


Type const *TS_classSpec::tcheck(Env &env)
{
  CompoundType *ct = env.getOrAddCompound(name, (CompoundType::Keyword)keyword);
  if (name) {
    env.errIf(!ct, stringc << name << " already declared differently");
    env.errIf(ct->isComplete(), stringc << name << " already declared");
  }
  else {
    xassert(ct);
  }

  // construct a Type
  xassert(cv == CV_NONE);    // I think the syntax precludes any alternative
  Type const *ret = env.makeCVType(ct, cv);

  if (env.lang.tagsAreTypes && name) {
    // C++: add an implicit typedef for the name
    env.addTypedef(name, ret);
  }

  // fill in 'ct' with its fields
  env.pushStruct(ct);      // declarations will go into 'ct'
  FOREACH_ASTLIST_NC(Declaration, members, iter) {
    iter.data()->tcheck(env);
  }
  env.popStruct();

  return ret;
}


class XNonConst : public xBase {
public:
  Expression const *subexpr;     // on which it fails to be const

public:
  XNonConst() : xBase("non-const") {}
  XNonConst(XNonConst const &obj) : xBase(obj), subexpr(obj.subexpr) {}
  ~XNonConst();
};

XNonConst::~XNonConst()
{}               


int constEval(Env &env, Expression *expr)
{
  try {
    return expr->constEval(env);
  }
  catch (XNonConst &x) {
    env.err(stringc << expr->toString() << " is not const because "
                    << x.subexpr->toString() << " is not const");
    return 1;     // arbitrary (but not 0, for array sizes)
  }
}


Type const *TS_enumSpec::tcheck(Env &env)
{
  if (name) {
    EnumType *et = env.getEnum(name);
    env.errIf(et, stringc << name << " already declared");
  }

  EnumType *et = env.addEnum(name);

  // fill in 'et' with enumerators
  int nextValue = 0;
  FOREACH_ASTLIST_NC(Enumerator, elts, iter) {
    Enumerator *e = iter.data();
    if (e->expr) {
      nextValue = constEval(env, e->expr);
    }
    
    // make a record of the name introduction
    Variable *var = new Variable(e->loc, name,
                                 new CVAtomicType(et, CV_NONE), DF_NONE);
    xassert(e->var == NULL);
    e->var = var;

    // add the value to the type, and put this enumerator into the
    // environment so subsequent enumerators can refer to its value
    env.addEnumerator(e->name, et, nextValue, var);

    // if the next enumerator doesn't specify a value, use +1
    nextValue++;
  }
  
  return env.makeCVType(et, cv);
}


// -------------------- Declarator -------------------
Type const *Declarator::tcheck(Env &env, Type const *base, DeclFlags dflags)
{
  // check the internal structure, eventually also setting 'var'
  xassert(var == NULL);
  decl->tcheck(env, base, dflags, this);
  xassert(var != NULL);

  if (init) {
    // verify the initializer is well-typed, given the type
    // of the variable it initializes
    // TODO: make the variable not visible in the init?
    init->tcheck(env, var->type);
  }

  return var->type;
}


void /*Type const * */IDeclarator::tcheck(Env &env, Type const *base,
                                          DeclFlags dflags, Declarator *declarator)
{
  FOREACH_ASTLIST(PtrOperator, stars, iter) {
    // the list is left-to-right, so the first one we encounter is
    // the one to be most immediately applied to the base type:
    //   int  * const * volatile x;
    //   ^^^  ^^^^^^^ ^^^^^^^^^^
    //   base  first    second
    base = env.makePtrOperType(PO_POINTER, iter.data()->cv, base);
  }

  // call inner function
  /*return*/ itcheck(env, base, dflags, declarator);
}


void /*Type const * */D_name::itcheck(Env &env, Type const *base,
                                      DeclFlags dflags, Declarator *declarator)
{
  trace("tcheck")
    << "found declarator name: " << (name? name : "(null)")
    << ", type is " << base->toCString() << endl;

  // construct a Variable: this is a binding introduction
  Variable *var = new Variable(loc, name, base, dflags);

  // annotate the declarator
  xassert(declarator->var == NULL);     // otherwise leak
  declarator->var = var;

  // look at the attributes
  if (attr) {
    xassert(attr->name == env.str("attr"));
    FOREACH_ASTLIST_NC(ThmprvAttr, attr->args, iter) {
      ThmprvAttr *at = iter.data();
      if (at->name == env.str("addrtaken")) {
        var->setFlag(DF_ADDRTAKEN);
        if (at->args.count() != 0) {
          env.err(stringc << "addrtaken cannot have parameters");
        }
      }
      else {
        env.err(stringc << "unknown attribute " << at->name);
      }
    }
  }

  // one way this happens is in prototypes with unnamed arguments
  if (!name) {
    // skip the following
  }
  else {
    if (dflags & DF_TYPEDEF) {
      env.addTypedef(name, var->type);
    }
    else {
      env.addVariable(name, var);
    }
  }

  //return base;
}


void /*Type const * */D_func::itcheck(Env &env, Type const *rettype,
                                      DeclFlags dflags, Declarator *declarator)
{
  // make the result variable
  result = new Variable(loc, env.str("result"), rettype, DF_LOGIC);

  FunctionType *ft = env.makeFunctionType(rettype);
  ft->result = result;

  // push a scope so the argument names aren't seen as colliding
  // with names already around
  env.enterScope();

  // build the argument types
  FOREACH_ASTLIST_NC(ASTTypeId, params, iter) {
    ASTTypeId *ti = iter.data();

    // handle "..."
    if (ti->spec->kind() == TypeSpecifier::TS_SIMPLE &&
        ti->spec->asTS_simple()->id == ST_ELLIPSIS) {
      ft->acceptsVarargs = true;
      break;
    }

    // compute the type of the parameter
    Type const *paramType = ti->tcheck(env);

    // mark the newly-created Variable structure as a parameter
    ti->decl->var->setFlag(DF_PARAMETER);

    // extract the name
    StringRef /*nullable*/ paramName = ti->decl->var->name;

    // add it to the type description
    FunctionType::Param *param =
      new FunctionType::Param(paramName, paramType, ti->decl->var);
    ft->params.prepend(param);    // build in wrong order initially..
  }

  // correct the argument order; this preserves linear performance
  ft->params.reverse();

  // pass the annotations along via the type language
  FOREACH_ASTLIST_NC(FuncAnnotation, ann, iter) {
    ASTSWITCH(FuncAnnotation, iter.data()) {
      ASTCASE(FA_precondition, pre) {
        IN_PREDICATE(env);

        // typecheck the precondition
        FOREACH_ASTLIST_NC(Declaration, pre->decls, iter) {
          iter.data()->tcheck(env);
        }
        checkBoolean(env, pre->expr->tcheck(env), pre->expr);

        if (ft->precondition) {
          env.err("function already has a precondition");
        }
        else {
          ft->precondition = pre;
        }
      }

      ASTNEXT(FA_postcondition, post) {
        IN_PREDICATE(env);

        // add the result variable to the environment, so the
        // postcondition can refer to it
        env.addVariable(result->name, result);

        // typecheck the postcondition
        checkBoolean(env, post->expr->tcheck(env), post->expr);

        if (ft->postcondition) {
          env.err("function already has a postcondition");
        }
        else {
          ft->postcondition = post;
        }
      }

      ASTENDCASED
    }
  }

  // I think leaving the scope here (and then re-creating it in
  // TF_func::tcheck) enforces proper scoping for *types* introduced
  // in the parameter declaration list.. ?
  env.leaveScope();

  // pass the constructed function type to base's tcheck so it can
  // further build upon the type
  /*return*/ base->tcheck(env, ft, dflags, declarator);
}


void /*Type const * */D_array::itcheck(Env &env, Type const *elttype,  
                                       DeclFlags dflags, Declarator *declarator)
{
  ArrayType *at;
  if (size) {
    at = env.makeArrayType(elttype, constEval(env, size));
  }
  else {
    at = env.makeArrayType(elttype);
  }

  /*return*/ base->tcheck(env, at, dflags, declarator);
}


void /*Type const * */D_bitfield::itcheck(Env &env, Type const *base, 
                                          DeclFlags dflags, Declarator *declarator)
{
  trace("tcheck")
    << "found bitfield declarator name: "
    << (name? name : "(null)") << endl;
  xfailure("bitfields not supported yet");
  //return NULL;    // silence warning
}


// ------------------- Declarator::getName ------------------------
StringRef D_name::getName() const
  { return name; }

StringRef D_func::getName() const
  { return base->getName(); }

StringRef D_array::getName() const
  { return base->getName(); }

StringRef D_bitfield::getName() const
  { return name; }


// ----------------------- Statement ---------------------
void Statement::tcheck(Env &env)
{
  env.pushLocation(loc);

  // the default actions here are suitable for most kinds of
  // statements, but there are exceptions which require special
  // treatment, and that is elaborated below

  // any pending 'next' pointers go here
  env.resolveNexts(this /*target*/, false /*continue*/);

  // my 'next' will go to whoever's (outer) tcheck is called next
  env.addPendingNext(this /*source*/);

  // do inner typecheck
  itcheck(env);
  
  env.popLocation();
}


void S_skip::itcheck(Env &env)
{}

void S_label::itcheck(Env &env)
{
  env.addLabel(name, this);
  s->tcheck(env);
}


void connectEnclosingSwitch(Env &env, Statement *stmt, char const *kind)
{
  S_switch *sw = env.getCurrentSwitch();
  if (!sw) {
    env.err(stringc << kind << " can only appear in the context of a 'switch'");
  }
  else {
    sw->cases.append(stmt);
  }
}

void S_case::itcheck(Env &env)
{
  connectEnclosingSwitch(env, this, "'case'");
  s->tcheck(env);
}

void S_caseRange::itcheck(Env &env)
{
  connectEnclosingSwitch(env, this, "'case'");
  s->tcheck(env);
}

void S_default::itcheck(Env &env)
{
  connectEnclosingSwitch(env, this, "'default'");
  s->tcheck(env);
}


void S_expr::itcheck(Env &env)
{
  expr->tcheck(env);
}

void S_compound::itcheck(Env &env)
{
  env.enterScope();
  FOREACH_ASTLIST_NC(Statement, stmts, iter) {
    iter.data()->tcheck(env);
  }
  env.leaveScope();
}

void S_if::itcheck(Env &env)
{
  checkBoolean(env, cond->tcheck(env), cond);
  thenBranch->tcheck(env);

  // any pending 'next's should not be resolved as pointing into
  // the 'else' clause, but instead as pointing at whatever follows
  // the entire 'if' statement
  env.pushNexts();

  elseBranch->tcheck(env);

  // merge current pending nexts with those saved above
  env.popNexts();
}

void S_switch::itcheck(Env &env)
{
  // existing 'break's must be postponed
  env.pushBreaks();

  // any occurrances of 'case' will be relative to this switch
  env.pushSwitch(this);
  branches->tcheck(env);
  env.popSwitch();

  // any previous 'break's will resolve to whatever comes next
  env.popBreaks();
}


void tcheckLoop(Env &env, Statement *loop, Expression *cond,
                Statement *body)
{
  // existing 'break's must be postponed
  env.pushBreaks();

  checkBoolean(env, cond->tcheck(env), cond);

  // any occurrances of 'continue' will be relative to this loop
  env.pushLoop(loop);
  body->tcheck(env);
  env.popLoop();

  // the body continues back into this loop
  env.resolveNexts(loop /*target*/, true /*continue*/);

  // any previous 'break's will resolve to whatever comes next
  env.popBreaks();

  // I want the loop's 'next' to point to what comes after; right now
  // it points at the body (if anywhere; see S_for), and this will
  // change it
  env.addPendingNext(loop /*source*/);
}

void S_while::itcheck(Env &env)
{
  tcheckLoop(env, this, cond, body);
}

void S_doWhile::itcheck(Env &env)
{
  tcheckLoop(env, this, cond, body);
}

// override the *outer* tcheck for S_for so we can handle 'init'
// correctly; it's easiest to understand how this works by looking at
// Statement::tcheck simultaneously and comparing the sequence of
// actions
void S_for::tcheck(Env &env)
{
  env.pushLocation(loc);

  // go immediately into 'init' so any pending 'next' pointers
  // point directly at the initializer; effectively, this makes the
  // system behave as if 'init' appeared syntactically just before
  // the "for" loop
  init->tcheck(env);

  // the pending 'next' from the 'init' should point at me,
  // as a non-continue edge
  env.resolveNexts(this /*target*/, false /*continue*/);

  // don't bother adding my 'next'; tcheckLoop will handle this,
  // and its action would just override anything I did here

  // check 'after'; it has no effect on CFG because it's an expression
  after->tcheck(env);

  // do the things that loops do
  tcheckLoop(env, this, cond, body);

  env.popLocation();
}


void S_for::itcheck(Env &env)
{
  xfailure("should not be called");

  #if 0    // old; wrong because CFG edge for init is wrong
  init->tcheck(env);
  after->tcheck(env);

  tcheckLoop(env, this, cond, body);
  #endif // 0
}


void S_break::itcheck(Env &env)
{
  // add myself to the list of active breaks
  env.addBreak(this);
}

void S_continue::itcheck(Env &env)
{
  Statement *loop = env.getCurrentLoop();
  if (!loop) {
    env.err("'continue' can only occur in the scope of a loop");
  }
  else {
    // take myself off the list of pending nexts
    env.clearNexts();

    // point my next at the loop
    next = makeNextPtr(loop, true /*continue*/);
  }
}

void S_return::itcheck(Env &env)
{
  // ensure my 'next' is null
  env.clearNexts();          
  xassert(next == NULL);

  Type const *rettype = env.getCurrentRetType();
  if (expr) {
    Type const *t = expr->tcheck(env);
    env.checkCoercible(t, rettype);
  }
  else {
    env.errIf(!rettype->isVoid(), "must supply return value for non-void function");
  }
}


void S_goto::itcheck(Env &env)
{
  env.addPendingGoto(target, this);
}


void S_decl::itcheck(Env &env)
{
  decl->tcheck(env);
}


void S_assert::itcheck(Env &env)
{
  IN_PREDICATE(env);

  Type const *type = expr->tcheck(env);
  checkBoolean(env, type, expr);
}

void S_assume::itcheck(Env &env)
{
  IN_PREDICATE(env);

  checkBoolean(env, expr->tcheck(env), expr);
}

void S_invariant::itcheck(Env &env)
{
  IN_PREDICATE(env);

  checkBoolean(env, expr->tcheck(env), expr);
}

void S_thmprv::itcheck(Env &env)
{
  IN_PREDICATE(env);

  s->tcheck(env);
}


// ------------------ Statement::getSuccessors ----------------
void Statement::getSuccessors(VoidList &dest, bool /*isContinue*/) const
{
  if (nextPtrStmt(next)) {
    dest.append(next);
  }
}


void S_if::getSuccessors(VoidList &dest, bool /*isContinue*/) const
{
  // the 'next' field is ignored since it always points at
  // the 'then' branch anyway

  dest.append(makeNextPtr(thenBranch, false));
  dest.append(makeNextPtr(elseBranch, false));
}


void S_switch::getSuccessors(VoidList &dest, bool /*isContinue*/) const
{
  xassert(dest.isEmpty());
  SFOREACH_OBJLIST(Statement, cases, iter) {
    dest.prepend(makeNextPtr(iter.data(), false));
  }
  dest.reverse();
}


void S_while::getSuccessors(VoidList &dest, bool isContinue) const
{
  Statement::getSuccessors(dest, isContinue);
  dest.append(makeNextPtr(body, false));
}


void S_doWhile::getSuccessors(VoidList &dest, bool isContinue) const
{
  if (isContinue) {
    // continue jumps to conditional, and can either go back
    // to the top (body) or past loop (next)
    Statement::getSuccessors(dest, isContinue);
  }

  // either way, doing the body is an option
  dest.append(makeNextPtr(body, false));
}


void S_for::getSuccessors(VoidList &dest, bool isContinue) const
{
  // though the semantics of which expressions get evaluated
  // are different depending on 'isContinue', the statement-level
  // control flow options are the same
  Statement::getSuccessors(dest, isContinue);
  dest.append(makeNextPtr(body, false));
}


string Statement::successorsToString() const
{
  VoidList succNoCont;
  getSuccessors(succNoCont, false);

  VoidList succYesCont;
  getSuccessors(succYesCont, true);

  stringBuilder sb;
  sb << "{";

  for (VoidListIter iter(succYesCont); !iter.isDone(); iter.adv()) {
    NextPtr np = iter.data();
    
    // a leading "(c)" means the successor edge is only present when
    // this node is reached via continue; a trailing "(c)" means that
    // successor is itself a continue edge; the algorithm assumes
    // that 'succYesCont' is a superset of 'succNoCont'
    Statement const *next = nextPtrStmt(np);
    sb << (succNoCont.contains(np)? " " : " (c)")
       << stmtLoc(next)
       << (nextPtrContinue(np)? "(c)" : "");
  }

  sb << " }";
  return sb;
}


string Statement::kindLocString() const
{
  return stringc << kindName() << "@" << stmtLoc(this);
}


string nextPtrString(NextPtr np)
{
  return stringc << nextPtrStmt(np)->kindLocString()
                 << (nextPtrContinue(np)? "(c)" : "");
}


// ------------------ Expression::tcheck --------------------
Type const *Expression::tcheck(Env &env)
{
  type = itcheck(env);
  
  // it's important we cound paths *after* typechecking, because
  // the path counter will rely on the fact that all subexpressions
  // have already had their paths counted
  countPaths(env, this);

  return type;
}


Type const *E_intLit::itcheck(Env &env)
{
  return fixed(ST_INT);
}


Type const *E_floatLit::itcheck(Env &env)
{
  return fixed(ST_FLOAT);
}


Type const *E_stringLit::itcheck(Env &env)
{
  return env.makePtrOperType(PO_POINTER, CV_NONE, fixed(ST_CHAR));
}


Type const *E_charLit::itcheck(Env &env)
{
  return fixed(ST_CHAR);
}


#if 0
Type const *E_structLit::itcheck(Env &env)
{
  // the initializer itself is ignored
  cout << "TODO: handle structure initializers\n";
  return stype->tcheck(env);
}
#endif // 0


// like Env::makeRefType, except we handle array types specially
Type const *makeReference(Env &env, Type const *type)
{
  // I no longer want the coercion to be implicit
  #if 0
  if (type->isArrayType()) {
    // implicit coercion to a pointer to the first element
    return env.makePtrOperType(PO_POINTER, CV_NONE,
                               type->asArrayTypeC().eltType);
  }
  else {
    // since the name of a variable is an lvalue, return a reference type
    return env.makeRefType(type);
  }
  #endif // 0

  // since the name of a variable is an lvalue, return a reference type
  return env.makeRefType(type);
}

Type const *E_variable::itcheck(Env &env)
{
  Variable *v;       // will point to the binding introduction

  if (scopeName) {
    // look up the scope qualifier; should be a typedef name (since in C++
    // all class and struct tags are automatically typedef names too)
    Type const *scopeType = env.getTypedef(scopeName);
    if (!scopeType) {
      return env.err(stringc << "unknown type in qualifier: " << scopeName);
    }

    // the type should be a struct (an enum would be conceivable, but
    // redundant..)
    CompoundType const *ct = scopeType->ifCompoundType();
    if (!ct || (ct->keyword == CompoundType::K_UNION)) {
      return env.err(stringc << "qualifier must be struct or class, in " 
                             << toString());
    }

    // look up the field the user asked for
    CompoundType::Field const *f = ct->getNamedField(name);
    if (!f) {
      return env.err(stringc << "unknown field: " << toString());
    }
    v = f->decl;
    xassert(v);    // I think this should be non-null by now
  }

  else {           // no scope qualifier
    v = env.getVariable(name);
    if (!v) {
      return env.err(stringc << "undeclared variable: " << name);
    }
  }

  // connect this name reference to its binding introduction
  var = v;

  if (v->hasFlag(DF_LOGIC) &&
      !env.inPredicate) {
    env.err(stringc << name << " cannot be referenced outside a predicate");
  }

  // if this is a global, annotate the current function to say
  // it is referenced
  if (v->hasFlag(DF_GLOBAL) || v->hasFlag(DF_MEMBER)) {
    TF_func *f = env.getCurrentFunction();
    if (f) {                          // might not be in a function
      f->globalRefs.appendUnique(v);    // append if not already present
    }
  }

  // if it's a field, then we're actually talking about its offset
  // (as long as we don't have static fields..)
  if (v->hasFlag(DF_MEMBER)) {
    return fixed(ST_INT);
  }

  // see makeReference's definition above
  return makeReference(env, v->type);
}


Type const *typeError(Env &env, Expression const *expr, 
                      Type const *type, char const *msg)
{
  if (type->isError()) {
    return type;      // no need to report error twice
  }
  else {
    return env.err(
      stringc << "expression `" << expr->toString()
              << "', type `" << type->toString()
              << "': " << msg);
  }
}


Type const *E_funCall::itcheck(Env &env)
{
  Type const *maybe = func->tcheck(env)->asRval();
  if (!maybe->isFunctionType()) {
    return typeError(env, func, maybe, "must be function type");
  }
  FunctionType const *ftype = &( maybe->asFunctionTypeC() );

  if (env.inPredicate) {
    env.errIf(!func->isE_variable(),
      "within predicates, function applications must be simple");
  }

  ObjListIter<FunctionType::Param> param(ftype->params);

  // check argument types
  FOREACH_ASTLIST_NC(Expression, args, iter) {
    Type const *atype = iter.data()->tcheck(env);
    if (!param.isDone()) {
      env.checkCoercible(atype, param.data()->type);
      param.adv();
    }
    else if (!ftype->acceptsVarargs) {
      env.err("too many arguments");
      break;    // won't check remaining arguments..
    }
    else {
      // we can only portably pass built-in types across
      // a varargs boundary
      checkBoolean(env, atype->asRval(), iter.data());
    }
  }
  if (!param.isDone()) {
    env.err("too few arguments");
  }

  return ftype->retType;
}


Type const *E_fieldAcc::itcheck(Env &env)
{
  Type const *lhstype = obj->tcheck(env);
  bool lval = lhstype->isLval();     // e.g. a structure literal is not an lval
  lhstype = lhstype->asRval();

  CompoundType const *ctype;

  // OWNER: treat all access to owner pointer fields as if they were
  // accesses to struct OwnerPtrMeta
  if (lhstype->isOwnerPtr()) {
    // get the special declaration of owner metadata
    ctype = env.getCompound(env.str("OwnerPtrMeta"));
    if (!ctype) {
      return env.err("can't find struct OwnerPtrMeta");
    }
  }
  else {
    try {
      ctype = &( lhstype->asCVAtomicTypeC().atomic->asCompoundTypeC() );
    }
    catch (...) {
      return env.err(stringc << obj->toString() << " is not a compound type");
    }
  }

  // get corresponding Field; this sets the 'field' member of E_fieldAcc
  field = ctype->getNamedField(fieldName);
  if (!field) {
    env.err(stringc << "no field named " << fieldName);
    return fixed(ST_ERROR);
  }

  // field reference is an lval if the LHS was an lval
  if (lval) {
    return makeReference(env, field->type);
  }
  else {
    if (field->type->isArrayType()) {
      env.err("I don't know how to handle array accesses inside non-lval structs");
      return fixed(ST_ERROR);
    }
    else {
      // the field's type, as a non-lvalue
      return field->type;
    }
  }
}


Type const *E_sizeof::itcheck(Env &env)
{
  size = expr->tcheck(env)->reprSize();
  return fixed(ST_INT);
}


Type const *E_unary::itcheck(Env &env)
{
  Type const *t = expr->tcheck(env)->asRval();

  // just about any built-in will do...
  checkBoolean(env, t, expr);

  // assume these unary operators to not widen their argument
  return t;
}

Type const *E_effect::itcheck(Env &env)
{
  Type const *t = checkLval(env, expr->tcheck(env), expr);
  checkBoolean(env, t, expr);

  if (env.inPredicate) {
    env.err("cannot have side effects in predicates");
  }

  return t;
}


// given 't' an array type, yield the type of a pointer to t's
// first element; also, construct a cast node and replace 'nodePtr'
// with it; 'nodePtr' should already have been typechecked
Type const *coerceArrayToPointer(Env &env, Type const *t, Expression *&nodePtr)
{
  ArrayType const &at = t->asArrayTypeC();

  // make the proper type
  t = env.makePtrType(at.eltType);

  // stick it into an ASTTypeId node; rather than construct the
  // proper syntax, I'll just set the 'type' field and leave the
  // syntax parts blank
  ASTTypeId *typeId = new ASTTypeId(NULL, NULL);
  typeId->type = t;

  // construct the cast node, and fill in its fields manually
  // instead of calling tcheck, because 'ptr' has already been
  // checked and I don't want to repeat that (I think I assume
  // it happens only once somewhere...)
  E_cast *newNode = new E_cast(typeId, nodePtr);
  newNode->type = t;
  newNode->numPaths = nodePtr->numPaths;
  nodePtr = newNode;
  
  return t;
}


Type const *E_binary::itcheck(Env &env)
{
  Type const *t1 = e1->tcheck(env)->asRval();
  if (t1->isArrayType()) {
    t1 = coerceArrayToPointer(env, t1, e1);
  }

  Type const *t2 = e2->tcheck(env)->asRval();

  if (!env.inPredicate && op >= BIN_IMPLIES) {
    // this restriction is useful because it means I don't have to
    // do path analysis for this operator
    env.err(stringc << "the " << ::toString(op)
                    << " operator can only be used in predicates");
  }

  checkBoolean(env, t1, e1);     // pointer is acceptable here..
  checkBoolean(env, t2, e2);

  if (isRelational(op) || isPredicateCombinator(op)) {
    return fixed(ST_INT);        // really want 'bool' ..
  }
  else {
    // e.g. (short,long) -> long
    return env.promoteTypes(op, t1, t2);
  }
}


Type const *E_addrOf::itcheck(Env &env)
{
  Type const *t = checkLval(env, expr->tcheck(env), expr);

  if (expr->isE_variable()) {
    // mark the variable as having its address taken
    Variable *v = expr->asE_variable()->var;
    if (!v->isGlobal()) {
      // for locals, we infer DF_ADDRTAKEN simply by scanning the
      // body of the function
      v->setFlag(DF_ADDRTAKEN);
    }
    else {
      // for globals, we require that DF_ADDRTAKEN be specified by
      // an attribute, for it to be legal to take that global's
      // address, so we have a consistent view across compilation
      // units                                 
      //
      // I allow a tracing flag to suppress this error since I want
      // to parse c.in4d with my C++ grammar, which doesn't have
      // the thmprv_attr stuff...
      if (!v->hasFlag(DF_ADDRTAKEN) &&
          !tracingSys("suppressAddrOfError")) {
        env.err(stringc << "you have to mark the global " << v->name
                        << " as thmprv_attr(addrtaken) if you take its address");
      }
    }
  }

  return env.makePtrOperType(PO_POINTER, CV_NONE, t);
}

                              
Type const *E_deref::itcheck(Env &env)
{
  Type const *t = ptr->tcheck(env)->asRval();
  if (t->isArrayType()) {
    t = coerceArrayToPointer(env, t, ptr);
  }

  if (!t->isPointer()) {
    env.err(stringc << "can only dereference pointers, not " << t->toCString());
    return fixed(ST_ERROR);
  }

  return env.makeRefType(t->asPointerTypeC().atType);
}


Type const *E_cast::itcheck(Env &env)
{
  // let's just allow any cast at all; strips lvalue-ness
  expr->tcheck(env)->asRval();
  return ctype->tcheck(env);
}


Type const *E_cond::itcheck(Env &env)
{
  checkBoolean(env, cond->tcheck(env), cond);

  Type const *t = th->tcheck(env);
  Type const *e = el->tcheck(env);

  // require both alternatives be primitive too..
  checkBoolean(env, t, th);
  checkBoolean(env, e, el);

  return env.promoteTypes(BIN_PLUS, t, e);
}


#if 0
Type const *E_gnuCond::itcheck(Env &env)
{
  Type const *c = cond->tcheck(env);
  Type const *e = el->tcheck(env);

  checkBoolean(env, c, cond);
  checkBoolean(env, e, el);

  return env.promoteTypes(BIN_PLUS, c, e);
}
#endif // 0


Type const *E_comma::itcheck(Env &env)
{
  e1->tcheck(env);
  e2->tcheck(env);

  return e2->type;
}


Type const *E_sizeofType::itcheck(Env &env)
{
  size = atype->tcheck(env)->asRval()->reprSize();
  return fixed(ST_INT);
}


Type const *E_new::itcheck(Env &env)
{
  return env.makePtrType(atype->tcheck(env));
}


Type const *E_assign::itcheck(Env &env)
{
  Type const *stype = src->tcheck(env)->asRval();
  Type const *ttype = checkLval(env, target->tcheck(env), target);

  if (env.inPredicate) {
    env.err(stringc << "cannot have side effects in predicates: " << toString());
  }

  if (op != BIN_ASSIGN) {
    // TODO: this isn't quite right.. I should first compute
    // the promotion of stype,ttype, then verify this can
    // in turn be coerced back to ttype.. (?)

    // they both need to be integers or pointers
    checkBoolean(env, stype, src);
    checkBoolean(env, ttype, target);
  }

  env.checkCoercible(stype, ttype);

  return ttype;    // not sure whether to return ttype or stype here..
}


Type const *E_quantifier::itcheck(Env &env)
{
  if (!env.inPredicate) {
    env.err("forall is only allowed inside thmprv predicates");
  }

  // add declared variables to the environment
  env.enterScope();
  FOREACH_ASTLIST_NC(Declaration, decls, iter) {
    Declaration *d = iter.data();

    // mark all these as universal/existential
    d->dflags = (DeclFlags)
      (d->dflags | (forall? DF_UNIVERSAL : DF_EXISTENTIAL));

    d->tcheck(env);
  }

  // typecheck the predicate
  Type const *type = pred->tcheck(env)->asRval();
  
  // I really want this to be an int.. in fact I want it to be
  // bool, but that type doesn't exist (yet?)
  if (!type->isSimple(ST_INT)) {
    env.err(stringc << "type of " << (forall? "forall" : "exists")
                    << " predicate should be int, not "
                    << type->toString());
  }

  // remove declared variables
  env.leaveScope();

  return type;
}


string Expression::extrasToString() const
{
  stringBuilder sb;
  sb << "paths=" << numPaths << ", type: ";
  if (type) {
    sb << type->toCString();
  }
  else {
    // for when we print a tree before/during typechecking
    sb << "(null)";
  }
  return sb;
}


// -------------------- Expression::constEval ----------------------
int Expression::xnonconst() const
{
  XNonConst x;
  x.subexpr = this;
  throw x;
}


int E_intLit::constEval(Env &env) const
{
  return i;
}

int E_charLit::constEval(Env &env) const
{
  return c;
}


int E_sizeof::constEval(Env &env) const
{
  return size;
}

int E_unary::constEval(Env &env) const
{
  int v = expr->constEval(env);
  switch (op) {
    default:         xfailure("bad op");
    case UNY_PLUS:   return v;
    case UNY_MINUS:  return -v;
    case UNY_NOT:    return !v;
    case UNY_BITNOT: return ~v;
  }
}

int E_effect::constEval(Env &env) const
{
  return xnonconst();
}


int E_binary::constEval(Env &env) const
{
  int v1 = e1->constEval(env);
  int v2 = e2->constEval(env);
  switch (op) {
    case BIN_MULT:      return v1 *  v2;
    case BIN_DIV:       return v1 /  v2;
    case BIN_MOD:       return v1 %  v2;
    case BIN_PLUS:      return v1 +  v2;
    case BIN_MINUS:     return v1 -  v2;
    case BIN_LSHIFT:    return v1 << v2;
    case BIN_RSHIFT:    return v1 >> v2;
    case BIN_LESS:      return v1 <  v2;
    case BIN_GREATER:   return v1 >  v2;
    case BIN_LESSEQ:    return v1 <= v2;
    case BIN_GREATEREQ: return v1 >= v2;
    case BIN_EQUAL:     return v1 == v2;
    case BIN_NOTEQUAL:  return v1 != v2;
    case BIN_BITAND:    return v1 &  v2;
    case BIN_BITXOR:    return v1 ^  v2;
    case BIN_BITOR:     return v1 |  v2;
    case BIN_AND:       return v1 && v2;
    case BIN_OR:        return v1 || v2;
    case BIN_IMPLIES:   return (!v1) || v2;

    default:            xfailure("bad operator");
                        return 0;   // silence warning
  }
}


int E_cast::constEval(Env &env) const
{
  return expr->constEval(env);
}


int E_sizeofType::constEval(Env &env) const
{
  return size;
}


int E_floatLit::constEval(Env &env) const { return xnonconst(); }
int E_stringLit::constEval(Env &env) const { return xnonconst(); }
//int E_structLit::constEval(Env &env) const { return xnonconst(); }
int E_variable::constEval(Env &env) const { return xnonconst(); }
int E_funCall::constEval(Env &env) const { return xnonconst(); }
int E_fieldAcc::constEval(Env &env) const { return xnonconst(); }
int E_addrOf::constEval(Env &env) const { return xnonconst(); }
int E_deref::constEval(Env &env) const { return xnonconst(); }
int E_cond::constEval(Env &env) const { return xnonconst(); }
//int E_gnuCond::constEval(Env &env) const { return xnonconst(); }
int E_comma::constEval(Env &env) const { return xnonconst(); }
int E_assign::constEval(Env &env) const { return xnonconst(); }
int E_new::constEval(Env &env) const { return xnonconst(); }
int E_quantifier::constEval(Env &env) const { return xnonconst(); }


// -------------------- Expression::toString --------------------
// TODO: these routines dramatically under-parenthesize the output..
// not sure how/if to improve the situation easily

string E_intLit::toString() const
  { return stringc << i; }
string E_floatLit::toString() const
  { return stringc << f; }
string E_stringLit::toString() const
  { return stringc << quoted(s); }
string E_charLit::toString() const
  { return stringc << "'" << c << "'"; }
//string E_structLit::toString() const
//  { return stringc << "(..some type..){ ... }"; }

string E_variable::toString() const
{
  if (!scopeName) {
    return stringc << name;
  }
  else {
    return stringc << scopeName << "::" << name;
  }
}

string E_funCall::toString() const
{
  stringBuilder sb;
  sb << func->toString() << "(";

  int count=0;
  FOREACH_ASTLIST(Expression, args, iter) {
    if (count++) {
      sb << ", ";
    }
    sb << iter.data()->toString();
  }
  sb << ")";
  return sb;
}

string E_fieldAcc::toString() const
  { return stringc << obj->toString() << "." << fieldName; }
string E_sizeof::toString() const
  { return stringc << "sizeof(" << expr->toString() << ")"; }
string E_unary::toString() const
  { return stringc << ::toString(op) << expr->toString(); }

string E_effect::toString() const
{ 
  if (isPostfix(op)) {
    return stringc << expr->toString() << ::toString(op);
  }
  else {
    return stringc << ::toString(op) << expr->toString(); 
  }
}

string E_binary::toString() const
  { return stringc << e1->toString() << ::toString(op) << e2->toString(); }
string E_addrOf::toString() const
  { return stringc << "&" << expr->toString(); }
string E_deref::toString() const
  { return stringc << "*(" << ptr->toString() << ")"; }
string E_cast::toString() const
  { return stringc << "(" << type->toCString() << ")" << expr->toString(); }
string E_cond::toString() const
  { return stringc << cond->toString() << "?" << th->toString() << ":" << el->toString(); }
//string E_gnuCond::toString() const
//  { return stringc << cond->toString() << "?:" << el->toString(); }
string E_comma::toString() const
  { return stringc << e1->toString() << ", " << e2->toString(); }
string E_sizeofType::toString() const
  { return stringc << "sizeof(..some type..)"; }

string E_new::toString() const
{
  return stringc << "new (" << type->asPointerTypeC().atType->toString() << ")";
}

string E_assign::toString() const
{
  if (op == BIN_ASSIGN) {
    return stringc << target->toString() << " = " << src->toString();
  }
  else {
    return stringc << target->toString() << " " << ::toString(op)
                   << "= " << src->toString();
  }
}

string E_quantifier::toString() const
{
  stringBuilder sb;
  sb << (forall? "thmprv_forall(" : "thmprv_exists(");

  FOREACH_ASTLIST(Declaration, decls, outer) {
    FOREACH_ASTLIST(Declarator, outer.data()->decllist, inner) {
      Variable *var = inner.data()->var;

      sb << var->type->toCString(var->name) << "; ";
    }
  }

  sb << pred->toString() << ")";
  return sb;
}


/*
  E_intLit
  E_floatLit
  E_stringLit
  E_charLit
  E_variable
  E_arrayAcc
  E_funCall
  E_fieldAcc
  E_sizeof
  E_unary 
  E_effect
  E_binary
  E_addrOf
  E_deref
  E_cast
  E_cond
  E_comma
  E_sizeofType
  E_assign
*/


// ------------------------- Initializer ---------------------------
void IN_expr::tcheck(Env &env, Type const *type)
{
  Type const *t = e->tcheck(env);
  env.checkCoercible(t, type);
}

void IN_compound::tcheck(Env &env, Type const *type)
{
  // for now, ignore labels
  
  if (type->isArrayType()) {
    ArrayType const &at = type->asArrayTypeC();

    // every element should correspond to the element type
    FOREACH_ASTLIST_NC(Initializer, inits, iter) {
      iter.data()->tcheck(env, at.eltType);
    }

    // check size restriction
    if (at.hasSize && inits.count() > at.size) {
      env.err(stringc << "initializer has " << inits.count()
                      << " elements but array only has " << at.size
                      << " elements");
    }
  }

  else if (type->isCVAtomicType() &&
           type->asCVAtomicTypeC().atomic->isCompoundType()) {
    CompoundType const &ct = type->asCVAtomicTypeC().atomic->asCompoundTypeC();
    
    if (ct.keyword == CompoundType::K_UNION) {
      env.err("I don't know how to initialize unions");
      return;
    }        

    // iterate simultanously over fields and initializers, establishing
    // correspondence
    int field = 0;
    FOREACH_ASTLIST_NC(Initializer, inits, iter) {
      if (field >= ct.numFields()) {
        env.err(stringc
          << "too many initializers; " << ct.keywordAndName()
          << " only has " << ct.numFields() << " fields, but "
          << inits.count() << " initializers are present");
        return;
      }

      CompoundType::Field const *f = ct.getNthField(field);

      // check this initializer against the field it initializes
      iter.data()->tcheck(env, f->type);
      
      field++;
    }

    // in C, it's ok to leave out initializers, since all data can
    // be initialized to 0; so we don't complain if field is still
    // less than ct.numFields()
  }

  else {
    env.err(stringc << "you can't use a compound initializer to initialize "
                    << type->toString());
  }
}
