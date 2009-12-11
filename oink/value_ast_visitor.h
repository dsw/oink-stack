// see License.txt for copyright and terms of use

// An AST visitor that annotates a typechecked AST with Values.

// FIX: this entire pass exists only because Expressions used to not
// have locations.  Now that they do, abstract values can simply be
// created lazily by getAbstrValue().  This pass does a few other
// ad-hoc things that really should be eliminated, so I leave it for
// now, however this entire pass should just removed.

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
