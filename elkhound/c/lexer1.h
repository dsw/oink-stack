// lexer1.h            see license.txt for copyright and terms of use
// Lexer 1: tokenize a single file, no translations
// see lexer1.txt

#ifndef __LEXER1_H
#define __LEXER1_H

#include "objlist.h"   // ObjList
#include "srcloc.h"    // SourceLoc

#include <stdio.h>     // FILE

// type of each L1 token
enum Lexer1TokenType {
  L1_IDENTIFIER,
  L1_INT_LITERAL,
  L1_FLOAT_LITERAL,
  L1_STRING_LITERAL,
  L1_UDEF_QUAL,                 // dsw: user-defined qualifier
  L1_CHAR_LITERAL,
  L1_OPERATOR,
  L1_PREPROCESSOR,
  L1_WHITESPACE,
  L1_COMMENT,
  L1_ILLEGAL,
  NUM_L1_TOKENS
};


// unit of output from L1
class Lexer1Token {
public:
  Lexer1TokenType type;         // kind of token
  string text;                  // token's text, null-terminated
  int length;                   // length of text (somewhat redundant, but whatever)
  SourceLoc loc;                // location in input stream

public:
  Lexer1Token(Lexer1TokenType aType, char const *aText, int aLength,
              SourceLoc aLoc);
  ~Lexer1Token();

  // debugging
  void print() const;
};


// L1 lexing state
class Lexer1 {
public:
  // lexer options
  bool allowMultilineStrings;             // true if newlines don't need to be escaped

  // lexing input state
  SourceLoc loc;                          // current location
  int errors;	                          // # of errors encountered so far

  // lexing results
  ObjList<Lexer1Token> tokens;            // list of tokens produced
  ObjListMutator<Lexer1Token> tokensMut;  // for appending to the 'tokens' list

public:
  Lexer1(char const *fname);
  ~Lexer1();

  // called by parser
  void error(char const *msg);
  void emit(Lexer1TokenType toktype, char const *text, int length);
};


// external interface to lexer; parses the entire token
// stream into 'lexer' object
int lexer1_lex(Lexer1 &lexer, FILE *inputFile);


// utilites
void printEscaped(char const *p, int len);



#endif // __LEXER1_H
