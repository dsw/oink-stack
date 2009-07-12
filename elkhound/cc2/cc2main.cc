// cc2main.cc            see license.txt for copyright and terms of use
// toplevel driver for cc2

#include <iostream.h>     // cout
#include <stdlib.h>       // exit

#include "trace.h"        // traceAddSys
#include "parssppt.h"     // ParseTreeAndTokens, treeMain
#include "cc_lang.h"      // CCLang
#include "ptreenode.h"    // PTreeNode
#include "parsetables.h"  // ParseTables
#include "cc2.gr.gen.h"   // CC2


// no bison-parser present, so need to define this
Lexer2Token const *yylval = NULL;


void doit(int argc, char **argv)
{
  traceAddSys("progress");
  //traceAddSys("parse-tree");

  SourceLocManager mgr;

  // parsing language options
  CCLang lang;
  lang.ANSI_Cplusplus();


  // --------------- parse --------------
  {
    SemanticValue treeTop;
    ParseTreeAndTokens tree(lang, treeTop);
    UserActions *user = new CC2;
    ParseTables *tables = user->makeTables();
    tree.userAct = user;
    tree.tables = tables;
    if (!treeMain(tree, argc, argv,
          "  additional flags for cc2:\n"
          "    printTree          print tree after parsing (if avail.)\n"
          "")) {
      // parse error
      exit(2);
    }

    traceProgress(2) << "final parse result: " << treeTop << endl;

    if (treeTop && tracingSys("printTree")) {
      PTreeNode *node = (PTreeNode*)treeTop;
      cout << "local ambiguities: " << PTreeNode::alternativeCount << endl;
      cout << "number of parses: " << node->countTrees() << endl;
      node->printTree(cout);
    }

    delete user;         
    delete tables;
  }

  traceRemoveAll();
}

int main(int argc, char **argv)
{
  doit(argc, argv);

  return 0;
}
