// cc2c.cc
// code for cc2c.h

#include "cc2c.h"            // this module

// Elsa
#include "ia64mangle.h"      // ia64MangleType

// smbase
#include "exc.h"             // xunimp
#include "macros.h"          // Restorer


// for now
SourceLoc const SL_GENERATED = SL_UNKNOWN;


// set/restore the current compound statement
#define SET_CUR_COMPOUND_STMT(newValue) \
  Restorer<S_compound*> restore(env.curCompoundStmt, (newValue)) /* user ; */


// define an S_compound to trap generated statements
#define TRAP_SIDE_EFFECTS_STMT(name)              \
  S_compound name(SL_GENERATED, NULL /*stmts*/);  \
  SET_CUR_COMPOUND_STMT(&name);



// ------------- CC2CEnv::TypedefEntry --------------
STATICDEF Type const *CC2CEnv::TypedefEntry::getType(TypedefEntry *entry)
{
  return entry->type;
}


// -------------------- CC2CEnv ---------------------
CC2CEnv::CC2CEnv(StringTable &s)
  : str(s),
    typedefs(&TypedefEntry::getType,
             &Type::hashType,
             &Type::equalTypes),
    compoundTypes(),
    curCompoundStmt(NULL),
    dest(new TranslationUnit(NULL /*topForms*/))
{}


CC2CEnv::~CC2CEnv()
{
  if (dest) {
    delete dest;
  }
}


TranslationUnit *CC2CEnv::takeDest()
{
  TranslationUnit *ret = dest;
  dest = NULL;
  return ret;
}


void CC2CEnv::addTopForm(TopForm *tf)
{
  dest->topForms.append(tf);
}


void CC2CEnv::addStatement(Statement *s)
{
  xassert(curCompoundStmt);
  curCompoundStmt->stmts.append(s);
}


void CC2CEnv::addDeclaration(Declaration *d)
{
  if (curCompoundStmt) {
    addStatement(new S_decl(SL_GENERATED, d));
  }
  else {
    addTopForm(new TF_decl(SL_GENERATED, d));
  }
}


StringRef CC2CEnv::getTypeName(Type *t)
{
  TypedefEntry *entry = typedefs.get(t);
  if (entry) {
    return entry->name;
  }

  // make a new entry; the name is chosen to correspond with the
  // mangled name of the type for readability, but any unique name
  // would suffice since types are not linker-visible
  entry = new TypedefEntry;
  entry->type = t;
  entry->name = str(stringb("Type__" << ia64MangleType(t)));
  typedefs.add(t, entry);

  // Also add a declaration to the output.  This requires actually
  // describing the type using TypeSpecifiers and Declarators.  Use
  // just one nontrivial declarator at a time, recursively creating
  // typedefs to describe the inner structure.
  //
  // Each of the cases below sets 'typeSpec' and 'decl'.
  TypeSpecifier *typeSpec = NULL;
  IDeclarator *decl = NULL;

  // 'decl' will always be built on top of this.
  D_name *baseName = new D_name(SL_GENERATED, makePQ_name(entry->name));

  switch (t->getTag()) {
    case Type::T_ATOMIC: {
      typeSpec = makeTypeSpecifier(t);
      decl = baseName;
      break;
    }

    case Type::T_POINTER:
    case Type::T_REFERENCE:
      // reference and pointer both become pointer
      typeSpec = makeTypeSpecifier(t->getAtType());
      decl = new D_pointer(SL_GENERATED, t->getCVFlags(), baseName);
      break;

    case Type::T_FUNCTION: {
      FunctionType *ft = t->asFunctionType();
      typeSpec = makeTypeSpecifier(ft->retType);
      xunimp("rest of FunctionType");
      break;
    }

    case Type::T_ARRAY:
      xunimp("array");
      break;

    case Type::T_POINTERTOMEMBER:
      xunimp("ptm");
      break;

    case Type::T_DEPENDENTSIZEDARRAY:
    case Type::T_LAST_TYPE_TAG:
      // don't set 'typeSpec'; error detected below
      break;

    // there is no 'default' so that I will get a compiler warning if
    // a new tag is added
  }

  if (!typeSpec) {
    xfailure("getTypeName: bad/unhandled type tag");
  }
  xassert(decl);

  addTopForm(new TF_decl(
    SL_GENERATED,
    new Declaration(
      DF_TYPEDEF,
      typeSpec,
      FakeList<Declarator>::makeList(
        new Declarator(decl, NULL /*init*/)
      )
    )
  ));

  return entry->name;
}


