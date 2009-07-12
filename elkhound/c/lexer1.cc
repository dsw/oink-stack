// lexer1.cc            see license.txt for copyright and terms of use
// non-parser code for Lexer 1, declared in lexer1.h

#include "lexer1.h"       // this module
#include "typ.h"          // staticAssert, TABLESIZE
#include "trace.h"        // tracing stuff
#include "strutil.h"      // encodeWithEscapes

#include <stdio.h>        // printf
#include <assert.h>       // assert
#include <ctype.h>        // isprint


// -------------------- Lexer1Token -----------------------------
Lexer1Token::Lexer1Token(Lexer1TokenType aType, char const *aText,
               	         int aLength, SourceLoc aLoc)
  : type(aType),
    text(substring(aText, aLength)),           // makes a copy
    length(aLength),
    loc(aLoc)
{}

Lexer1Token::~Lexer1Token()
{
  // 'text' deallocates its string
}


// map Lexer1TokenType to a string
char const *l1Tok2String(Lexer1TokenType tok)
{
  char const *map[] = {
    "L1_IDENTIFIER",
    "L1_INT_LITERAL",
    "L1_FLOAT_LITERAL",
    "L1_STRING_LITERAL",
    "L1_UDEF_QUAL",             // dsw: user-defined qualifier: $tainted
    "L1_CHAR_LITERAL",
    "L1_OPERATOR",
    "L1_PREPROCESSOR",
    "L1_WHITESPACE",
    "L1_COMMENT",
    "L1_ILLEGAL"
  };
  assert(TABLESIZE(map) == NUM_L1_TOKENS);

  assert(tok >= L1_IDENTIFIER && tok < NUM_L1_TOKENS);
  return map[tok];
}


void Lexer1Token::print() const
{
  char const *fname;
  int line, col;
  sourceLocManager->decodeLineCol(loc, fname, line, col);

  printf("[L1] Token at line %d, col %d: %s \"%s\"\n",
         line, col, l1Tok2String(type),
         encodeWithEscapes(text.c_str(), length).c_str());
}


// -------------------- Lexer1 -----------------------------
Lexer1::Lexer1(char const *fname)
  : allowMultilineStrings(true),    // GNU extension
    loc(sourceLocManager->encodeBegin(fname)),
    errors(0),
    tokens(),
    tokensMut(tokens)
{}

Lexer1::~Lexer1()
{
  // tokens list is deallocated
}


// eventually I want this to store the errors in a list of objects...
void Lexer1::error(char const *msg)
{
  char const *fname;
  int line, col;
  sourceLocManager->decodeLineCol(loc, fname, line, col);

  printf("[L1] Error at line %d, col %d: %s\n", line, col, msg);
  errors++;
}


void Lexer1::emit(Lexer1TokenType toktype, char const *tokenText, int length)
{
  // construct object to represent this token
  Lexer1Token *tok = new Lexer1Token(toktype, tokenText, length, loc);

  // (debugging) print it
  if (tracingSys("lexer1")) {
    tok->print();
  }

  // illegal tokens should be noted
  if (toktype == L1_ILLEGAL) {
    error(stringb("illegal token: `" << tokenText << "'"));
  }

  // add it to our running list of tokens
  tokensMut.append(tok);

  // update line and column counters
  loc = sourceLocManager->advText(loc, tokenText, length);
}


// ------------------- testing -----------------------
#ifdef TEST_LEXER1

int main(int argc, char **argv)
{
  while (traceProcessArg(argc, argv)) {}

  if (argc < 2) {
    printf("usage: lexer1 <file>\n");
    return 0;
  }

  Lexer1 lexer(argv[1]); 
  lexer1_lex(lexer, fopen(argv[1], "r"));

  printf("%d token(s), %d error(s)\n",
         lexer.tokens.count(), lexer.errors);

  return 0;
}

#endif // TEST_LEXER1


