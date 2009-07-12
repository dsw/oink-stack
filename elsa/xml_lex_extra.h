#ifndef FLEX_SCANNER
# error "This file should only be included in the flex-generated scanner"
#endif

#include <assert.h>

// SGM 2006-05-26: My flex does not seem to have
// YY_CURRENT_BUFFER_LVALUE.. maybe this will work?  seems to.
#ifndef YY_CURRENT_BUFFER_LVALUE
  #define YY_CURRENT_BUFFER_LVALUE YY_CURRENT_BUFFER
#endif

// This file is to be included in xml_lex.gen.yy.cc (see xml_lex_0top.lex).
// It depends on flex macros so we can't make it a separate translation unit.
// It's named a ".h" file to work with existing oink build infrastructure.

// Read a string, save it in dequote, and advance pointer past closing quote.
//
// The dequoted and unescaped string is null-terminated and stored in yytext.
//
// Returns 1 on success (or recoverable error); returns 0 on error.
bool XmlLexer::read_xml_string()
{
  // action should have read 1 char
  assert(yy_c_buf_p == yytext + 1);

  // unput the saved char (which was clobbered with '\0')
  assert(*yy_c_buf_p == '\0');
  *yy_c_buf_p = (yy_hold_char);

  // the quote char is the previous character
  char delim = yy_c_buf_p[-1];
  assert(delim == '\'' || delim == '"');

  // p now points to the after the open quote.
  // This pointer which is what we'll return.
  yytext = yy_c_buf_p;

  // Optimization: try to do a lot before we have to start copying chars due
  // to unescaping.  If the entire string has nothing to unescape we can
  // just return a pointer from the lexer buffer without modification!

  // loop#1: no copying, since no characters so far were modified.
  while (1) {
    if (yyunderflow() < 0) return false;
    if (*yy_c_buf_p == delim) {
      // Yay, reached end of string with nothing to decode!
      // Overwrite quote, which we no longer need, with a '\0'
      *yy_c_buf_p = '\0';

      // yy_c_buf_p currently points at the quote; the next character to lex
      // is the one after that
      ++yy_c_buf_p;

      // This is needed since the real lexer will try to unclobber with this
      // character
      yy_hold_char = *yy_c_buf_p;
      return true;
    }

    if (*yy_c_buf_p == '&' /*|| *yy_c_buf_p == '\\'*/) {
      // Special characters: we're going to have to copy characters now so
      // go into loop#2.
      break;
    }

    if (*yy_c_buf_p == '<') {
      // illegal - http://www.w3.org/TR/2004/REC-xml-20040204/#NT-AttValue
      err("non-escaped left-angle-bracket (<) inside string literal");
      return false;
    }

    if (*yy_c_buf_p == '\n') {
      err("string literal missing final `\"'");

      // error recovery
      *yy_c_buf_p = '\0';
      ++yy_c_buf_p;
      yy_hold_char = *yy_c_buf_p;
      return true;
    }

    // normal character; continue.
    ++yy_c_buf_p;
  }

  // Destination to write characters.
  char *d = yy_c_buf_p;

  // loop#2: for all characters, whether unescaped or not, we have to copy
  // to the destination.  The destination is actually in the same buffer,
  // but guaranteed to be <= the current position since unescaping can only
  // shorten.

  while (1) {
    if (int movement = yyunderflow()) {
      if (movement < 0) return false;
      d -= movement;
    }

    if (*yy_c_buf_p == delim) {
      // Reached end of string.
      //
      // Null-terminate destination string by overwriting some character
      // which we no longer need.
      *d = '\0';
      // Advance read pointer to past quote char.
      ++yy_c_buf_p;
      yy_hold_char = *yy_c_buf_p;
      return true;
    }

    // It looks like backslash is not an escaping character in XML?
    // if (*yy_c_buf_p == '\\') {
    // }

    else if (*yy_c_buf_p == '&') {
      static const int MAX_ENTITY_CHARS = 10;
      // Special characters

      char *entity_start = ++yy_c_buf_p;
      int entity_length;
      // find the ';'
      while (1) {
        if (int movement = yyunderflow()) {
          if (movement < 0) return false;
          d -= movement;
          entity_start -= movement;
        }
        if (*yy_c_buf_p == ';') break;
        ++yy_c_buf_p;
        if (yy_c_buf_p - entity_start > MAX_ENTITY_CHARS)
          goto bad_entity;
      }

      // entity_start points at after '&', and yy_c_buf_p points at ';'
      assert (*yy_c_buf_p == ';');

      entity_length = yy_c_buf_p - entity_start;
      if (entity_length < 2) goto bad_entity;

      if (*entity_start == '#') {
        char *digit = entity_start + 1;
        int base = 10;
        if (*digit == 'x') {
          base = 16;
          digit++;
        }

        char *endptr;
        unsigned long val = strtoul(digit, &endptr, base);
        // endptr should now point to the ';' if strtoul was happy
        if (endptr != yy_c_buf_p) {
          goto bad_entity;
        }
        // possible truncation since we're using skinny chars
        *d++ = ((char) (unsigned char) val);
        yy_c_buf_p++;
        continue;
      }

#define DO_ENTITY(S, C) DO_ENTITY1(S, (sizeof(S)-1), C)
#define DO_ENTITY1(S, N, C)                                       \
      if (entity_length==N && 0==memcmp(entity_start, S, N)) {    \
        *d++ = C;                                                 \
        yy_c_buf_p++;                                             \
        continue;                                                 \
      }

      DO_ENTITY("lt",   '<');
      DO_ENTITY("gt",   '>');
      DO_ENTITY("amp",  '&');
      DO_ENTITY("quot", '"');
      DO_ENTITY("apos", '\'');
#undef DO_ENTITY

    bad_entity:
      err("use of an unimplemented or illegal amperstand escape");

      // error recovery: write character as usual
      yy_c_buf_p = entity_start;
    }

    else if (*yy_c_buf_p == '<') {
      // illegal - http://www.w3.org/TR/2004/REC-xml-20040204/#NT-AttValue
      err("non-escaped left-angle-bracket (<) inside string literal");
      return false;
    }

    else if (*yy_c_buf_p == '\n') {
      err("string literal missing final `\"'");

      // error recovery
      *yy_c_buf_p = '\0';
      ++yy_c_buf_p;
      yy_hold_char = *yy_c_buf_p;
      return false;
    }

    else {
      // regular character
      *d++ = *yy_c_buf_p++;
    }
  }
  // not reached
}

