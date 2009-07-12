// parser.cc
// driver program for guarded command example

#include "lexer.h"     // Lexer
#include "gcom.h"      // GCom
#include "glr.h"       // GLR


int main()
{
  // create and initialize the lexer
  Lexer lexer;
  lexer.nextToken(&lexer);

  // create the parser context object
  GCom gcom;

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
  
  return 0;
}



