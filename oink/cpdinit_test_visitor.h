// see License.txt for copyright and terms of use

// A visitor for testing LibCpdInit.

#ifndef CPDINIT_TEST_VISITOR_H
#define CPDINIT_TEST_VISITOR_H

#include "cc_ast.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

class CpdInitTestVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  std::ostream &out;
  CpdInitTestVisitor(std::ostream &out0)
    : loweredVisitor(this)
    , out(out0)
  {}
  bool visitDeclarator(Declarator *decl);
  bool visitExpression(Expression *expression);
  virtual ~CpdInitTestVisitor(){}
};

#endif // CPDINIT_TEST_VISITOR_H
