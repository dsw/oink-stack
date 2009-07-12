// cc_ast_aux.h
// dsw: stuff I would like to put into cc.ast but I can't

#ifndef CC_AST_AUX_H
#define CC_AST_AUX_H

#include "sobjset.h"            // class SObjSet
#include "macros.h"             // ENUM_BITWISE_OPS
#include "cc_ast.h"             // ASTVisitor

// Note that LoweredASTVisitor and LoweredASTVisitorHelper work
// closely together

// I can't put LoweredASTVisitor and LoweredASTVisitorHelper into
// cc.ast because the class I inherit from, ASTVisitor, is generated
// at the end of cc.ast.gen.h

// forward
class LoweredASTVisitor;

// this class helps LoweredASTVisitor turn off visitation during
// uninstantiated templated bodies
class LoweredASTVisitorHelper : public ASTVisitor {
private:     // data
  LoweredASTVisitor &loweredVisitor;

  // also visit template definitions of primaries and partials
  bool primariesAndPartials;

  // set of the (primary) TemplateInfo objects the instantiations of
  // which we have visited; prevents us from visiting them twice
  SObjSet<TemplateInfo *> primaryTemplateInfos;

public:      // funcs
  LoweredASTVisitorHelper(LoweredASTVisitor &loweredVisitor0,
                          bool primariesAndPartials0)
    : loweredVisitor(loweredVisitor0)
    , primariesAndPartials(primariesAndPartials0)
  {}

  virtual bool visitDeclarator(Declarator *decltor);
  virtual bool visitTypeSpecifier(TypeSpecifier *spec);
  virtual bool visitFunction(Function *func);

  virtual bool subvisitTS_classSpec(TS_classSpec *spec);

  void oneTempl(Variable *var0);
  void oneContainer(Variable *container);
  void oneVariable(Variable *tinfoVar);
  void visitDeclarator0(Declarator *decltor);
  void visitTypeSpecifier0(TypeSpecifier *spec);
};


// there were too many boolean arguments to the LoweredASTVisitor
// ctor; FIX: make all visitors use these uniformly; FIX: Scott has
// some scheme to make these work better as flags but I can't find an
// example just now
enum VisitorFlags {
  VF_NONE                    = 0x00,
  VF_VISIT_ELAB              = 0x01, // visit the elaborated AST field as well
  VF_VISIT_PRIM_AND_PARTIALS = 0x02, // visit the template primaries and partials as well
  VF_CHECK_IS_TREE           = 0x04, // check the AST is a tree while we are at it

  VF_ALL                     = 0x07
};
ENUM_BITWISE_OPS(VisitorFlags, VF_ALL)


// extend the visitor to traverse the lowered AST, which includes
// additional AST that has bee elaborated into the AST, such as
// instantiated templates, implicit ctors and dtors, etc.
class LoweredASTVisitor : public DelegatorASTVisitor {
private:     // data
  LoweredASTVisitorHelper helper;

  // track the current source location for the benefit of client
  // analyses
  SourceLoc loc;

  // visit elaborated AST fields such as ctorStatement, etc., but
  // other than instantiated templates
  bool visitElaboratedAST;

  // FIX: this set for marking visited ast nodes is a rather strange
  // thing to have here as DelegatorASTVisitor also has one.  This one
  // is just used to avoid visiting templatized AST twice, which I
  // think can really legitimately otherwise occur given our template
  // representation even if the AST is a tree.  The subclasses here
  // should intercept that duplication and return without calling the
  // overridden method of superclass which would otherwise fire an
  // assertion failure.
  SObjSet<void*> visitedTemplatizedASTNodes;

public:      // funcs
  LoweredASTVisitor(ASTVisitor *client0,
                    VisitorFlags flags0 = VF_CHECK_IS_TREE | VF_VISIT_ELAB)
    : DelegatorASTVisitor(client0, flags0 & VF_CHECK_IS_TREE)
    , helper(*this, flags0 & VF_VISIT_PRIM_AND_PARTIALS)
    , loc(SL_UNKNOWN)
    , visitElaboratedAST(flags0 & VF_VISIT_ELAB)
  {}
  virtual ~LoweredASTVisitor() {}

  SourceLoc getLoc() {return loc;}

  // maintain source loc; those commented out occur also below and so
  // their body is folded in there
  virtual bool visitTopForm(TopForm *obj);
  virtual bool visitPQName(PQName *obj);
  // virtual bool visitTypeSpecifier(TypeSpecifier *obj);
  virtual bool visitEnumerator(Enumerator *obj);
  virtual bool visitMember(Member *obj);
  virtual bool visitIDeclarator(IDeclarator *obj);
  // virtual bool visitStatement(Statement *obj);
  // virtual bool visitInitializer(Initializer *obj);
  virtual bool visitTemplateParameter(TemplateParameter *obj);

