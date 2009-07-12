// see License.txt for copyright and terms of use

// Control Flow Graph Printer: A sample Oink tool that shows you how
// to access the intRA-procedural control flow graph (provided by
// elsa).  For now we just print it out.

#ifndef CFGPRINT_H
#define CFGPRINT_H

#include "oink.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

class CfgPrint : public virtual Oink {
  // tor ****
  public:
  CfgPrint() {}
  
  // methods ****
  void computeCfg_stage();
  void printCfg_stage();
};

class CfgPrintVisitor : private ASTVisitor {
  public:
  LoweredASTVisitor loweredVisitor; // use this as the argument for traverse()

  CfgPrintVisitor() : loweredVisitor(this) {}
  virtual ~CfgPrintVisitor(){}

  private:
  explicit CfgPrintVisitor(CfgPrintVisitor &); // prohibit

  public:
  bool visitFunction(Function *obj);
};

#endif // CFGPRINT_H
