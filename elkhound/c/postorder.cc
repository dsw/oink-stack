// postorder.cc            see license.txt for copyright and terms of use
// given the AST for a function, compute a reverse postorder
// enumeration of all the statements

#include "c.ast.gen.h"     // C AST stuff, including decl for this module
#include "sobjset.h"       // SObjSet

// DFS from 'node', having arrived at node with 'isContinue'
// disposition; 'seen' is those nodes either currently being
// considered somewhere in the call chain ("gray"), or else finished
// entirely ("black"), and 'seenCont' is the same thing but for the
// continue==true halves of the nodes
void rp_dfs(NextPtrList &order, Statement const *node, bool isContinue,
            SObjSet<Statement const *> &seen, SObjSet<Statement const*> &seenCont)
{
  // we're now considering 'node'
  (isContinue? seenCont : seen).add(node);     // C++ generalized lvalue!

  // consider each of this node's successors
  NextPtrList successors;
  node->getSuccessors(successors, isContinue);

  for (VoidListIter iter(successors); !iter.isDone(); iter.adv()) {
    Statement const *succ = nextPtrStmt(iter.data());
    bool succCont = nextPtrContinue(iter.data());

    if ((succCont? seenCont : seen).contains(succ)) {
      // we're already considering, or have already considered, this node;
      // do nothing with it
    }
    else {
      // visit this new child
      rp_dfs(order, succ, succCont, seen, seenCont);
    }
  }

  // since we're finished with this node, we would append it to compute
  // the postorder; since we actually want reverse postorder, prepend
  order.prepend(makeNextPtr(node, isContinue));
}


void reversePostorder(NextPtrList &order, TF_func const &func)
{
  xassert(order.isEmpty());

  // DFS from the function start, computing the spanning tree implicitly,
  // and the reverse postorder explicitly
  SObjSet<Statement const*> seen, seenCont;
  rp_dfs(order, func.body, false /*isContinue*/, seen, seenCont);
}
