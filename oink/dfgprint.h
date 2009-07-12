// see License.txt for copyright and terms of use

// Data Flow Graph Printer: A sample Oink tool that shows you how to
// access the intER-procedural data flow graph, provided by the
// dataflow_* modules.  For now we just print it out.

#ifndef DFGPRINT_H
#define DFGPRINT_H

#include "oink.h"
#include "ptrmap.h"             // PtrMap
#include "dataflow_ex.h"        // DataFlowEx
#include "cc_ast_aux.h"         // LoweredASTVisitor
#include "Lib/union_find.h"     // UnionFind

// forwards in this file
class DataFlowEx_Print;

class DfgPrint : public virtual Oink {
  public:

  // a UnionFind that maps together values that are unified during
  // dataflow
  UnionFind<Value> canonValue;

  // map from a Value to a string that prints it as an expression
  PtrMap<Value, char const> valueName;

  // tor ****
  public:
  DfgPrint() : canonValue(NULL) {}
  
  // methods ****
  void uniValues_stage();
  void nameValues_stage();
  void printDfg_stage();
};

// build an equivalence relation on values that are unified during
// dataflow
class DataFlowEx_ValueUnify : public DataFlowEx {
  UnionFind<Value> &canonValue;

  public:
  explicit DataFlowEx_ValueUnify(UnionFind<Value> &canonValue0)
    : canonValue(canonValue0)
  {}

  void eDataFlow_unify(Value *src, Value *tgt, SourceLoc loc);
  void eDataFlow_refUnify(Value *src, Value *tgt, SourceLoc loc);
};

// build valueName for values of Expressions
class NameValuesVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  private:
  UnionFind<Value> &canonValue;
  PtrMap<Value, char const> &valueName;

  public:
  NameValuesVisitor(UnionFind<Value> &canonValue0,
                   PtrMap<Value, char const> &valueName0)
    : loweredVisitor(this)
    , canonValue(canonValue0)
    , valueName(valueName0)
  {}
  virtual ~NameValuesVisitor(){}

  private:
  explicit NameValuesVisitor(NameValuesVisitor &mpv); // prohibit

  public:
  void addName0(Value *t, char const *str);
  void addName(Value *t, rostring str);

  bool visitFunction(Function *obj);
  bool visitExpression(Expression *obj);
  bool visitDeclarator(Declarator *obj);
};

// print out the dataflow graph
class DataFlowEx_Print : public DataFlowEx {
  UnionFind<Value> &canonValue;
  PtrMap<Value, char const> &valueName;

  public:
  explicit DataFlowEx_Print
    (UnionFind<Value> &canonValue0,
     PtrMap<Value, char const> &valueName0)
    : canonValue(canonValue0)
    , valueName(valueName0)
  {}

  virtual char const *getValueName(Value *v);
  virtual void eDataFlow
    (Value *src, Value *tgt, SourceLoc loc,
     DataFlowKind dfk, bool mayUnrefSrc, bool cast,
     bool useMI, LibQual::polarity miPol, int fcId);
};

#endif // DFGPRINT_H
