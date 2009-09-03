// see License.txt for copyright and terms of use

#include "cfgprint.h"           // this module
#include "cfgprint_cmd.h"       // CfgPrintCmd
#include "cfgprint_global.h"
#include "oink.gr.gen.h"        // CCParse_Oink
#include "cfg.h"                // CFGEnv
#include "sobjset.h"            // SObjSet
#include "strutil.h"            // quoted
#include "oink_util.h"

void CfgPrint::computeCfg_stage() {
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    // compute the cfg; not sure where Scott keeps the thing
    int numCfgErrors = computeUnitCFG(unit);
    if (numCfgErrors) {
      std::cout << "number of cfg errors " << numCfgErrors << std::endl;
    }
  }
}

void CfgPrint::printCfg_stage() {
  std::cout << "digraph G {" << std::endl;
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    CfgPrintVisitor cpv;
    unit->traverse(cpv.loweredVisitor);
  }
  std::cout << "}" << std::endl;
}

// ****

class CfgPrintDfs {
  NextPtrList order;
  SObjSet<Statement*> seen;
  SObjSet<Statement*> seenCont;
  Function *func;
  NextPtr rootNode;
  // FIX: this is kind of ugly
  bool rootNodePrinted;

  public:
  CfgPrintDfs(Function *func0)
    : func(func0)
    , rootNode(NextPtr(func->body, false /*isContinue*/))
    , rootNodePrinted(false)
  {}
  private:
  // prohibit
  explicit CfgPrintDfs(CfgPrintDfs&);

  void dfg(NextPtr node);

  public:
  void go() {
    dfg(rootNode);
  }
};

// Stolen mercilessly from elsa/cfg.cc
//  void rp_dfs(NextPtrList &order, NextPtr node,
//              SObjSet<Statement*> &seen, SObjSet<Statement*> &seenCont)
void CfgPrintDfs::dfg(NextPtr node)
{
  // we're now considering 'node'; did we arrive via continue?
  (node.cont()? seenCont : seen).add(node.stmt());     // C++ generalized lvalue!

  // consider each of this node's successors
  NextPtrList successors;
  node.stmt()->getSuccessors(successors, node.cont());

  if (!rootNodePrinted) {
    std::cout << quoted(stringc << func->nameAndParams->var->fullyQualifiedName0())
         << " -> "
         << quoted(node.asString())
         << std::endl;
    rootNodePrinted = true;
  }

  for (int i=0; i < successors.length(); i++) {
    NextPtr succ = successors[i];
    bool haveSeenIt = (succ.cont() ? seenCont : seen).contains(succ.stmt());

    // print the edge out
    std::cout << quoted(stringc << node.asString())
         << " -> "
         << quoted(succ.asString())
         << std::endl;
    std::cout << "[";
    if (succ.cont()) std::cout << "color=green";
    std::cout << "]" << std::endl;

    if (haveSeenIt) {
      // we're already considering, or have already considered, this node;
      // do nothing with it
    } else {
      // visit this new child
      dfg(succ);
    }
  }

//    // since we're finished with this node, we append it to compute the
//    // postorder
//    order.push(node);
}

// ****

bool CfgPrintVisitor::visitFunction(Function *obj) {
  CfgPrintDfs cp(obj);
  cp.go();
  return true;
}
