// see License.txt for copyright and terms of use

// Allocation Tool: this tool allows one to query the way allocation
// is managed in a C program.

#ifndef ALLOCTOOL_H
#define ALLOCTOOL_H

#include "oink.h"
#include "cc_ast_aux.h"         // LoweredASTVisitor

class AllocTool : public virtual Oink {
  // tor ****
  public:
  AllocTool() {}
  
  // methods ****
  void printStackAllocAddrTaken_stage();
};

#endif // ALLOCTOOL_H
