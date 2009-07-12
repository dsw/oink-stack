/* arith.lex
 * lexical analyzer for arithmetic language */

/* flex options */
%option noyywrap
%option nounput


/* C++ declarations */
  #include "arith.h"       // lexer, ArithTokenCodes


/* token definitions */
%%

[0-9]+      {
  lexer.sval = (SemanticValue)atoi(yytext);
  return TOK_NUMBER;
}

  /* operators, punctuators */
"+"         { return TOK_PLUS; }
"-"         { return TOK_MINUS; }
"*"         { return TOK_TIMES; }
"/"         { return TOK_DIVIDE; }
"("         { return TOK_LPAREN; }
")"         { return TOK_RPAREN; }

[ \t\n]     {
  /* whitespace; ignore */
}

"#.*\n"     {
  /* comment; ignore */
}

.           {
  printf("illegal character: %c\n", yytext[0]);
  /* but continue anyway */
}
