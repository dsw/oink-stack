// cc_ast_aux.cc            see license.txt for copyright and terms of use
// auxilliary code (debug printing, etc.) for cc.ast

#include "strutil.h"        // plural
#include "generic_aux.h"    // C++ AST, and genericPrintAmbiguities, etc.
#include "cc_ast_aux.h"     // class LoweredASTVisitor


// ---------------------- LoweredASTVisitorHelper ----------------------
void LoweredASTVisitorHelper::oneTempl(Variable *var0) {
  xassert(var0);
  xassert(var0->templateInfo());

  // run a sub-traversal of the AST instantiation
  if (var0->funcDefn) {
    var0->funcDefn->traverse(loweredVisitor);
  }
  if (var0->type->isCompoundType() && var0->type->asCompoundType()->syntax) {
    var0->type->asCompoundType()->syntax->traverse(loweredVisitor);
  }
}

// sm: added this layer to handle new design with specializations
// as a middle layer between primaries and instantiations
void LoweredASTVisitorHelper::oneContainer(Variable *container)
{
  TemplateInfo *ti = container->templateInfo();
  xassert(ti);

  if (ti->isCompleteSpec()) {
    // visit as template and bail
    oneTempl(container);
    return;
  }

  // visit the container itself, if desired
  if (primariesAndPartials) {
    // dsw: FIX: Given the avoidance of visiting TemplateDeclaration-s
    // below in the main visitor, I'm not entirely sure that this
    // makes sense.  Then again, if you set this flag you get what you
    // deserve.
    oneTempl(container);
  }

  // visit each instantiation
  SFOREACH_OBJLIST_NC(Variable, ti->instantiations, iter) {
    oneTempl(iter.data());
  }
}


void LoweredASTVisitorHelper::oneVariable(Variable *tinfoVar)
{
  xassert(tinfoVar);

  TemplateInfo *tinfo = tinfoVar->templateInfo();

  // skip any non-primary TemplateInfo-s
  if (!tinfo || !tinfo->isPrimary()) {
    return;
  }

  // visit a primary only once
  if (primaryTemplateInfos.contains(tinfo)) {
    return;
  }
  primaryTemplateInfos.add(tinfo);

  // look in the primary (as a container)
  oneContainer(tinfoVar);

  // look in the specializations (as containers)
  SFOREACH_OBJLIST_NC(Variable, tinfo->specializations, iter) {
    oneContainer(iter.data());
  }
}


void LoweredASTVisitorHelper::visitDeclarator0(Declarator *decltor)
{
  // visit all the template instantiations
  oneVariable(decltor->var);
}


bool LoweredASTVisitorHelper::visitDeclarator(Declarator *decltor)
{
  // this kind of check is now done in DelegatorASTVisitor
//    xassert(!loweredVisitor.visitedAST(decltor));

  // this is silly, but OO-correct
  if (!ASTVisitor::visitDeclarator(decltor)) return false;

  visitDeclarator0(decltor);

  return true;
}


void LoweredASTVisitorHelper::visitTypeSpecifier0(TypeSpecifier *spec)
{
  TS_classSpec *ts = spec->asTS_classSpec();
  if (ts->ctype) {
    // visit all the template instantiations
    oneVariable(ts->ctype->asCompoundType()->getTypedefVar());
  }
}


bool LoweredASTVisitorHelper::visitTypeSpecifier(TypeSpecifier *spec)
{
  if (spec->isTS_classSpec()) {
    return subvisitTS_classSpec(spec->asTS_classSpec());
  }
  return ASTVisitor::visitTypeSpecifier(spec);
}


bool LoweredASTVisitorHelper::subvisitTS_classSpec(TS_classSpec *spec)
{
  // ensure idempotency of templatized AST
  if (loweredVisitor.visitedAST(spec)) {
    return false;
  }

  // this is silly, but OO-correct
  if (!ASTVisitor::visitTypeSpecifier(spec)) return false;

  visitTypeSpecifier0(spec);

  return true;
}


bool LoweredASTVisitorHelper::visitFunction(Function *func)
{
  // ensure idempotency of templatized AST
  if (loweredVisitor.visitedAST(func)) {
    return false;
  }
  return ASTVisitor::visitFunction(func);
}

// ---------------------- LoweredASTVisitor ----------------------

// **** maintain source location

bool LoweredASTVisitor::visitTopForm(TopForm *obj) {
  loc = obj->loc;
  return DelegatorASTVisitor::visitTopForm(obj);
}

bool LoweredASTVisitor::visitPQName(PQName *obj) {
  loc = obj->loc;
  return DelegatorASTVisitor::visitPQName(obj);
}

// bool LoweredASTVisitor::visitTypeSpecifier(TypeSpecifier *obj) {
//   loc = obj->loc;
//   return DelegatorASTVisitor::visitTypeSpecifier(obj);
// }

bool LoweredASTVisitor::visitEnumerator(Enumerator *obj) {
  loc = obj->loc;
  return DelegatorASTVisitor::visitEnumerator(obj);
}

bool LoweredASTVisitor::visitMember(Member *obj) {
  loc = obj->loc;
  return DelegatorASTVisitor::visitMember(obj);
}

bool LoweredASTVisitor::visitIDeclarator(IDeclarator *obj) {
  loc = obj->loc;
  return DelegatorASTVisitor::visitIDeclarator(obj);
}

// bool LoweredASTVisitor::visitStatement(Statement *obj) {
//   loc = obj->loc;
//   return DelegatorASTVisitor::visitStatement(obj);
// }

// bool LoweredASTVisitor::visitInitializer(Initializer *obj) {
//   loc = obj->loc;
//   return DelegatorASTVisitor::visitInitializer(obj);
// }

bool LoweredASTVisitor::visitTemplateParameter(TemplateParameter *obj) {
  loc = obj->loc;
  return DelegatorASTVisitor::visitTemplateParameter(obj);
}

