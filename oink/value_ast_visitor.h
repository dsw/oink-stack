// see License.txt for copyright and terms of use

// An AST visitor that annotates a typechecked AST with Values.

#ifndef VALUE_VISITOR_H
#define VALUE_VISITOR_H

#include "cc_ast_aux.h"         // LoweredASTVisitor

class ValueASTVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  // ctor ****
  public:
  ValueASTVisitor()
    : loweredVisitor(this)
  {}
  virtual ~ValueASTVisitor() {}

  SourceLoc getLoc() {return loweredVisitor.getLoc();}

  virtual bool visitExpression(Expression *);
  virtual bool visitFunction(Function *);
  virtual bool visitDeclarator(Declarator *);
  virtual bool visitPQName(PQName *);
};

#endif // VALUE_VISITOR_H