  virtual bool visitFullExpressionAnnot(FullExpressionAnnot *fea);
  virtual bool visitDeclarator(Declarator *decltor);
  virtual bool visitTypeSpecifier(TypeSpecifier *spec);
  virtual bool visitTemplateDeclaration(TemplateDeclaration *templDecl);
  virtual bool visitFunction(Function *func);
  virtual bool visitMemberInit(MemberInit *mInit);
  virtual bool visitStatement(Statement *stmt);
  virtual bool visitExpression(Expression *expr);
  virtual bool visitHandler(Handler *handler);
  virtual bool visitFullExpression(FullExpression *fexpr);
  virtual bool visitInitializer(Initializer *initializer);

  virtual bool subvisitTS_classSpec(TS_classSpec *spec);

  // ensure idempotency of visiting AST
  bool visitedAST(void *ast);
};


// visitor for checking that the "raw" AST is a tree; note that the
// ctor arguments given to the parent are the sometime defaults of
// DelegatorASTVisitor, but we will not rely on that and also make the
// client code more self documenting by making this separate class
// here.
class IsTreeVisitor : public DelegatorASTVisitor {
public:
  IsTreeVisitor()
    : DelegatorASTVisitor(NULL /*client*/, true /*ensureOneVisit*/)
  {}
};


// visitor for checking that the "raw" AST is a tree *and* the
// "lowered" AST is a tree; the ensureOneVisit flag of our *parent* is
// the one that matters for checking that the *lowered* tree is a
// tree; LoweredASTVisitor inherits from DelegatorASTVisitor which the
// VF_CHECK_IS_TREE flag below tells to check that the *non*-lowered
// tree is a tree
class LoweredIsTreeVisitor : private IsTreeVisitor {
public:
  // used at creation site
  LoweredASTVisitor loweredVisitor;

  LoweredIsTreeVisitor()
    : loweredVisitor(this /*client*/, VF_VISIT_ELAB | VF_CHECK_IS_TREE)
  {}
};

// Visit all the real (not a member of an uninstantiated template)
// Variables and Types in a TranslationUnit hanging directly off of
// the AST.
//
// The Variables were found by a grep that was not as generous as this
// one; it found 21 true hits and 4 false hits when I ran this one as
// a check on the implementation below.
// grep -n -e '\bVariable[a-zA-Z]*[ ]*\*' *.ast
//
// The Types were found by this grep:
// grep -n -e '\bType[a-zA-Z]*[ ]*\*' *.ast
class RealVarAndTypeASTVisitor : private ASTVisitor {
  // types
  public:
  class VariableVisitor {
    public:
    virtual ~VariableVisitor() {}
    virtual bool shouldVisitVariable(Variable *var) = 0;
    virtual void visitVariable(Variable *var) = 0;
  };
  class TypeVisitor {
    public:
    virtual ~TypeVisitor() {}
    virtual void visitType(Type *type) = 0;
    virtual void visitCompoundType(CompoundType *ct) = 0;
    virtual void visitScope(Scope *scope) = 0;
  };

  // data
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  VariableVisitor *variableVisitor; // callback for vars
  TypeVisitor *typeVisitor;     // callback for types

  // tor
  public:
  explicit RealVarAndTypeASTVisitor(VariableVisitor *variableVisitor0 = NULL)
    : loweredVisitor(this)
    , variableVisitor(variableVisitor0)
    , typeVisitor(NULL)
  {}
  explicit RealVarAndTypeASTVisitor(TypeVisitor *typeVisitor0)
    : loweredVisitor(this)
    , variableVisitor(NULL)
    , typeVisitor(typeVisitor0)
  {}
  explicit RealVarAndTypeASTVisitor(VariableVisitor *variableVisitor0, TypeVisitor *typeVisitor0)
    : loweredVisitor(this)
    , variableVisitor(variableVisitor0)
    , typeVisitor(typeVisitor0)
  {}
  private:
  RealVarAndTypeASTVisitor(RealVarAndTypeASTVisitor &other); // prohibit

  // methods
  public:
  virtual void visitVariable(Variable *var);
  virtual void visitType(Type *type);

  virtual bool visitTranslationUnit(TranslationUnit *obj);
  virtual bool visitFunction(Function *obj);
  virtual bool visitPQName(PQName *obj);
  virtual bool visitHandler(Handler *obj);
  virtual bool visitExpression(Expression *obj);
  virtual bool visitMemberInit(MemberInit *obj);
  virtual bool visitTypeSpecifier(TypeSpecifier *obj);
  virtual bool visitEnumerator(Enumerator *obj);
  virtual bool visitDeclarator(Declarator *obj);
  virtual bool visitInitializer(Initializer *obj);
  virtual bool visitTemplateParameter(TemplateParameter *obj);
#ifdef GNU_EXTENSION
  virtual bool visitASTTypeof(ASTTypeof *obj);
#endif // GNU_EXTENSION
};