// ****

bool LoweredASTVisitor::visitFullExpressionAnnot(FullExpressionAnnot *fea)
{
  if (!DelegatorASTVisitor::visitFullExpressionAnnot(fea)) return false;

  return true;
}

// NOTE: I am trying to use visitDeclarator as a way to visit all
// variables, but it doesn't quite get them all; see
// visitTypeSpecifier below
bool LoweredASTVisitor::visitDeclarator(Declarator *decltor)
{
  // this kind of check is now done in DelegatorASTVisitor
//    xassert(!visitedAST(decltor));
  if (!DelegatorASTVisitor::visitDeclarator(decltor)) return false;

  if (visitElaboratedAST) {
    if (decltor->ctorStatement) {
      decltor->ctorStatement->traverse(*this);
    }
    if (decltor->dtorStatement) {
      decltor->dtorStatement->traverse(*this);
    }
  }

  helper.visitDeclarator0(decltor);

  return true;
}


// looks like we have to look here as well for typedef variables of
// class templates
bool LoweredASTVisitor::visitTypeSpecifier(TypeSpecifier *spec)
{
  loc = spec->loc;

  if (spec->isTS_classSpec()) {
    return subvisitTS_classSpec(spec->asTS_classSpec());
  }

  return DelegatorASTVisitor::visitTypeSpecifier(spec);
}


bool LoweredASTVisitor::subvisitTS_classSpec(TS_classSpec *spec)
{
  // ensure idempotency of templatized AST
  if (visitedAST(spec)) {
    return false;
  }
  if (!DelegatorASTVisitor::visitTypeSpecifier(spec)) return false;
  helper.visitTypeSpecifier0(spec);
  return true;
}


bool LoweredASTVisitor::visitTemplateDeclaration(TemplateDeclaration *templDecl)
{
  templDecl->traverse(helper);
  // PRUNE the walk since we start another one with the helper
  //
  // dsw: FIX: I don't know what the semantics should be if the client
  // has a visitTemplateDeclaration().  It seems to be a contradiction
  // if they do, as they want the lowered AST and that would not
  // contain uninstantiated templates.  I could call it anyway, but
  // what if it returns true?  What if they for some bizare reason
  // want to not prune at a TemplateDeclaration and are telling me
  // that by returning true?  Well, this is what we get if they simply
  // don't have any visitTemplateDeclaration() at all and we get the
  // default from the superclass.  There is no way to tell those two
  // situations apart and I want to prune it by default.
  return false;
}


bool LoweredASTVisitor::visitFunction(Function *func)
{
  // ensure idempotency of templatized AST
  if (visitedAST(func)) {
    return false;
  }

  // SPECIAL CASE: due to the way that member functions of template
  // classes are handled, sometimes Functions exist that have not been
  // tchecked; avoid them
  if (func->instButNotTchecked()) {
    return false;
  }

  if (!DelegatorASTVisitor::visitFunction(func)) return false;

  if (visitElaboratedAST) {
    if (func->dtorStatement) {
      func->dtorStatement->traverse(*this);
    }
  }

  return true;
}


bool LoweredASTVisitor::visitMemberInit(MemberInit *mInit)
{
  if (!DelegatorASTVisitor::visitMemberInit(mInit)) return false;

  if (visitElaboratedAST) {
    if (mInit->hasAnnot()) {
      mInit->getAnnot()->traverse(*this);
    }
    if (mInit->ctorStatement) {
      mInit->ctorStatement->traverse(*this);
    }
  }

  return true;
}


bool LoweredASTVisitor::visitStatement(Statement *stmt)
{
  loc = stmt->loc;

  if (!DelegatorASTVisitor::visitStatement(stmt)) return false;

  if (visitElaboratedAST) {
    if (stmt->isS_return()) {
      if (stmt->asS_return()->ctorStatement) {
        stmt->asS_return()->ctorStatement->traverse(*this);
      }
    }
  }

  return true;
}


bool LoweredASTVisitor::visitExpression(Expression *expr)
{
  if (!DelegatorASTVisitor::visitExpression(expr)) return false;

  if (visitElaboratedAST) {
    if (expr->isE_new()) {
      if (expr->asE_new()->ctorStatement) {
        expr->asE_new()->ctorStatement->traverse(*this);
      }
    } else if (expr->isE_delete()) {
      if (expr->asE_delete()->dtorStatement) {
        expr->asE_delete()->dtorStatement->traverse(*this);
      }
    } else if (expr->isE_throw()) {
      if (expr->asE_throw()->globalCtorStatement) {
        expr->asE_throw()->globalCtorStatement->traverse(*this);
      }
    } else if (expr->isE_funCall()) {
      if (expr->asE_funCall()->retObj) {
        expr->asE_funCall()->retObj->traverse(*this);
      }
    } else if (expr->isE_constructor()) {
      if (expr->asE_constructor()->retObj) {
        expr->asE_constructor()->retObj->traverse(*this);
      }
    }
  }

  return true;
}


bool LoweredASTVisitor::visitHandler(Handler *handler)
{
  if (!DelegatorASTVisitor::visitHandler(handler)) return false;

  if (visitElaboratedAST) {
    if (handler->hasAnnot()) {
      handler->getAnnot()->traverse(*this);
    }
    if (handler->localArg) {
      handler->localArg->traverse(*this);
    }
    if (handler->globalDtorStatement) {
      handler->globalDtorStatement->traverse(*this);
    }
  }

  return true;
}


bool LoweredASTVisitor::visitFullExpression(FullExpression *fexpr)
{
  if (!DelegatorASTVisitor::visitFullExpression(fexpr)) return false;

  if (fexpr->hasAnnot()) {
    fexpr->getAnnot()->traverse(*this);
  }

  return true;
}


