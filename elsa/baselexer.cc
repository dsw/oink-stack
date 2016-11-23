// baselexer.cc            see license.txt for copyright and terms of use
// code for baselexer.h

#include "baselexer.h"   // this module
#include "strtable.h"    // StringTable
#include "exc.h"         // throw_XOpen

#include <fstream>       // ifstream

#if defined(__GNUC__) && (__GNUC__ > 2)
  // gcc-3 doesn't have istrstream (but it is standard!), so fake it
  #include <sstream>       // istringstream

  #undef string            // in case the string->mystring definition is active

  inline std::istream *construct_istrstream(char const *buf, int len)
  {
    return new std::istringstream(std::string(buf, len));
  }
#else
  // should work on any compiler that implements the C++98 standard
  // (istrstream is deprecated but still standard)
  #include <strstream>   // istrstream

  inline std::istream *construct_istrstream(char const *buf, int len)
  {
    return new std::istrstream(buf, len);
  }
#endif


// this function effectively lets me initialize one of the
// members before initing a base class
std::istream *BaseLexer::openFile(char const *fname)
{
  // 2005-01-17: open in binary mode to coincide with srcloc.cc
  // doing the same, for cygwin reasons
  this->inputStream = new std::ifstream(fname, std::ios::in | std::ios::binary);
  if (!*inputStream) {
    // destructor won't be called so delete here.
    delete inputStream; inputStream = NULL;
    throw_XOpen(fname);
  }
  return inputStream;
}

BaseLexer::BaseLexer(StringTable &s, char const *fname)
  : yyFlexLexer(openFile(fname)),

    // 'inputStream' is initialized by 'openFile'
    srcFile(NULL),           // changed below

    nextLoc(SL_UNKNOWN),     // changed below
    curLine(1),

    strtable(s),
    errors(0),
    warnings(0)
{
  srcFile = sourceLocManager->getInternalFile(fname);

  loc = sourceLocManager->encodeBegin(fname);
  nextLoc = loc;
}


std::istream *BaseLexer::openString(char const *buf, int len)
{
  this->inputStream = construct_istrstream(buf, len);
  return inputStream;
}

BaseLexer::BaseLexer(StringTable &s, SourceLoc initLoc,
                     char const *buf, int len)
  : yyFlexLexer(openString(buf, len)),

    // 'inputStream' is initialized by 'openString'
    srcFile(NULL),           // changed below

    nextLoc(initLoc),
    curLine(0),              // changed below

    strtable(s),
    errors(0),
    warnings(0)
{
  // decode the given location
  char const *fname;
  int line, col;
  sourceLocManager->decodeLineCol(initLoc, fname, line, col);

  // finish initializing data members
  srcFile = sourceLocManager->getInternalFile(fname);
  curLine = line;

  loc = initLoc;
}


BaseLexer::~BaseLexer()
{
  delete inputStream;
}


StringRef BaseLexer::addString(char *str, int len)
{
  // (copied from gramlex.cc, GrammarBaseLexer::addString)

  // write a null terminator temporarily
  char wasThere = str[len];
  if (wasThere) {
    str[len] = 0;
    StringRef ret = strtable.add(str);
    str[len] = wasThere;
    return ret;
  }
  else {
    return strtable.add(str);
  }
}

StringRef BaseLexer::addStringConst(const char *str)
{
  return strtable.add(str);
}


void BaseLexer::whitespace()
{
  updLoc();

  // scan for newlines
  char *p = yytext, *endp = yytext+yyleng;
  for (; p < endp; p++) {
    if (*p == '\n') {
      curLine++;
    }
  }
}


int BaseLexer::tok(int t)
{
  updLoc();
  sval = NULL_SVAL;     // catch mistaken uses of 'sval' for single-spelling tokens
  return t;
}


void BaseLexer::err(char const *msg)
{
  errors++;
  std::cerr << toString(loc) << ": error: " << msg << std::endl;
}


void BaseLexer::warning(char const *msg)
{
  warnings++;
  std::cerr << toString(loc) << ": warning: " << msg << std::endl;
}


STATICDEF void BaseLexer::tokenFunc(LexerInterface *lex)
{
  BaseLexer *ths = static_cast<BaseLexer*>(lex);

  // call into the flex lexer; this updates 'loc' and sets
  // 'sval' as appropriate
  ths->type = ths->yylex();
}


BaseLexer::NextTokenFunc BaseLexer::getTokenFunc() const
{
  return &BaseLexer::tokenFunc;
}


// EOF