class ReachableVarsTypePred : public TypePred {
  // data
  RealVarAndTypeASTVisitor::VariableVisitor &variableVisitor;
  // This is a bit deceptive: it is only for visiting scopes.
  RealVarAndTypeASTVisitor::TypeVisitor &typeVisitor;
//   SObjSet<CompoundType*> &seenCpdTypes;

  // tor
  public:
  explicit ReachableVarsTypePred
    (RealVarAndTypeASTVisitor::VariableVisitor &variableVisitor0
//      , SObjSet<CompoundType*> &seenCpdTypes0
     , RealVarAndTypeASTVisitor::TypeVisitor &typeVisitor0
     )
    : variableVisitor(variableVisitor0)
    , typeVisitor(typeVisitor0)
//     , seenCpdTypes(seenCpdTypes0)
  {}
  virtual ~ReachableVarsTypePred() {}

  // methods
  virtual bool operator() (Type const *t);
};

class ReachableVarsTypeVisitor : public RealVarAndTypeASTVisitor::TypeVisitor {
  // data
  public:
  RealVarAndTypeASTVisitor::VariableVisitor *variableVisitor;
  SObjSet<Type*> seenTypes;
  // Careful!  Since CompoundType inherits from Scope, we must use a
  // different set as the visitation of a CompoundType as a Scope
  // differs from its visitation as a CompoundType.
  SObjSet<Scope*> seenScopes;
//   SObjSet<CompoundType*> seenCpdTypes; // re-used across anyCtorSatisfies visitations

  // tor
  public:
  explicit ReachableVarsTypeVisitor(RealVarAndTypeASTVisitor::VariableVisitor *variableVisitor0)
    : variableVisitor(variableVisitor0)
  {}
  virtual ~ReachableVarsTypeVisitor() {}

  // methods
  virtual void visitType(Type *type);
  virtual void visitCompoundType(CompoundType *ct);
  // FIX: should this be in its own visitor?
  virtual void visitScope(Scope *scope);
  virtual void visitTypeIdem(Type *type) {}; // only visits each Type once
};

class ReachableVarsVariableVisitor : public RealVarAndTypeASTVisitor::VariableVisitor {
  // data
  public:
  RealVarAndTypeASTVisitor::TypeVisitor *typeVisitor;
  SObjSet<Variable*> seenVariables;

  // tor
  public:
  explicit ReachableVarsVariableVisitor(RealVarAndTypeASTVisitor::TypeVisitor *typeVisitor0)
    : typeVisitor(typeVisitor0)
  {}
  virtual ~ReachableVarsVariableVisitor() {}

  // methods
  virtual bool shouldVisitVariable(Variable *var);
  virtual void visitVariable(Variable *var);
  virtual void visitVariableIdem(Variable *var) {}; // only visits each Variable once
};

// visit all the real vars
class VisitRealVars : public ReachableVarsVariableVisitor {
  public:
  // type
  typedef void visitVarFunc_t(Variable *);

  // data
  ReachableVarsTypeVisitor doNothing_tv; // a placeholder
  visitVarFunc_t *visitVarFunc;

  // tor
  explicit VisitRealVars(visitVarFunc_t *visitVarFunc0)
    : ReachableVarsVariableVisitor(&doNothing_tv)
    , doNothing_tv(this)
    , visitVarFunc(visitVarFunc0)
  {}

  // methods
  virtual void visitVariableIdem(Variable *var); // only visits each Variable once
};

// mark reachable vars as real; NOTE: do NOT make this inherit from
// VisitRealVars_filter as we want to mark all real vars as real.
class MarkRealVars : public VisitRealVars {
  // tor
  public:
  explicit MarkRealVars()
    // instead of supplying a visitVarFunc, we override
    // visitVariableIdem, hence the NULL here
    : VisitRealVars(NULL)
  {}
  // methods
  virtual void visitVariableIdem(Variable *var); // only visits each Variable once
};

// Visit vars (whether real or not)
void visitVarsF(ArrayStack<Variable*> &builtinVars, VisitRealVars &visitReal);

// Visit vars that have been marked real
void visitVarsMarkedRealF(ArrayStack<Variable*> &builtinVars, VisitRealVars &visitReal);

// Visit the AST.  This visitation does not depend on real markings and
// therefore is also used to define real markings.
void visitRealVarsF(TranslationUnit *tunit, VisitRealVars &visitReal);

#endif // CC_AST_AUX_H