bool LoweredASTVisitor::visitInitializer(Initializer *initializer)
{
  loc = initializer->loc;

  if (!DelegatorASTVisitor::visitInitializer(initializer)) return false;

  if (initializer->hasAnnot()) {
    initializer->getAnnot()->traverse(*this);
  }

  return true;
}


// not a visitor method; returns true iff the node has been visited
// before
bool LoweredASTVisitor::visitedAST(void *ast)
{
  if (visitedTemplatizedASTNodes.contains(ast)) {
    return true;
  } else {
    visitedTemplatizedASTNodes.add(ast);
    return false;
  }
}


// ---------------------- refersTo ----------------------
// Nominally "refers to <loc>", but with additional information
// about "using declaration" aliasing.  This is an example of how
// the aliasing complicates what used to be a simple process,
// in this case getting a unique name for an entity.  We'll see
// how much more of this I can take before I implement some global
// de-aliasing measure.
//
// Now that I'm using Env::storeVar, the AST shouldn't have any
// alias pointers in it.  But this method remains so I can do things
// like grepping through printTypedAST output for stray aliases.
//
// 1/15/04: Modified to tolerate NULL 'v' values, and to print types,
// since Daniel and I wanted to see addtional information while
// debugging a tricky oink/qual issue.  The result is more verbose but
// the extra information is probably worth it.
string refersTo(Variable *v)
{
  if (!v) {
    return "NULL";
  }

  stringBuilder sb;
  if (v->isNamespace()) {
    sb << "namespace " << v->name;
    return sb;
  }

  sb << v->toString();
  sb << ", at " << toString(v->loc);
  if (v->getUsingAlias()) {
    sb << ", alias of " << toString(v->skipAlias()->loc);
  }
  sb << stringf(" (0x%08X)", (long)v);
  return sb;
}


// TranslationUnit

// ---------------------- TopForm --------------------
void TopForm::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "TopForm", os, indent);
}


void TopForm::addAmbiguity(TopForm *alt)
{
  // this does not call 'genericAddAmbiguity' because TopForms do not
  // have 'next' fields

  // prepend 'alt' to my list
  xassert(alt->ambiguity == NULL);
  alt->ambiguity = ambiguity;
  ambiguity = alt;
}


// ---------------------- Function --------------------
void Function::printExtras(ostream &os, int indent) const
{
  if (funcType) {
    ind(os, indent) << "funcType = " << funcType->toString() << "\n";
  }
  ind(os, indent) << "receiver = " << refersTo(receiver) << "\n";

  // template with instantiations to print?
  if (isTemplate()) {
    TemplateInfo *ti = nameAndParams->var->templateInfo();
    int ct=0;
    SFOREACH_OBJLIST(Variable, ti->instantiations, iter) {
      Variable const *inst = iter.data();

      string label = stringc << "instantiation[" << ct++ << "]: "
                             << inst->templateInfo()->templateName();

      if (inst->funcDefn) {
        label = stringc << label << " (defn)";
        inst->funcDefn->debugPrint(os, indent+2, label.c_str());
      }
      else {
        ind(os, indent+2) << label << " (decl) = " << inst->toString() << "\n";
      }
    }
  }
}


SourceLoc Function::getLoc() const
{
  return nameAndParams->getLoc();
}


Function *Function::shallowClone() const
{
  Function *ret = new Function(
    dflags,
    retspec? retspec->clone() : NULL,
    nameAndParams? nameAndParams->clone() : NULL,

    // leave the init/body/handlers empty
    NULL, NULL, NULL
  );

  ret->cloneThunkSource = this;

  return ret;
}

void Function::finishClone()
{
  if (cloneThunkSource) {
    // follow the chain of thunk sources (in/t0258.cc)
    Function const *src = cloneThunkSource;
    while (src->cloneThunkSource) {
      src = src->cloneThunkSource;
    }

    inits = cloneFakeList(src->inits);
    body = src->body->clone();
    handlers = cloneFakeList(src->handlers);

    cloneThunkSource = NULL;
  }
}


bool Function::isTemplate() const
{
  if (nameAndParams->var) {
    TemplateInfo *ti = nameAndParams->var->templateInfo();
    if (ti && ti->isPrimary()) {
      return true;
    }
  }
  return false;
}


// ---------------------- MemberInit ----------------------
void MemberInit::printExtras(ostream &os, int indent) const
{
  if (member) {
    ind(os, indent) << "member: " << refersTo(member) << "\n";
  }

  if (base) {
    ind(os, indent) << "base: " << base->toString() << "\n";
  }

  if (ctorVar) {
    ind(os, indent) << "ctorVar: " << refersTo(ctorVar) << "\n";
  }
}


// Declaration

// ---------------------- ASTTypeId -----------------------
void ASTTypeId::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "ASTTypeId", os, indent);

  genericCheckNexts(this);
}

void ASTTypeId::addAmbiguity(ASTTypeId *alt)
{
  genericAddAmbiguity(this, alt);
}

void ASTTypeId::setNext(ASTTypeId *newNext)
{
  genericSetNext(this, newNext);
}


// ------------------------ PQName ------------------------
string targsToString(ObjList<STemplateArgument> const &sargs,
                     /*fakelist*/TemplateArgument const *targs)
{
  // iterate down both lists
  ObjListIter<STemplateArgument> siter(sargs);
  TemplateArgument const *titer = targs/*firstC*/;

  if (titer && titer->isTA_templateUsed()) {
    titer = titer->next;
  }

  stringBuilder sb;
  sb << "<";
  int ct=0;

  while (titer) {
    if (ct++ > 0) {
      sb << ", ";
    }

    if (!siter.isDone()) {
      // use 'siter'
      sb << siter.data()->toString();
      siter.adv();
    }
    else {
      // use 'titer'
      sb << titer->argString();
    }

    titer = titer->next;
  }

  sb << ">";
  return sb;
}


