// main.cc
// driver program for scannerless example

#include "sless.h"     // Scannerless
#include "glr.h"       // GLR
#include "lexerint.h"  // LexerInterface
#include "strutil.h"   // quoted
#include "ptreenode.h" // PTreeNode
#include "ptreeact.h"  // ParseTreeLexer, ParseTreeActions

#include <stdio.h>     // getchar
#include <iostream.h>  // cout
#include <string.h>    // strcmp


class Lexer : public LexerInterface {
public:
  // function that retrieves the next token from
  // the input stream
  static void nextToken(LexerInterface *lex);
  virtual NextTokenFunc getTokenFunc() const
    { return &Lexer::nextToken; }

  // debugging assistance functions
  string tokenDesc() const;
  string tokenKindDesc(int kind) const;
};

void Lexer::nextToken(LexerInterface *lex)
{
  int ch = getchar();

  if (ch == EOF) {
    lex->type = 0 /*eof*/;
  }
  else {
    lex->type = ch;
  }
}

string Lexer::tokenDesc() const
{
  return tokenKindDesc(type);
}

string Lexer::tokenKindDesc(int kind) const
{
  string k = quoted(stringc << (char)kind);
  return stringf("%s (%d)", k.c_str(), kind);
}


int main(int argc, char *argv[])
{
  // use "-tree" command-line arg to print the tree
  bool printTree = argc==2 && 0==strcmp(argv[1], "-tree");

  // create and initialize the lexer
  Lexer lexer;
  lexer.nextToken(&lexer);

  // create the parser context object
  Scannerless sless;

  if (printTree) {
    // wrap the lexer and actions with versions that make a parse tree
    ParseTreeLexer ptlexer(&lexer, &sless);
    ParseTreeActions ptact(&sless, sless.makeTables());

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
    ptn->printTree(cout, PTreeNode::PF_EXPAND);
  }
  
  else {
    // initialize the parser
    GLR glr(&sless, sless.makeTables());

    // parse the input
    SemanticValue result;
    if (!glr.glrParse(lexer, result)) {
      printf("parse error\n");
      return 2;
    }

    printf("ok\n");
  }

  return 0;
}


// EOF