StringRef CC2CEnv::getCompoundTypeName(CompoundType *ct)
{
  StringRef ret = compoundTypes.get(ct);
  if (ret) {
    return ret;
  }

  if (!ct->parentScope->isGlobalScope()) {
    xunimp("CC2CEnv::getCompoundTypeName: non-global scope");
  }

  if (ct->templateInfo()) {
    xunimp("CC2CEnv::getCompoundTypeName: non-NULL templateInfo");
  }

  if (!ct->name) {
    xunimp("CC2CEnv::getCompoundTypeName: anonymous");
  }

  xunimp("CC2CEnv::getCompoundTypeName: add a declaration");
  // also need to add to 'compoundTypes'

  return str(ct->name);
}


TypeSpecifier *CC2CEnv::makeTypeSpecifier(Type *t)
{
  if (t->isCVAtomicType()) {
    // no need for a declarator
    CVAtomicType *at = t->asCVAtomicType();
    TypeSpecifier *ret = makeAtomicTypeSpecifier(at->atomic);
    ret->cv = at->cv;
    return ret;
  }

  else {
    // need a declarator; circumvent by using a typedef
    return new TS_name(
      SL_GENERATED, 
      makePQ_name(getTypeName(t)),
      false /*typenameUsed*/
    );
  }
}


TypeSpecifier *CC2CEnv::makeAtomicTypeSpecifier(AtomicType *at)
{
  switch (at->getTag()) {
    case AtomicType::T_SIMPLE: {
      SimpleType *st = at->asSimpleType();
      
      // I assume that the target language supports all of the
      // built-in types that the source language does except for bool,
      // which is translated into 'char'.
      SimpleTypeId id = st->type;
      if (id == ST_BOOL) {
        id = ST_CHAR;
      }

      return new TS_simple(SL_GENERATED, id);
    }

    case AtomicType::T_COMPOUND: {
      CompoundType *ct = at->asCompoundType();
      return new TS_elaborated(
        SL_GENERATED,
        ct->keyword == CompoundType::K_UNION? TI_UNION : TI_STRUCT,
        makePQ_name(getCompoundTypeName(ct))
      );
    }

    case AtomicType::T_ENUM:
      xunimp("makeAtomicTypeSpecifier for enum");
      return NULL;   // placeholder
      
    case AtomicType::T_TYPEVAR:
    case AtomicType::T_PSEUDOINSTANTIATION:
    case AtomicType::T_DEPENDENTQTYPE:
    case AtomicType::T_TEMPLATETYPEVAR:
      xfailure("makeAtomicTypeSpecifier: template-related type");
      break;

    case AtomicType::NUM_TAGS:
      break;
  }

  xfailure("makeAtomicTypeSpecifier: bad/unhandled type tag");
  return NULL;   // silence warning
}


StringRef CC2CEnv::getVariableName(Variable const *v)
{
  // for now
  return str(v->name);
}


PQ_name *CC2CEnv::makeName(Variable const *v)
{
  return makePQ_name(getVariableName(v));
}


PQ_name *CC2CEnv::makePQ_name(StringRef name)
{
  return new PQ_name(SL_GENERATED, name);
}


FakeList<ASTTypeId> *CC2CEnv::makeParameterTypes(FunctionType *ft)
{
  // will build in reverse order, then fix at the end
  FakeList<ASTTypeId> *dest = FakeList<ASTTypeId>::emptyList();

  if (ft->acceptsVarargs()) {
    dest = dest->prepend(
      new ASTTypeId(
        new TS_simple(SL_GENERATED, ST_ELLIPSIS),
        new Declarator(
          new D_name(SL_GENERATED, NULL /*name*/),
          NULL /*init*/
        )
      )
    );
  }

  SFOREACH_OBJLIST(Variable, ft->params, iter) {
    Variable const *param = iter.data();
    
    dest = dest->prepend(
      new ASTTypeId(
        makeTypeSpecifier(param->type),
        new Declarator(
          new D_name(SL_GENERATED, makeName(param)),
          NULL /*init*/
        )
      )
    );
  }

  return dest->reverse();
}


