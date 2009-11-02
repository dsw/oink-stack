// see License.txt for copyright and terms of use

// Allocation Tool: this tool allows one to query the way allocation
// is managed in a C program.

#ifndef ALLOCTOOL_H
#define ALLOCTOOL_H

#include "oink.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

// interface for computing a predicate on variables
class VarPredicate {
public:
  virtual bool pass(Variable *var) = 0;
  virtual ~VarPredicate() {}
};

class AllocTool : public virtual Oink {
  // tor ****
  public:
  AllocTool() {}
  
  // methods ****
  void printStackAlloc_stage();
  void printStackAllocAddrTaken_stage();
  void heapifyStackAllocAddrTaken_stage();
  void verifyCrossModuleParams_stage();
  void localizeHeapAlloc_stage();
};

#endif // ALLOCTOOL_H