string PQName::qualifierString() const
{
  stringBuilder sb;

  PQName const *p = this;
  while (p->isPQ_qualifier()) {
    PQ_qualifier const *q = p->asPQ_qualifierC();
    sb << q->toComponentString() << "::";

    p = q->rest;
  }
  return sb;
}

stringBuilder& operator<< (stringBuilder &sb, PQName const &obj)
{
  sb << obj.toString();
  return sb;
}

string PQName::toString() const
{
  return stringc << qualifierString()
                 << getUnqualifiedNameC()->toComponentString();
}

string PQName::toString_noTemplArgs() const
{
  return stringc << qualifierString() << getName();
}


StringRef PQ_qualifier::getName() const
{
  return rest->getName();
}

StringRef PQ_name::getName() const
{
  return name;
}

StringRef PQ_operator::getName() const
{
  return fakeName;
}

StringRef PQ_template::getName() const
{
  return name;
}


string PQ_qualifier::toComponentString() const
{
  if (templArgs/*isNotEmpty*/) {
    return stringc << qualifier << targsToString(sargs, templArgs);
  }
  else if (qualifier) {
    return qualifier;
  }
  else {
    // for a NULL qualifier, don't print anything; it means
    // there was a leading "::" with no explicit qualifier,
    // and I'll use similar syntax on output
    return "";
  }
}

string PQ_name::toComponentString() const
{
  return name;
}

string PQ_operator::toComponentString() const
{
  return fakeName;
}

string PQ_template::toComponentString() const
{
  return stringc << name << targsToString(sargs, templArgs);
}


PQName const *PQName::getUnqualifiedNameC() const
{
  PQName const *p = this;
  while (p->isPQ_qualifier()) {
    p = p->asPQ_qualifierC()->rest;
  }
  return p;
}


bool PQName::templateUsed() const
{
  if (isPQ_qualifier() &&
      asPQ_qualifierC()->templArgs/*->isNotEmpty()*/ &&
      asPQ_qualifierC()->templArgs/*->firstC()*/->isTA_templateUsed()) {
    return true;
  }

  if (isPQ_template() &&
      asPQ_templateC()->templArgs/*->isNotEmpty()*/ &&
      asPQ_templateC()->templArgs/*->firstC()*/->isTA_templateUsed()) {
    return true;
  }

  return false;
}


// The setup is that 'this' and 'obj' are pointers to chains of
// ambiguous PQNames.  Each chain consists of an initial sequence
// of PQ_qualifiers, then (optionally) a final PQ_template.  Each
// list is of nonzero length (neither is NULL), and there can be
// at most one PQ_template among both lists.  So the situation
// looks something like this:
//
//            +--------------+   +--------------+   +-------------+
//     this-->| PQ_qualifier |-->| PQ_qualifier |-->| PQ_template |
//            +--------------+   +--------------+   +-------------+
//
//            +--------------+   +--------------+
//      obj-->| PQ_qualifier |-->| PQ_qualifier |-->NULL
//            +--------------+   +--------------+
//                                     ^
//                                     |
//                                    tail (assigned below)
//
// The return value is the head of a single unified list.  If there
// was a PQ_template, it of course goes at the end of the returned
// list.
PQName *PQName::mergeAmbiguous(PQName *obj)
{
  if (this->isPQ_qualifier()) {
    PQ_qualifier *thisq = this->asPQ_qualifier();

    if (obj->isPQ_qualifier()) {
      PQ_qualifier *objq = obj->asPQ_qualifier();
      PQName *tail = objq->ambiguity;

      // insert 'objq' into 'thisq' ambiguity list
      objq->ambiguity = thisq->ambiguity;
      thisq->ambiguity = objq;

      if (tail) {
        // merge with the tail
        return thisq->mergeAmbiguous(tail);
      }
      else {
        // done
        return thisq;
      }
    }

    if (thisq->ambiguity) {
      // push 'obj' further down
      thisq->ambiguity = thisq->ambiguity->mergeAmbiguous(obj);
      return thisq;
    }
    else {
      // attach 'obj' here
      xassert(obj->isPQ_template());
      thisq->ambiguity = obj;
      return thisq;
    }
  }

  // reverse the roles
  xassert(obj->isPQ_qualifier());
  return obj->mergeAmbiguous(this);
}


void PQ_qualifier::printAmbiguities(ostream &os, int indent) const
{
  PQName const *n = this;
  genericPrintAmbiguities(n, "PQName", os, indent);
}


PQName *getAmbiguity(PQName const *n)
{
  if (n->isPQ_qualifier()) {
    return n->asPQ_qualifierC()->ambiguity;
  }
  else {
    return NULL;
  }
}

void setAmbiguity(PQName *n, PQName *newAmbig)
{
  if (n->isPQ_qualifier()) {
    n->asPQ_qualifier()->ambiguity = newAmbig;
  }
  else {
    // the 'ambiguity' link is always fixed at NULL
    xassert(newAmbig == NULL);
  }
}


//  ------------------- TypeSpecifier ---------------------
void TypeSpecifier::printExtras(ostream &os, int indent) const
{
  // used to do something, now just a placeholder (could be deleted)
}


// disambiguation for cppstd 14.1 para 3
bool TypeSpecifier::canBeTypeParam() const
{
  if (isTS_elaborated() &&
      asTS_elaboratedC()->keyword == TI_CLASS) {
    return true;
  }

  // quarl 2006-06-06
  //    cppstd 14.1 para 2 "typename followed by a qualified-name denotes the
  //    type in a non-type parameter-declaration."  If the specifier is
  //    qualified, then "typename" is not used as a type param.
  if (isTS_name() &&
      asTS_nameC()->typenameUsed &&
      !asTS_nameC()->name->hasQualifiers())
  {
    return true;
  }

  return false;
}


