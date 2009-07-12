/* see License.txt for copyright and terms of use */

%option noyywrap nounput
%x COMMENT

%{
  #include "libqual/lattice-parse.tab.h"
  #include "libregion/regions.h"
  #include <string.h>
  extern int lattice_line;
  extern region lattice_strings_region;

  #define mystrdup(s) rstrdup(lattice_strings_region, s)

  void cleanup_lattice_parse() {
#if 0
    // quarl 2006-07-11
    //    Commented out for now since it doesn't work in flex 2.5.4.
    if (yy_buffer_stack) {
      if (yy_buffer_stack[0]) {
        if (yy_buffer_stack[0]->yy_ch_buf)
          lyyfree(yy_buffer_stack[0]->yy_ch_buf);
        lyyfree(yy_buffer_stack[0]);
      }
      lyyfree(yy_buffer_stack);
      yy_buffer_stack = NULL;
    }
#endif
  }
%}

qualifier [0-9a-zA-Z_$]+
string \"[^""\n]*\"

%%

[\t ]      /* Skip */
\n          { lattice_line++; }
"/*"        { BEGIN(COMMENT); }
"const"     { lyylval.str = mystrdup(lyytext); return QUALIFIER; }
"volatile"  { lyylval.str = mystrdup(lyytext); return QUALIFIER; }
"restrict"  { lyylval.str = mystrdup(lyytext); return QUALIFIER; }
"color"     { return COLOR; }
"flow-insensitive"   { return FLOW_INSENSITIVE; }
"flow-sensitive"   { return FLOW_SENSITIVE; }
"nonprop"   { return NONPROP; }
"ref"       { return REF; }
"value"     { return VALUE; }
"level"     { return LEVEL; }
"sign"      { return SIGN; }
"pos"       { return POS; }
"neg"       { return NEG; }
"eq"        { return EQ; }
"up"        { return UP; }
"down"      { return DOWN; }
"all"       { return ALL; }
"none"      { return NONE; }
"ptrflow"   { return PTRFLOW; }
"fieldflow" { return FIELDFLOW; }
"fieldptrflow" { return FIELDPTRFLOW; }
"effect"    { return EFFECT; }
"casts-preserve"    { return CASTS_PRESERVE; }
[Pp][Aa][Rr][Tt][Ii][Aa][Ll] { return PARTIAL; }
[Oo][Rr][Dd][Ee][Rr] { return ORDER; }
\{          { return '{'; }
\}          { return '}'; }
{string}    { lyylval.str = mystrdup(lyytext+1);
              lyylval.str[strlen(lyylval.str)-1] = '\0';
              return STRING; }
\<          { return '<'; }
\=          { return '='; }
\[          { return '['; }
\]          { return ']'; }
,           { return ','; }
{qualifier} { lyylval.str = mystrdup(lyytext); return QUALIFIER; }

    /* Taken from flex man page */
<COMMENT>{
[^*\n]*        /* eat anything that's not a '*' */
"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
\n             { lattice_line++; }
"*"+"/"        BEGIN(INITIAL);
}
.	    { printf("Unrecognized character: %s\n", yytext); }

%%