// -------------------- TopForm --------------------
void TopForm::cc2c(CC2CEnv &env) const
{
  ASTSWITCHC(TopForm, this) {
    ASTCASEC(TF_decl, d) {
      d->decl->cc2c(env);


    } // end of TF_decl

    ASTNEXTC(TF_func, f) {
      env.addTopForm(new TF_func(SL_GENERATED, f->f->cc2c(env)));
    }

    ASTDEFAULTC {
      xunimp("cc2cTopForm");
    }
    
    ASTENDCASE
  }
}


// -------------------- Function -------------------
// If 's' is simply wrapping another single S_compound, then
// strip away the redundant container.
static S_compound *collapseRedudantCompounds(S_compound *s)
{
  if (s->stmts.count() == 1 &&
      s->stmts.first()->isS_compound()) {
    S_compound *ret = s->stmts.removeFirst()->asS_compound();
    delete s;
    return ret;
  }
  else {
    return s;
  }
}


Function *Function::cc2c(CC2CEnv &env) const
{
  if (inits->isNotEmpty()) {
    xunimp("member initializers");
  }

  if (handlers->isNotEmpty()) {
    xunimp("exception handlers");
  }

  // Translate the body.
  S_compound *genBody = new S_compound(SL_GENERATED, NULL /*stmts*/);
  {
    SET_CUR_COMPOUND_STMT(genBody);
    body->cc2c(env);
  }
  genBody = collapseRedudantCompounds(genBody);

  // Wrap it up in a Function object.
  Function *ret = new Function(
    dflags & (DF_STATIC | DF_EXTERN | DF_INLINE),
    env.makeTypeSpecifier(funcType->retType),
    new Declarator(
      new D_func(
        SL_GENERATED,
        new D_name(SL_GENERATED, env.makeName(nameAndParams->var)),
        env.makeParameterTypes(funcType),
        CV_NONE,
        NULL /*exnSpec*/
      ),
      NULL /*init*/
    ),
    NULL /*inits*/,
    genBody,
    NULL /*handlers*/
  );

  return ret;
}


// ----------------------- Declaration -------------------------
void Declaration::cc2c(CC2CEnv &env) const
{
  // No need to explicitly process 'spec'.  Even if it introduces
  // new types, they will be defined in the output on demand when
  // something later uses them.

  // If 'typedef' is specified, then the declarators just
  // introduce types, which we have already taken care of and
  // incorporated into the Type system during type checking.
  if (dflags & DF_TYPEDEF) {
    return;
  }

  // Generate a new declaration for each declarator.
  //
  // This is convenient for separating concerns, and necessary in
  // the case of declarations containing pointer-to-member,
  // because the PTM has to be handled separately, for example:
  //
  //   int i, C::*ptm;
  //
  // must become two declarations:
  //
  //  int i;
  //  struct ptm__C_int { ... } ptm;
  //
  FAKELIST_FOREACH(Declarator, decllist, declarator) {
    // At this point we exclusively consult the Type and Variable
    // system; syntax is irrelevant, only semantics matters.
    Variable *var = declarator->var;

    // But elaborated statements are relevant; for now, fail if
    // they are present.
    if (declarator->ctorStatement || declarator->dtorStatement) {
      xunimp("declaration with ctorStatement or dtorStatement");
    }

    // Initializer.        
    Initializer *genInit = NULL;
    if (declarator->init) {
      if (declarator->init->isIN_expr()) {
        Expression const *e = declarator->init->asIN_exprC()->e;

        TRAP_SIDE_EFFECTS_STMT(temp);
        e->cc2c(env);

        if (temp.stmts.isNotEmpty()) {
          xunimp("initializer expression with side effects");
        }
      }
      else {
        xunimp("unhandled kind of initializer");
      }
    }

    // Create the full generated declaration.
    env.addDeclaration(
      new Declaration(
        var->flags & (DF_STATIC | DF_EXTERN),
        env.makeTypeSpecifier(var->type),
        FakeList<Declarator>::makeList(
          new Declarator(
            new D_name(SL_GENERATED, env.makeName(var)),
            genInit
          )
        )
      )
    );
  }
}


// -------------------- Statement --------------------
static S_skip *makeSkip()
{
  return new S_skip(SL_GENERATED);
}


void S_skip::cc2c(CC2CEnv &env) const
{}


