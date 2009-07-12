// lexer.h
// lexer for the guarded-command example language

#ifndef LEXER_H
#define LEXER_H

#include "lexerint.h"      // LexerInterface

// token codes (must agree with the parser)
enum TokenCode {
  TOK_EOF         = 0,     // end of file
  
  // minimal set of tokens for AExp
  TOK_LITERAL,             // integer literal
  TOK_IDENTIFIER,          // identifier like "x"
  TOK_PLUS,                // "+"
  TOK_MINUS,               // "-"
  TOK_TIMES,               // "*"
  TOK_LPAREN,              // "("
  TOK_RPAREN,              // ")"

  // for BExp
  TOK_TRUE,                // "true"
  TOK_FALSE,               // "false"
  TOK_EQUAL,               // "="
  TOK_LESS,                // "<"
  TOK_NOT,                 // "!"
  TOK_AND,                 // elkhound doesn't like quoted backslashes
  TOK_OR,

  // for Stmt
  TOK_SKIP,                // "skip"
  TOK_ABORT,               // "abort"
  TOK_PRINT,               // "print"
  TOK_ASSIGN,              // ":="
  TOK_SEMI,                // ";"
  TOK_IF,                  // "if"
  TOK_FI,                  // "fi"
  TOK_DO,                  // "do"
  TOK_OD,                  // "od"
  
  // for GCom
  TOK_ARROW,               // "->"
  TOK_FATBAR,              // "#"
};


// read characters from stdin, yield tokens for the parser
class Lexer : public LexerInterface {
private:
  static void fail(char const *msg);

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


#endif // LEXER_H
