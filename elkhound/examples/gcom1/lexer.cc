// lexer.cc
// code for lexer.h

#include "lexer.h"       // this module

#include <stdio.h>       // getchar, ungetc
#include <ctype.h>       // isspace
#include <string.h>      // strchr, strdup
#include <stdlib.h>      // abort

// simple hand-written lexer; could also be done using (e.g.) flex
void Lexer::nextToken(LexerInterface *lex)
{
  int ch = getchar();

  // skip whitespace
  while (isspace(ch)) {
    ch = getchar();
  }

  // end of file?
  if (ch == EOF) {
    lex->type = TOK_EOF;
    return;
  }

  // simple one-character tokens
  switch (ch) {
    case '+': lex->type = TOK_PLUS; return;
    case '-': lex->type = TOK_PLUS; return;
    case '*': lex->type = TOK_TIMES; return;
    case '(': lex->type = TOK_LPAREN; return;
    case ')': lex->type = TOK_RPAREN; return;
  }
  
  // integer literal
  if (isdigit(ch)) {
    int value = 0;
    while (isdigit(ch)) {
      value = value*10 + ch-'0';
      ch = getchar();
    }
    ungetc(ch, stdin);      // put back the nondigit

    // semantic value is the integer value of the literal
    lex->sval = (SemanticValue)value;

    lex->type = TOK_LITERAL;
    return;
  }

  // identifier
  if (isalpha(ch)) {        // must start with letter
    char buf[80];
    int i=0;
    while (isalnum(ch)) {   // but allow digits later on
      buf[i++] = (char)ch;
      if (i==80) {
        fprintf(stderr, "identifier is too long\n");
        abort();
      }
      ch = getchar();
    }
    buf[i]=0;
    ungetc(ch, stdin);

    // semantic value is a pointer to an allocated string; it
    // is simply leaked (never deallocated) for this example
    lex->sval = (SemanticValue)strdup(buf);

    lex->type = TOK_IDENTIFIER;
    return;
  }

  fprintf(stderr, "illegal character: %c\n", ch);
  abort();
}


string Lexer::tokenDesc() const
{
  switch (type) {
    // for two kinds of tokens, interpret their semantic value
    case TOK_LITERAL:      return stringf("%d", (int)sval);
    case TOK_IDENTIFIER:   return string((char*)sval);
    
    // otherwise, just return the token kind description
    default:               return tokenKindDesc(type);
  }
}


string Lexer::tokenKindDesc(int kind) const
{
  switch (kind) {
    case TOK_EOF:          return "EOF";
    case TOK_LITERAL:      return "lit";
    case TOK_IDENTIFIER:   return "id";
    default: {
      static char const map[] = "+-*()";
      return substring(&map[kind-TOK_PLUS], 1);
    }
  }
}


#ifdef TEST_LEXER
int main()
{
  Lexer lexer;
  for (;;) {
    lexer.getTokenFunc()(&lexer);    // first call yields a function pointer
    
    // print the returned token
    string desc = lexer.tokenDesc();
    printf("%s\n", desc.c_str());

    if (lexer.type == TOK_EOF) {
      break;
    }
  }
  
  return 0;
}
#endif // TEST_LEXER
