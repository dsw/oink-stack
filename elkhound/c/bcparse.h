// bcparse.h            see license.txt for copyright and terms of use
// decls shared between bcparse.cc and c.gr.gen.y

#ifndef BCPARSE_H
#define BCPARSE_H

#include <stdlib.h>     // free

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// functions called by Bison-parser
void yyerror(char const *msg);
int yylex();

// Bison-parser entry
int yyparse();
extern int yydebug;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BCPARSE_H
