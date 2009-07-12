  /* see License.txt for copyright and terms of use */

  /* Qual extensions to the lexer; this file is inserted into elsa's
     lexer.lex file. */

  /* user-defined qualifier; example: $tainted */
"$"{ALNUM}+ {
  return svalTok(TOK_UDEF_QUAL);
}

  /* ad-hoc user-defined qualifier that means something other than a lattice qualifier */
"$!"{ALNUM}+ {
  return svalTok(TOK_UDEF_QUAL);
}

  /* a user type variable; example: $$a as in $$a _op_deref($$a *$kernel); */
"$$"{ALNUM}+ {
  return svalTok(TOK_UTYPE_VAR);
}

"__qual_assert_type"           return tok(TOK_ASSERT_TYPE);
"__qual_change_type"           return tok(TOK_CHANGE_TYPE);
