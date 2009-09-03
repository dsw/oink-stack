// arith.cc
// driver program for arithmetic evaluator

#include "arith.h"     // this module
#include "glr.h"       // GLR parser
#include "ptreenode.h" // PTreeNode
#include "ptreeact.h"  // ParseTreeLexer, ParseTreeActions
#include "trace.h"     // traceAddSys

#include <assert.h>    // assert


// ------------------ ArithLexer ------------------
/*static*/ void ArithLexer::nextToken(ArithLexer *ths)
{
  // call underlying lexer; it will set 'sval' if necessary
  ths->type = yylex();
}

LexerInterface::NextTokenFunc ArithLexer::getTokenFunc() const
{
  return (NextTokenFunc)&ArithLexer::nextToken;
}

  
char const *toString(ArithTokenCodes code)
{
  char const * const names[] = {
    "EOF",
    "number",
    "+",
    "-",
    "*",
    "/",
    "(",
    ")",
  };

  assert((unsigned)code < sizeof(names) / sizeof(names[0]));
  return names[code];
}

string ArithLexer::tokenDesc() const
{
  if (type == TOK_NUMBER) {
    return stringc << "number(" << (int)sval << ")";
  }
  else {
    return toString((ArithTokenCodes)type);
  }
}

string ArithLexer::tokenKindDesc(int kind) const
{
  return toString((ArithTokenCodes)kind);
}


// --------------------- main ----------------------
ArithLexer lexer;

int main(int argc)
{
  // initialize lexer by grabbing first token
  lexer.nextToken(&lexer);

  // create parser; actions and tables not dealloc'd but who cares
  Arith *arith = new Arith;
  ParseTables *tables = arith->makeTables();

  // uncomment this to get bison-like shift/reduce reports
  //traceAddSys("parse");
  
  // get tracing info from environment variable TRACE
  traceAddFromEnvVar();

  if (argc == 1) {
    // start parsing
    GLR glr(arith, tables);
    SemanticValue result;
    if (!glr.glrParse(lexer, result)) {
      printf("parse error\n");
      return 2;
    }

    // print result
    printf("result: %d\n", (int)result);
  }

  else {
    // make it print a parse tree instead of evaluating the expression
    ParseTreeLexer ptlexer(&lexer, arith);
    ParseTreeActions ptact(arith, tables);

    GLR glr(&ptact, tables);
    SemanticValue result;
    if (!glr.glrParse(ptlexer, result)) {
      printf("parse error\n");
      return 2;
    }

    // print the tree
    PTreeNode *ptn = (PTreeNode*)result;
    ptn->printTree(std::cout, PTreeNode::PF_EXPAND);
  }

  return 0;
}
