// xml_lexer.cc         see License.txt for copyright and terms of use

#include "xml_lexer.h"
#include "xassert.h"
#include "exc.h"


// ------------------------ XmlLexer -------------------
static char const * const tokenNames[] = {
  "XTOK_EOF",

  "XTOK_NAME",

  "XTOK_INT_LITERAL",
  "XTOK_FLOAT_LITERAL",
  "XTOK_HEX_LITERAL",
  "XTOK_STRING_LITERAL",

  "XTOK_LESSTHAN",
  "XTOK_GREATERTHAN",
  "XTOK_EQUAL",
  "XTOK_SLASH",

  "XTOK_DOT_ID",

  // tokens for lexing the Xml
#include "xml_name_1.gen.cc"

  "NUM_XML_TOKEN_TYPES",
};

int XmlLexer::getToken() {
  int token = this->yylex();
  if (token==0) {
    sawEof = true;
  }
  return token;
}

char const *XmlLexer::currentText() const 
{
  // Unfortunately, FlexLexer.h is missing a 'const'.  So
  // work around it with a cast.
  return const_cast<XmlLexer*>(this)->YYText();
}

void XmlLexer::restart(istream *in) 
{ 
  this->yyrestart(in); 
  linenumber = 1; 
  sawEof = false; 
}

int XmlLexer::tok(XmlToken kind)
{
//    printf("%s\n", tokenKindDesc(kind).c_str());
//    fflush(stdout);
  return kind;
}

int XmlLexer::svalTok(XmlToken kind)
{
//    printf("%s '%s'\n", tokenKindDesc(kind).c_str(), yytext);
//    fflush(stdout);
  return kind;
}

void XmlLexer::err(char const *msg)
{
  stringBuilder msg0;
  if (inputFname) {
    msg0 << inputFname << ":";
  }
  msg0 << linenumber << ":" << msg;
  THROW(xBase(msg0));
}

string XmlLexer::tokenKindDesc(int kind) const
{
  xassert(0 <= kind && kind < NUM_XML_TOKEN_TYPES);
  xassert(tokenNames[kind]);     // make sure the tokenNames array grows with the enum
  return tokenNames[kind];
}

string XmlLexer::tokenKindDescV(int kind) const
{
  stringBuilder s;
  s << tokenKindDesc(kind)
    << " (" << kind << ")";
  return s;
}

string XmlLexer::tokenDesc(int kind) const
{
  stringBuilder sb;
  sb << tokenKindDesc(kind) 
     << " (code=" << kind 
     << ", lexeme=\"" << currentText() << "\")";
  return sb;
}

// EOF
