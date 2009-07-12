// lexer.cc
// code for lexer.h

#include "lexer.h"       // this module

#include <stdio.h>       // getchar, ungetc
#include <ctype.h>       // isspace, isalpha, isalnum
#include <string.h>      // strchr, strdup, strcmp
#include <stdlib.h>      // abort
#include <assert.h>      // assert

// simple hand-written lexer; could also be done using (e.g.) flex
void Lexer::nextToken(LexerInterface *lex)
{
  // avoid accidental use of another token's sval
  lex->sval = (SemanticValue)0;

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
    case '*': lex->type = TOK_TIMES; return;
    case '(': lex->type = TOK_LPAREN; return;
    case ')': lex->type = TOK_RPAREN; return;

    case '-':
      // TOK_MINUS or TOK_ARROW?
      ch = getchar();
      if (ch == '>') {
        lex->type = TOK_ARROW;
      }
      else {
        lex->type = TOK_MINUS;
        ungetc(ch, stdin);
      }
      return;

    case '=': lex->type = TOK_EQUAL; return;
    case '<': lex->type = TOK_LESS; return;
    case '!': lex->type = TOK_NOT; return;
    case ';': lex->type = TOK_SEMI; return;
    case '#': lex->type = TOK_FATBAR; return;

    case '/':
      ch = getchar();
      if (ch != '\\') fail("'/' must be followed by '\\'");
      lex->type = TOK_AND; 
      return;

    case '\\':
      ch = getchar();
      if (ch != '/') fail("'\\' must be followed by '/'");
      lex->type = TOK_OR;
      return;

    case ':':
      ch = getchar();
      if (ch != '=') fail("':' must be followed by '='");
      lex->type = TOK_ASSIGN;
      return;
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

  // identifier or keyword
  if (isalpha(ch)) {        // must start with letter
    char buf[80];
    int i=0;
    while (isalnum(ch)) {   // but allow digits later on
      buf[i++] = (char)ch;
      if (i==80) {
        fail("identifier is too long\n");
      }
      ch = getchar();
    }
    buf[i]=0;
    ungetc(ch, stdin);

    // keyword?  (not a very efficient way to do this ...)
    if (0==strcmp(buf, "true"))  { lex->type = TOK_TRUE; return; }
    if (0==strcmp(buf, "false")) { lex->type = TOK_FALSE; return; }
    if (0==strcmp(buf, "skip"))  { lex->type = TOK_SKIP; return; }
    if (0==strcmp(buf, "abort")) { lex->type = TOK_ABORT; return; }
    if (0==strcmp(buf, "print")) { lex->type = TOK_PRINT; return; }
    if (0==strcmp(buf, "if"))    { lex->type = TOK_IF; return; }
    if (0==strcmp(buf, "fi"))    { lex->type = TOK_FI; return; }
    if (0==strcmp(buf, "do"))    { lex->type = TOK_DO; return; }
    if (0==strcmp(buf, "od"))    { lex->type = TOK_OD; return; }

    // semantic value is a pointer to an allocated string; it
    // is simply leaked (never deallocated) for this example
    lex->sval = (SemanticValue)strdup(buf);

    lex->type = TOK_IDENTIFIER;
    return;
  }

  fprintf(stderr, "illegal character: %c\n", ch);
  abort();
}

void Lexer::fail(char const *msg)
{
  fprintf(stderr, "%s\n", msg);
  abort();
}


string Lexer::tokenDesc() const
{
  switch (type) {
    // for two kinds of tokens, interpret their semantic value
    case TOK_LITERAL:      return stringf("%d", (int)sval);
    case TOK_IDENTIFIER:   return stringf("id(%s)", (char*)sval);

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
      static char const * const map[] = {
        "+",
        "-",
        "*",
        "(",
        ")",

        "true",
        "false",
        "=",
        "<",
        "!",
        "/\\",
        "\\/",

        "skip",
        "abort",
        "print",
        ":=",
        ";",
        "if",
        "fi",
        "do",
        "od",

        "->",
        "#"
      };
      kind -= TOK_PLUS;
      assert((unsigned)kind < (sizeof(map)/sizeof(map[0])));
      return map[kind];
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
