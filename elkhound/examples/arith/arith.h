// arith.h
// declarations shared across the arith evaluator

#ifndef ARITH_H
#define ARITH_H

#include "lexerint.h"        // LexerInterface
#include "arith.gr.gen.h"    // Arith, the parser context class

class ParseTables;           // parsetables.h
class UserActions;           // useract.h



// interface to the parser generator's output;
// defined in arith.gr -> arith.gr.gen.cc
UserActions *makeUserActions();
ParseTables *make_Arith_tables();

// interface to the lexer
int yylex();                        // defined in arith.lex -> arithyy.cc


// token codes
enum ArithTokenCodes {
  TOK_EOF    =0,
  TOK_NUMBER =1,
  TOK_PLUS   =2,
  TOK_MINUS  =3,
  TOK_TIMES  =4,
  TOK_DIVIDE =5,
  TOK_LPAREN =6,
  TOK_RPAREN =7,
};

char const *toString(ArithTokenCodes code);


// lexer interface object
class ArithLexer : public LexerInterface {
public:
  static void nextToken(ArithLexer *ths);

  // LexerInterface functions
  virtual NextTokenFunc getTokenFunc() const;
  virtual string tokenDesc() const;
  virtual string tokenKindDesc(int kind) const;
};

// there will be only one
extern ArithLexer lexer;



#endif // ARITH_H