// Underflow function to read_xml_string() based on yyinput() but optimized
// for read_xml_string().  It returns the backwards movement (as a positive
// number) of current pointers within the buffer.  yy_c_buf_p is updated if
// necessary.  Other pointers between yytext and yy_c_buf_p are still valid
// if you update them.  After calling this function, *yy_c_buf_p should
// point to the next character to read.
//
// If end-of-buffer is reached, return -1.
//
// It assumes yytext is not NULL-terminated i.e. do not unclobber the buffer
// (which is unnecessary and annoying), and do not reclobber the buffer
// after input.  Also do not support yywrap().  If we reach end of buffer,
// die.

inline int XmlLexer::yyunderflow() {
  assert(*yy_c_buf_p != '\0');
  if (*yy_c_buf_p == YY_END_OF_BUFFER_CHAR) {
    if ( (yy_c_buf_p) < &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] ) {
      // reached null

      // TODO: improve error messages. prefix with "XML parser: " and add
      // line number.
      err("unterminated string literal");
      return -1;
    } else {
      // need more input
      char *old_yytext = yytext;
      int movement;
#ifndef NDEBUG
      int offset = yy_c_buf_p - yytext; // debug
#endif
      ++yy_c_buf_p;

      switch (yy_get_next_buffer())
      {
      case EOB_ACT_LAST_MATCH: // fall-through
      case EOB_ACT_END_OF_FILE:
        // end of file.
        err("unterminated string literal");
        return -1;

      case EOB_ACT_CONTINUE_SCAN:
        // yytext should have been moved to beginning of buffer
        movement = old_yytext - yytext;
        assert (movement > 0);

        yy_c_buf_p--;
        yy_c_buf_p -= movement;
        assert (yy_c_buf_p == yytext+offset);
        return movement;

      default:
        assert(0);
      }
    }
  }
  return 0;
}