void S_label::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_label(SL_GENERATED, env.str(name), makeSkip()));
  s->cc2c(env);
}


void S_case::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_case(SL_GENERATED, expr->cc2cNoSideEffects(env), makeSkip()));
  s->cc2c(env);
}


void S_default::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_default(SL_GENERATED, makeSkip()));
  s->cc2c(env);
}


void S_expr::cc2c(CC2CEnv &env) const
{
  expr->cc2c(env);
}


void S_compound::cc2c(CC2CEnv &env) const
{
  S_compound *ret = new S_compound(SL_GENERATED, NULL /*stmts*/);

  {
    SET_CUR_COMPOUND_STMT(ret);

    FOREACH_ASTLIST(Statement, stmts, iter) {
      iter.data()->cc2c(env);
    }
  }
                   
  ret = collapseRedudantCompounds(ret);
  env.addStatement(ret);
}


void S_if::cc2c(CC2CEnv &env) const { xunimp("if"); }
void S_switch::cc2c(CC2CEnv &env) const { xunimp("switch"); }
void S_while::cc2c(CC2CEnv &env) const { xunimp("while"); }
void S_doWhile::cc2c(CC2CEnv &env) const { xunimp("doWhile"); }
void S_for::cc2c(CC2CEnv &env) const { xunimp("for"); }


void S_break::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_break(SL_GENERATED));
}


void S_continue::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_continue(SL_GENERATED));
}


void S_return::cc2c(CC2CEnv &env) const
{
  env.addStatement(expr?
    new S_return(SL_GENERATED, expr->cc2c(env)) :
    new S_return(SL_GENERATED, NULL));
}


void S_goto::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_goto(SL_GENERATED, env.str(target)));
}


void S_decl::cc2c(CC2CEnv &env) const
{
  decl->cc2c(env);
}


void S_try::cc2c(CC2CEnv &env) const { xunimp("try"); }


void S_asm::cc2c(CC2CEnv &env) const
{
  env.addStatement(
    new S_asm(SL_GENERATED, text->cc2c(env)->asE_stringLit()));
}


void S_namespaceDecl::cc2c(CC2CEnv &env) const 
{
  // should be able to just drop these
}


void S_computedGoto::cc2c(CC2CEnv &env) const { xunimp("computed goto"); }
void S_rangeCase::cc2c(CC2CEnv &env) const { xunimp("range case"); }
void S_function::cc2c(CC2CEnv &env) const { xunimp("S_function"); }


// ------------------- Expression --------------------
Expression *Expression::cc2cNoSideEffects(CC2CEnv &env) const
{
  // provide a place to put generated statements
  TRAP_SIDE_EFFECTS_STMT(temp);

  Expression *ret = cc2c(env);
  
  // confirm that nothing got put there
  xassert(temp.stmts.isEmpty());
  
  return ret;
}


Expression *E_boolLit::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }


Expression *E_intLit::cc2c(CC2CEnv &env) const
{
  return new E_intLit(text);
}


Expression *E_floatLit::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_stringLit::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_charLit::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_this::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }


Expression *E_variable::cc2c(CC2CEnv &env) const
{
  return new E_variable(env.makeName(var));
}


Expression *E_funCall::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_constructor::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_fieldAcc::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_sizeof::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_unary::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_effect::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_binary::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_addrOf::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_deref::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_cast::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_cond::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_sizeofType::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_assign::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_new::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_delete::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_throw::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_keywordCast::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_typeidExpr::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_typeidType::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_grouping::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_arrow::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }

Expression *E_addrOfLabel::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_gnuCond::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_alignofExpr::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_alignofType::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E___builtin_va_arg::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E___builtin_constant_p::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_compoundLit::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }
Expression *E_statement::cc2c(CC2CEnv &env) const { xunimp(""); return NULL; }


// ------------------ FullExpression -----------------
FullExpression *FullExpression::cc2c(CC2CEnv &env) const
{
  return new FullExpression(expr->cc2c(env));
}


// ------------------- entry point -------------------
TranslationUnit *cc_to_c(StringTable &str, TranslationUnit const &input)
{
  CC2CEnv env(str);

  FOREACH_ASTLIST(TopForm, input.topForms, iter) {
    iter.data()->cc2c(env);
  }

  return env.takeDest();
}


// EOF
