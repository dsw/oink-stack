  /* xml_lex_2bot.lex            see license.txt for copyright and terms of use
   * flex description of scanner for C and C++ souce
   */

  /* This file is the bottom part of the generated .lex file. */

  /* identifier: e.g. foo */
{ALNUMDOT}* {
  return svalTok(XTOK_NAME);
}

  /** dequote the string directly while lexing */

({QUOTE}|{SQUOTE}) {
  // read_xml_string() will Do The Right Thing: read a quoted and escaped
  // string; dequote and unescape into yytext and update all pointers
  // appropriately.
  read_xml_string();
  return svalTok(XTOK_STRING_LITERAL);
}

[\n]   {
  ++linenumber;                 /* have to do this manually; see above */
}

  /* whitespace */
  /* 10/20/02: added '\r' to accomodate files coming from Windows; this
   * could be seen as part of the mapping from physical source file
   * characters to the basic character set (cppstd 2.1 para 1 phase 1),
   * except that it doesn't happen for chars in string/char literals... */
[ \t\f\v\r]+  {
  /*    whitespace(); */
}

  /* illegal */
.  {
  /* updLoc(); */
  err(stringc << "illegal character: `" << yytext[0] << "'");
}

<<EOF>> {
  /*    srcFile->doneAdding(); */
  yyterminate();
}


%%