// The issue here is that some inputs such as in/k0072.cc cause the
// cc.gr nonterminal ElaboratedOrSpecifier to be yielded more than
// once due to nondeterminism.  This creates a problem because
// applying the CV flags is a destructive action, and multi-yield and
// mutation cannot be mixed.
//
// However, it is my belief that any such case of multi-yield will
// still apply the same CV flags for all yielded contexts; that is, I
// think 'const' and 'volatile' cannot be interpreted as applying to
// different things depending on potential ambiguities.  At the
// moment, this is more a conjecture than a proven fact; we'll see.
//
// The alternative is to create a TS_cvFlags so that CV can be
// safely stacked upon any TypeSpecifier.  This idea is so appealing
// that I actually did it, only to discover:
//
// * Attaching 'cv' to TypeSpecifier is nice because it means that
// cv-ness is orthogonal to other aspects of TypeSpecifiers.  Once I
// had TS_cvFlags, I had to skip them all over the place.  Ugly, but
// probably manageable, esp. if I augment astgen to have a notion of a
// "transparent" variant that isTS_ and asTS_ usually skip.
//
// * Although I got Oink to compile, its tests did not pass b/c the
// placement of qualifiers is sensitive to placement of CV flags.  I
// think I could have pushed that change through, but I was already
// at the threshold of abandoning it due to Elsa changes.
//
// So, instead, I have this function which will ensure that the CV
// flags, if set more than once, are set consistently, thus making
// mutation and multi-yield compatible.  The technique for doing that
// will be to use a CV_UNLOCKED bit; initially 'cv' is CV_UNLOCKED.
// Setting it to anything clears CV_UNLOCKED.  Further calls need to
// be the same as the previously-set value.
//
// The reason for using CV_UNLOCKED rather than CV_LOCKED is I wanted
// the final value to be like it was before so that casual tests
// like "if (cv)" will still work.
//
void TypeSpecifier::setCVOnce(CVFlags newCV)
{                                   
  // caller should not be using CV_UNLOCKED
  xassert(!(newCV & CV_UNLOCKED));

  if (cv == CV_UNLOCKED) {
    cv = newCV;
  }
  else {
    // must be the same as previously set
    xassert(cv == newCV);
    
    // note that in/k0072.cc is one of very few testcases
    // that actually reaches this line of code
  }
}


void TS_classSpec::printExtras(ostream &os, int indent) const
{
  // template with instantiations to print?
  if (ctype) {
    TemplateInfo *ti = ctype->templateInfo();
    if (ti && (ti->isPrimary() || ti->isPartialSpec())) {
      ind(os, indent) << "instantiations:\n";
      int ct=0;
      SFOREACH_OBJLIST(Variable, ti->instantiations, iter) {
        string label = stringc << "instantiation[" << ct++ << "] ";

        CompoundType *instCT = iter.data()->type->asCompoundType();
        if (instCT->syntax) {
          label = stringc << label << "(defn) " << instCT->instName;
          instCT->syntax->debugPrint(os, indent+2, label.c_str());
        }
        else {
          ind(os, indent+2) << label << "(decl) = " << instCT->instName << "\n";
        }
      }
    }
  }
}


// ------------------- BaseClassSpec ---------------------
void BaseClassSpec::printExtras(ostream &os, int indent) const
{
  if (type) {
    ind(os, indent) << "type: " << type->toString() << "\n";
  }
}


// MemberList
// Member

// ---------------------- Enumerator ------------------
void Enumerator::printExtras(ostream &os, int indent) const
{
  if (var) {
    ind(os, indent) << "var: "
      << toString(var->flags) << (var->flags? " " : "")
      << var->toString() << "\n";
    PRINT_GENERIC(enumValue);
  }
}


// ---------------------- Declarator ---------------------------
void Declarator::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "Declarator", os, indent);

  // check 'next' fields
  for (Declarator *d = ambiguity; d != NULL; d = d->ambiguity) {
    xassert(this->next == d->next);
  }
}


void Declarator::addAmbiguity(Declarator *alt)
{
  genericAddAmbiguity(this, alt);
}

void Declarator::setNext(Declarator *newNext)
{
  genericSetNext(this, newNext);
}


PQName const *Declarator::getDeclaratorIdC() const
{
  return decl->getDeclaratorIdC();
}


void Declarator::setDeclaratorId(PQName *n)
{
  IDeclarator *d = decl;
  for(;;) {
    IDeclarator *b = d->getBase();
    if (!b) {
      break;
    }
    d = b;
  }

  if (d->isD_name()) {
    d->asD_name()->name = n;
  }
  else if (d->isD_bitfield()) {
    d->asD_bitfield()->name = n;
  }
  else {
    // getBase loop should only have stopped at D_name or D_bitfield
    xfailure("setting name of unknown base IDeclarator");
  }
}


SourceLoc Declarator::getLoc() const
{
  return decl->loc;
}


void Declarator::printExtras(ostream &os, int indent) const
{
  if (var) {
    ind(os, indent) << "var: "
      << toString(var->flags) << (var->flags? " " : "")
      << var->toString();

    if (var->overload) {
      int n = var->overload->count();
      os << " (" << n << " " << plural(n, "overloading") << ")";
    }

    os << "\n";
  }

  ind(os, indent) << "context = " << toString(context) << "\n";
}


// --------------------- IDeclarator ---------------------------
PQName const *D_name::getDeclaratorIdC() const
{
  return name;
}

PQName const *D_pointer::getDeclaratorIdC() const
{
  return base->getDeclaratorIdC();
}

PQName const *D_reference::getDeclaratorIdC() const
{
  return base->getDeclaratorIdC();
}

PQName const *D_func::getDeclaratorIdC() const
{
  return base->getDeclaratorIdC();
}

PQName const *D_array::getDeclaratorIdC() const
{
  return base->getDeclaratorIdC();
}

