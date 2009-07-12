// bcparse.cc            see license.txt for copyright and terms of use
// driver for bison parser for c.gr

#include "bcparse.h"    // this module
#include "lexer2.h"     // Lexer2
#include "lexer1.h"     // Lexer1
#include "trace.h"      // traceProgress
#include "syserr.h"     // xsyserror
#include "cc_lang.h"    // CCLang
#include "cyctimer.h"   // CycleTimer

#include <stdio.h>      // printf
#include <iostream.h>   // cout, etc.
#include <string.h>     // strcmp

// global list of L2 tokens for yielding to Bison
CCLang cclang;
Lexer2 lexer2(cclang);
Lexer2Token const *lastTokenYielded = NULL;

// parsing entry point
int yyparse();


#if 0     // previous interface
// returns token types until EOF, at which point L2_EOF is returned
int yylex()
{
  static ObjListIter<Lexer2Token> *iter = NULL;

  if (!iter) {
    // prepare to return tokens
    iter = new ObjListIter<Lexer2Token>(lexer2.tokens);
  }

  if (!iter->isDone()) {
    // grab type to return
    lastTokenYielded = iter->data();
    Lexer2TokenType ret = iter->data()->type;

    // advance to next token
    iter->adv();

    // since my token reclassifier is hard to translate to
    // Bison, I'll just yield variable names always, and only
    // parse typedef-less C programs
    if (ret == L2_NAME) {
      ret = L2_VARIABLE_NAME;
    }

    // return one we just advanced past
    return ret;
  }
  else {
    // done; don't bother freeing things
    lastTokenYielded = NULL;
    return L2_EOF;
  }
}
#endif // 0


LexerInterface::NextTokenFunc nextToken;

// returns token types until EOF, at which point L2_EOF is returned
int yylex()
{
  int ret = lexer2.type;

  // since my token reclassifier is hard to translate to
  // Bison, I'll just yield variable names always, and only
  // parse typedef-less C programs
  if (ret == L2_NAME) {
    ret = L2_VARIABLE_NAME;
  }

  if (ret != L2_EOF) {
    // advance to next token
    nextToken(&lexer2);
  }

  // return one we just advanced past
  return ret;
}


void yyerror(char const *s)
{
  if (lastTokenYielded) {
    printf("%s: ", toString(lastTokenYielded->loc).pcharc());
  }
  else {
    printf("<eof>: ");
  }
  printf("%s\n", s);
}


int main(int argc, char *argv[])
{
  char const *progname = argv[0];

  if (argc >= 2 &&
      0==strcmp(argv[1], "-d")) {
    #ifdef YYDEBUG
      yydebug = 1;
    #else
      printf("debugging is disabled by NDEBUG\n");
      return 2;
    #endif

    argc--;
    argv++;
  }

  if (argc < 2) {
    printf("usage: %s [-d] input.c\n", progname);
    printf("  -d: turn on yydebug, so it prints shift/reduce actions\n");
    return 0;
  }

  char const *inputFname = argv[1];

  traceAddSys("progress");

  SourceLocManager mgr;

  traceProgress() << "lexical analysis stage 1...\n";
  Lexer1 lexer1(inputFname);
  {
    FILE *input = fopen(inputFname, "r");
    if (!input) {
      xsyserror("fopen", inputFname);
    }

    lexer1_lex(lexer1, input);
    fclose(input);

    if (lexer1.errors > 0) {
      printf("L1: %d error(s)\n", lexer1.errors);
      return 2;
    }
  }

  // do second phase lexer
  traceProgress() << "lexical analysis stage 2...\n";
  lexer2_lex(lexer2, lexer1, inputFname);
  lexer2.beginReading();
  nextToken = lexer2.getTokenFunc();

  // start Bison-parser
  traceProgress() << "starting parse..." << endl;
  CycleTimer timer;
  if (yyparse() != 0) {
    cout << "yyparse returned with an error\n";
    return 4;
  }
  traceProgress() << "finished parse (" << timer.elapsed() << ")\n";

  return 0;
}
