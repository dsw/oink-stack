// see License.txt for copyright and terms of use

// Check the integrity of the AST after parsing, typechecking, and
// elaboration.  Not to be confused with elsa/integrity.h.

// We check the following:
//
// 1) The map *to* AST nodes is 1-1.
//
// 2) If requested, the map *to* Values (from AST nodes, variables,
// and other types) is 1-1.
//
// 3) Various other ad-hoc things don't occur anywhere.  One such is
// that Declarator::context is not DC_UNKNOWN

// NOTE: the places where a Variable* field is tested to see if it is
// non-NULL before its integrity() method is called were determined
// completely empirically by running tests; if you disagree with one
// (you think it should never be NULL) just turn off the "if" and
// re-run the tests to find out which test caused me to turn it on

// Future:
// Maybe we should add a check that all templates get instantiated?

#ifndef OINK_INTEGRITY_H
#define OINK_INTEGRITY_H

#include "cc_ast.h"             // AST components, etc.
#include "cc_ast_aux.h"         // LoweredASTVisitor
#include "value.h"              // Value
#include "sobjset.h"            // SObjSet

// this visitor is responsible for conducting all the integrity
// checking activities
//
// UPDATE: I don't really recall what this pass is doing, but it seems
// to be checking that the AST annotated with Types and Values is in
// fact a tree.  This was important to check back when Oink used the
// Types as its abstract values and missing a clone() call somewhere
// in Elsa meant that the annotated AST would not in fact be a tree,
// as a Type would get re-used.  Since the invention of Values, I
// don't think this pass is so necessary anymore.
//
// It doesn't seem that I am visiting Types anymore.
class IntegrityVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()
  
  bool checkAtomicTypes;        // should we check atomic types also?
  bool checkConstructedValues;  // should we check constructed values also?
  
  // set of AST nodes encountered
  SObjSet<void*> astSet;

  // sets of things encountered
  SObjSet<AtomicType const *> atomicTypeSet;
  SObjSet<Value const *> constructedValueSet;

  // set of Variable encountered
  SObjSet<Variable const *> varSet;

  IntegrityVisitor(bool checkAtomicTypes0, bool checkConstructedValues0)
    : loweredVisitor(this)
    , checkAtomicTypes(checkAtomicTypes0)
    , checkConstructedValues(checkConstructedValues0)
  {}

  virtual ~IntegrityVisitor() {}

//    void visitAst(void *obj);

  // **** AST
//    bool visitTranslationUnit(TranslationUnit *);
//    bool visitTopForm(TopForm *);
  bool visitFunction(Function *);
  bool visitMemberInit(MemberInit *);
//    bool visitDeclaration(Declaration *);
//    bool visitASTTypeId(ASTTypeId *);
  bool visitPQName(PQName *);
  bool visitTypeSpecifier(TypeSpecifier *);
//    bool visitBaseClassSpec(BaseClassSpec *);
  bool visitEnumerator(Enumerator *);
//    bool visitMemberList(MemberList *);
//    bool visitMember(Member *);
  bool visitDeclarator(Declarator *);
//    bool visitIDeclarator(IDeclarator *);
//    bool visitExceptionSpec(ExceptionSpec *);
//    bool visitStatement(Statement *);
//    bool visitCondition(Condition *);
  bool visitHandler(Handler *);
  bool visitExpression(Expression *);
//    bool visitFullExpression(FullExpression *);
//    bool visitArgExpression(ArgExpression *);
//    bool visitArgExpressionListOpt(ArgExpressionListOpt *);
  bool visitInitializer(Initializer *);

  // this wouldn't be called if it existed anyway
  bool visitTemplateDeclaration(TemplateDeclaration *);
//    bool visitTemplateParameter(TemplateParameter *);
//    bool visitTemplateArgument(TemplateArgument *);
//    bool visitNamespaceDecl(NamespaceDecl *);

  // **** type tree
  void checkIntegrity_AtomicType(AtomicType const *type);
  void checkIntegrity_ConstructedValue(Value const *type);

  void integrity_Atomic(AtomicType const *type);
  void integrity(SimpleType const *type);
  void integrity(CompoundType const *type);
  void integrity(EnumType const *type);
  void integrity(TypeVariable const *type);
  void integrity(PseudoInstantiation const *type);

  void integrity_Constructed(Value const *type);
  void integrity(CVAtomicValue const *type);
  void integrity(PointerValue const *type);
  void integrity(ReferenceValue const *type);
  void integrity(FunctionValue const *type);
  void integrity(ArrayValue const *type);
  void integrity(PointerToMemberValue const *type);

  // **** variables
  void integrity(Variable const *var);
};

#endif // OINK_INTEGRITY_H