PQName const *D_bitfield::getDeclaratorIdC() const
{
  // the ability to simply return 'name' here is why bitfields contain
  // a PQName instead of just a StringRef
  return name;
}

PQName const *D_ptrToMember::getDeclaratorIdC() const
{
  return base->getDeclaratorIdC();
}

PQName const *D_grouping::getDeclaratorIdC() const
{
  return base->getDeclaratorIdC();
}


IDeclarator *IDeclarator::skipGroups()
{
  if (isD_grouping()) {
    return asD_grouping()->base->skipGroups();
  }
  else {
    return this;
  }
}


bool IDeclarator::bottomIsDfunc() const
{
  IDeclarator const *d = this;
  IDeclarator const *prev = d;     // last non-D_name, non-D_grouping declarator seen

  for (;;) {
    IDeclarator const *next = d->getBaseC();
    if (!next) {
      break;
    }

    if (!d->isD_grouping()) {
      prev = d;
    }
    d = next;
  }

  return prev->isD_func();
}


IDeclarator const *D_name::getBaseC() const            { return NULL; }
IDeclarator const *D_pointer::getBaseC() const         { return base; }
IDeclarator const *D_reference::getBaseC() const       { return base; }
IDeclarator const *D_func::getBaseC() const            { return base; }
IDeclarator const *D_array::getBaseC() const           { return base; }
IDeclarator const *D_bitfield::getBaseC() const        { return NULL; }
IDeclarator const *D_ptrToMember::getBaseC() const     { return base; }
IDeclarator const *D_grouping::getBaseC() const        { return base; }


// ExceptionSpec
// OperatorDeclarator

// ---------------------- Statement --------------------
void Statement::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "Statement", os, indent);
}


void Statement::addAmbiguity(Statement *alt)
{
  // this does not call 'genericAddAmbiguity' because Statements
  // do not have 'next' fields

  // prepend 'alt' to my list
  xassert(alt->ambiguity == NULL);
  alt->ambiguity = ambiguity;
  ambiguity = alt;
}


string Statement::lineColString() const
{
  char const *fname;
  int line, col;
  sourceLocManager->decodeLineCol(loc, fname, line, col);

  return stringc << line << ":" << col;
}

string Statement::kindLocString() const
{
  return stringc << kindName() << "@" << lineColString();
}


// ----------------------- Condition ----------------------
void Condition::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "Condition", os, indent);
}


void Condition::addAmbiguity(Condition *alt)
{
  // this does not call 'genericAddAmbiguity' because Conditions
  // do not have 'next' fields

  // prepend 'alt' to my list
  xassert(alt->ambiguity == NULL);
  alt->ambiguity = ambiguity;
  ambiguity = alt;
}


// ----------------------- Handler ----------------------
bool Handler::isEllipsis() const
{
  return typeId->spec->isTS_simple() &&
         typeId->spec->asTS_simple()->id == ST_ELLIPSIS;
}


// --------------------- Expression ---------------------
void Expression::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "Expression", os, indent);

  // old
  //genericCheckNexts(this);
}


void Expression::addAmbiguity(Expression *alt)
{
  // it turns out the RHS could have been yielded if the
  // reduction action is the identity function.. so instead
  // find the last node in the 'alt' list and we'll splice
  // that entire list into 'main's ambiguity list
  Expression *altLast = alt;
  while (altLast->ambiguity) {
    altLast = altLast->ambiguity;
  }

  // finally, prepend 'alt's ambiguity list to 'this's ambiguity list
  altLast->ambiguity = this->ambiguity;
  this->ambiguity = alt;

  #if 0     // old; from when I had lists of Expressions
  genericAddAmbiguity(this, alt);
  #endif // 0
}

#if 0     // old; from when I had lists of Expressions
void Expression::setNext(Expression *newNext)
{
  // relaxation: The syntax
  //   tok = strtok(((void *)0) , delim);
  // provokes a double-add, where 'next' is the same both
  // times.  I think this is because we merge a little
  // later than usual due to unexpected state splitting.
  // I might try to investigate this more carefully at a
  // later time, but for now..
  if (next == newNext) {
    return;    // bail if it's already what we want..
  }

  genericSetNext(this, newNext);
}
#endif // 0


void Expression::printExtras(ostream &os, int indent) const
{
  if (type) {
    ind(os, indent) << "type: " << type->toString() << "\n";
  }

  // print type-specific extras
  ASTSWITCHC(Expression, this) {
    ASTCASEC(E_intLit, i) {
      ind(os, indent) << "i: " << i->i << "\n";
    }

    ASTNEXTC(E_floatLit, f) {
      ind(os, indent) << "f: " << f->d << "\n";
    }

    ASTNEXTC(E_stringLit, s) {
      // nothing extra to print since there's no interpretation yet
      PRETEND_USED(s);
    }

    ASTNEXTC(E_charLit, c) {
      ind(os, indent) << "c: " << c->c << "\n";    // prints as an integer
    }

    ASTNEXTC(E_variable, v)
      ind(os, indent) << "var: " << refersTo(v->var) << "\n";

    ASTNEXTC(E_constructor, c)
      ind(os, indent) << "ctorVar: " << refersTo(c->ctorVar) << "\n";

    ASTNEXTC(E_new, n)
      PRINT_SUBTREE(n->arraySize);

    ASTNEXTC(E_fieldAcc, f)
      ind(os, indent) << "field: " << refersTo(f->field) << "\n";

    ASTDEFAULTC
      /* do nothing */

    ASTENDCASEC
  }
}


bool Expression::isBinary(BinaryOp op) const
{
  return isE_binary() &&
         asE_binaryC()->op == op;
}


// remove layers of parens: keep going down until the expression is
// not an E_grouping and return that
Expression *Expression::skipGroups()
{
  return const_cast<Expression*>(skipGroupsC());
}

