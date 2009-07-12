// cexp3mrg.cc
// code to merge alternatives for cexp3ast.ast

#include "cexp3ast.ast.gen.h"    // Exp
#include "xassert.h"             // xfailure
#include "trace.h"               // trace


void Exp::incRefCt()
{
  refCt++;
  trace("refct") << "incremented refct of " << this << " to " << refCt << endl;
}

void Exp::decRefCt()
{
  xassert(refCt > 0);
  --refCt;
  trace("refct") << "decremented refct of " << this << " to " << refCt << endl;
  if (refCt == 0) {
    delete this;
  }
}


// this code is used to select between two competing interpretations
// for the same sequence of ground terminals
STATICDEF Exp *Exp::mergeAlts(Exp *_p1, Exp *_p2)
{
  E_op *p1 = _p1->ifE_op();
  E_op *p2 = _p2->ifE_op();
                    
  // the only way conflict can occur is between E_op expressions
  xassert(p1 && p2);

  // look at the operators
  if (p1->op != p2->op) {
    // they are different; keep the one with '+' at the
    // top, as this is the lower-precedence operator
    if (p1->op == '+') {
      p2->decRefCt();
      return p1;
    }
    else {
      p1->decRefCt();
      return p2;
    }
  }

  // same operators; decide using associativity:
  // for left associativity, we want the left subtree to be larger
  int nodes1 = p1->left->numNodes();
  int nodes2 = p2->left->numNodes();
  if (nodes1 != nodes2) {
    if (nodes1 > nodes2) {
      p2->decRefCt();
      return p1;
    }
    else {
      p1->decRefCt();
      return p2;
    }
  }

  // it should never be possible for two competing interpretations
  // to have the same operators and the same tree sizes
  xfailure("competing trees are too similar");
  return NULL;    // silence warning
}
