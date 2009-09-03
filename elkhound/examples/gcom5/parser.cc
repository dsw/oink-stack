// parser.cc
// driver program for guarded command example

#include "lexer.h"     // Lexer
#include "gcom.h"      // GCom
#include "glr.h"       // GLR
#include "ptreenode.h" // PTreeNode
#include "ptreeact.h"  // ParseTreeLexer, ParseTreeActions

#include <string.h>    // strcmp


int main(int argc, char *argv[])
{
  // use "-tree" command-line arg to print the tree
  bool printTree = argc==2 && 0==strcmp(argv[1], "-tree");

  // create and initialize the lexer
  Lexer lexer;
  lexer.nextToken(&lexer);

  // create the parser context object
  GCom gcom;

  if (printTree) {
    // wrap the lexer and actions with versions that make a parse tree
    ParseTreeLexer ptlexer(&lexer, &gcom);
    ParseTreeActions ptact(&gcom, gcom.makeTables());

    // initialize the parser
    GLR glr(&ptact, ptact.getTables());

    // parse the input
    SemanticValue result;
    if (!glr.glrParse(ptlexer, result)) {
      printf("parse error\n");
      return 2;
    }

    // print the tree
    PTreeNode *ptn = (PTreeNode*)result;
    ptn->printTree(std::cout, PTreeNode::PF_EXPAND);
  }
  
  else {
    // initialize the parser
    GLR glr(&gcom, gcom.makeTables());

    // parse the input
    SemanticValue result;
    if (!glr.glrParse(lexer, result)) {
      printf("parse error\n");
      return 2;
    }

    // print result
    printf("result: %d\n", (int)result);
  }

  return 0;
}