Expression const *Expression::skipGroupsC() const
{
  Expression const *ret = this;
  while (ret->isE_grouping()) {
    ret = ret->asE_groupingC()->expr;
  }
  return ret;
}


// FullExpression

// ------------------- ArgExpression -------------------------
void ArgExpression::setNext(ArgExpression *newNext)
{
  xassert(next == NULL);
  next = newNext;
}


void ArgExpression::addAmbiguity(ArgExpression *alt)
{
  // find the end of alt's ambiguity list
  ArgExpression *altLast = alt;
  while (altLast->ambiguity) {
    altLast = altLast->ambiguity;
  }

  // finally, prepend 'alt's ambiguity list to 'this's ambiguity list
  altLast->ambiguity = this->ambiguity;
  this->ambiguity = alt;
}


void ArgExpression::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "ArgExpression", os, indent);
}


// ExpressionListOpt
// Initializer
// InitLabel
// TemplateDeclaration

// -------------------- TemplateParameter ---------------------
bool anyHaveDefaultArgs(TemplateParameter const *list)
{
  for (TemplateParameter const *iter = list; iter; iter = iter->next) {
    if (iter->hasDefaultArg()) {
      return true;
    }
  }
  return false;
}


bool TP_type::hasDefaultArg() const
{
  return !!defaultType;
}

bool TP_nontype::hasDefaultArg() const
{
  return !!param->decl->init;
}

bool TP_template::hasDefaultArg() const
{
  return !!defaultTemplate;
}


void TemplateParameter::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "TemplateParameter", os, indent);
}


void TemplateParameter::addAmbiguity(TemplateParameter *alt)
{
  xassert(alt->ambiguity == NULL);
  alt->ambiguity = this->ambiguity;
  this->ambiguity = alt;
}


// -------------------- TemplateArgument ---------------------
void TemplateArgument::printAmbiguities(ostream &os, int indent) const
{
  genericPrintAmbiguities(this, "TemplateArgument", os, indent);
}


void TemplateArgument::addAmbiguity(TemplateArgument *alt)
{
  xassert(alt->ambiguity == NULL);
  alt->ambiguity = this->ambiguity;
  this->ambiguity = alt;
}


string TA_type::argString() const
{
  return "(un-tchecked-TA_type)";
}

string TA_nontype::argString() const
{
  return expr->exprToString();
}

string TA_templateUsed::argString() const
{
  // this should not show up in, e.g., error messages, because
  // it's just a communication device between the parser and
  // the tchecker
  return "(templateUsed)";
}


// ----------------- RealVarAndTypeASTVisitor ----------------

void RealVarAndTypeASTVisitor::visitVariable(Variable *var) {
  if (variableVisitor) variableVisitor->visitVariable(var);
}

void RealVarAndTypeASTVisitor::visitType(Type *type) {
  if (typeVisitor) typeVisitor->visitType(type);
}


// **** visit methods

// class BaseClassSpec {
//   public(xml_TY) CompoundType *type = NULL;
// }

bool RealVarAndTypeASTVisitor::visitTranslationUnit(TranslationUnit *obj) {
  typeVisitor->visitScope(obj->globalScope);
  return true;
}

bool RealVarAndTypeASTVisitor::visitFunction(Function *obj) {
  visitVariable(obj->retVar);
  visitVariable(obj->receiver);
  visitType(obj->funcType);
  return true;
}

bool RealVarAndTypeASTVisitor::visitPQName(PQName *obj) {
  if (obj->isPQ_variable()) {
    visitVariable(obj->asPQ_variable()->var);
  }
//   else if (obj->isPQ_qualifier()) {
    // dsw: Ok, it seems that this is supposed to sometimes be a
    // template even for an instantiated instance (in/t0057.cc).  Is
    // it possible for this variable to 1) not be a template primary
    // and 2) also not get visited somehow else?
//     visitVariable(obj->asPQ_qualifier()->qualifierVar);
//   }

  // SGM 2007-08-25: Do not look inside PQ_template argument lists.
  // For template template parameters, the argument list may refer to
  // an uninstantiated template, but client analyses will just treat
  // the whole PQ_template as just a name; no need to look inside it.
  if (obj->isPQ_template()) {
    return false;
  }

  return true;
}

bool RealVarAndTypeASTVisitor::visitHandler(Handler *obj) {
  visitVariable(obj->globalVar);
  return true;
}

bool RealVarAndTypeASTVisitor::visitExpression(Expression *obj) {
  if (obj->isE_new()) {
    E_new *enew = obj->asE_new();
    visitVariable(enew->heapVar);
    visitVariable(enew->ctorVar);
  } else if (obj->isE_throw()) {
    visitVariable(obj->asE_throw()->globalVar);
  } else if (obj->isE_this()) {
    visitVariable(obj->asE_this()->receiver);
  } else if (obj->isE_variable()) {
    E_variable *evar = obj->asE_variable();
    visitVariable(evar->var);
    visitVariable(evar->nondependentVar);
  } else if (obj->isE_constructor()) {
    visitVariable(obj->asE_constructor()->ctorVar);
  } else if (obj->isE_fieldAcc()) {
    visitVariable(obj->asE_fieldAcc()->field);
  }
  visitType(obj->type);
  return true;
}

bool RealVarAndTypeASTVisitor::visitMemberInit(MemberInit *obj) {
  visitVariable(obj->member);
  visitVariable(obj->ctorVar);
//   public(xml_TY) CompoundType *base = NULL;
  return true;
}

bool RealVarAndTypeASTVisitor::visitTypeSpecifier(TypeSpecifier *obj) {
  if (obj->isTS_name()) {
    TS_name *tsn = obj->asTS_name();
    visitVariable(tsn->var);
    visitVariable(tsn->nondependentVar);
  }
  if (obj->isTS_type()) {
    visitType(obj->asTS_type()->type);
  }
//   -> TS_elaborated {
//        public(xml_TY) NamedAtomicType *atype = NULL;
//      }
//   -> TS_classSpec {
//        public(xml_TY) CompoundType *ctype = NULL;
//      }
//   -> TS_enumSpec {
//        public(xml_TY) EnumType *etype = NULL;
//      }
  return true;
}

