// see License.txt for copyright and terms of use

// Allocation Tool: this tool allows one to query the way allocation
// is managed in a C program.

#ifndef XFORM_H
#define XFORM_H

#include "oink.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

// issue and record warnings
class IssuesWarnings {
private:
  bool warningIssued;

public:
  IssuesWarnings() : warningIssued(false) {}

  // print a warning to the user of a situation we can't handle and
  // record that a warning was issued
  void warn(SourceLoc, string);

  bool get_warningIssued() {return warningIssued;}
};

// interface for computing a predicate on variables
class VarPredicate {
public:
  virtual bool pass(Variable *var) = 0;
  virtual ~VarPredicate() {}
};

class Xform : public virtual Oink {
  // tor ****
  public:
  Xform() {}
  
  // methods ****
  void printStackAlloc_stage();
  void printStackAllocAddrTaken_stage();

  void heapifyStackAllocAddrTaken_stage(IssuesWarnings &warn);
  void verifyCrossModuleParams_stage();
  void localizeHeapAlloc_stage(IssuesWarnings &warn);
  void introFunCall_stage();

  void jimmy_stage();
};

#endif  // XFORM_H