bool RealVarAndTypeASTVisitor::visitEnumerator(Enumerator *obj) {
  visitVariable(obj->var);
  return true;
}

bool RealVarAndTypeASTVisitor::visitDeclarator(Declarator *obj) {
  visitVariable(obj->var);
  visitType(obj->type);
  return true;
}

bool RealVarAndTypeASTVisitor::visitInitializer(Initializer *obj) {
  if (obj->isIN_ctor()) {
    visitVariable(obj->asIN_ctor()->ctorVar);
  }
  return true;
}

bool RealVarAndTypeASTVisitor::visitTemplateParameter(TemplateParameter *obj) {
  // this one is a bit outlandish as the whole point is to avoid
  // templates, so it should probably never be called
  visitVariable(obj->var);
  return true;
}

#ifdef GNU_EXTENSION
bool RealVarAndTypeASTVisitor::visitASTTypeof(ASTTypeof *obj) {
  visitType(obj->type);
  return true;
}
#endif // GNU_EXTENSION

// ---------------- ReachableVarsTypeVisitor --------------

bool ReachableVarsTypePred::operator() (Type const *t0) {
  Type *t = const_cast<Type *>(t0);
  if (t->isFunctionType()) {
    // visit the function param variables
    SFOREACH_OBJLIST_NC(Variable, t->asFunctionType()->params, iter) {
      variableVisitor.visitVariable(iter.data());
    }
  } else if (t->isCompoundType()) {
    typeVisitor.visitCompoundType(t->asCompoundType());
  }
  return false;                 // dummy value; just means keep searching
}

void ReachableVarsTypeVisitor::visitType(Type *type) {
  if (!type) return;
  if (seenTypes.contains(type)) return;
  seenTypes.add(type);
  visitTypeIdem(type);
  ReachableVarsTypePred tPred(*variableVisitor
                              , *this
//                               , seenCpdTypes
                              );
  bool sat = type->anyCtorSatisfies(tPred);
  xassert(!sat);                // the point was just to visit anyway
}

void ReachableVarsTypeVisitor::visitCompoundType(CompoundType *ct) {
  variableVisitor->visitVariable(ct->typedefVar);
  visitScope(ct);
  for(StringRefMap<Variable>::Iter iter(ct->getVariableIter()); !iter.isDone(); iter.adv()) {
    Variable *v = iter.value();
    if (v->templateInfo() && !v->templateInfo()->isInstantiation()) continue;
    variableVisitor->visitVariable(v);
  }
  for(StringRefMap<Variable>::Iter iter(ct->getTypeTagIter()); !iter.isDone(); iter.adv()) {
    Variable *v = iter.value();
    if (v->templateInfo() && !v->templateInfo()->isInstantiation()) continue;
    variableVisitor->visitVariable(v);
  }
  FOREACH_OBJLIST(BaseClass, ct->bases, iter) {
    visitCompoundType(iter.data()->ct);
  }
  SFOREACH_OBJLIST_NC(Variable, ct->conversionOperators, iter) {
    Variable *v = iter.data();
    if (v->templateInfo() && !v->templateInfo()->isInstantiation()) continue;
    variableVisitor->visitVariable(v);
  }
  visitType(ct->selfType);
}

void ReachableVarsTypeVisitor::visitScope(Scope *scope) {
  if (!scope) return;
  // See comment at the declaration of seenScopes.
  if (seenScopes.contains(scope)) return;
  seenScopes.add(scope);
  variableVisitor->visitVariable(scope->namespaceVar);
  // FIX: If this type is a CompoundType, I think it will get visited
  // as a Type by the type visitor at some point
  visitScope(scope->parentScope);
}

bool ReachableVarsVariableVisitor::shouldVisitVariable(Variable *var) {
  return true;
}

void ReachableVarsVariableVisitor::visitVariable(Variable *var) {
  if (!var) return;
  if (seenVariables.contains(var)) return;
  seenVariables.add(var);
  xassert(!var->isTemplate() && "ee42ebc5-7154-4ace-be35-c2090a2821c5");
  xassert(!var->isUninstTemplateMember());
  if (shouldVisitVariable(var)) visitVariableIdem(var);
  typeVisitor->visitType(var->type);
  typeVisitor->visitScope(var->scope);
}

void VisitRealVars::visitVariableIdem(Variable *var) {
  xassert(var->getReal());     // if this visit is idempotent, this should always be false
  if (visitVarFunc) visitVarFunc(var);
}

void MarkRealVars::visitVariableIdem(Variable *var) {
  xassert(!var->getReal());     // if this visit is idempotent, this should always be false
  var->setReal(true);
}

void visitVarsF(ArrayStack<Variable*> &builtinVars, VisitRealVars &visitReal) {
  FOREACH_ARRAYSTACK_NC(Variable*, builtinVars, iter) {
    Variable *var = *iter.data();
    visitReal.visitVariable(var);
  }
}

void visitVarsMarkedRealF(ArrayStack<Variable*> &builtinVars, VisitRealVars &visitReal) {
  FOREACH_ARRAYSTACK_NC(Variable*, builtinVars, iter) {
    Variable *var = *iter.data();
    if (!var->getReal()) continue;
    visitReal.visitVariable(var);
  }
}

void visitRealVarsF(TranslationUnit *tunit, VisitRealVars &visitReal) {
  RealVarAndTypeASTVisitor vis(&visitReal, &visitReal.doNothing_tv);
  tunit->traverse(vis.loweredVisitor);
}

// EOF
