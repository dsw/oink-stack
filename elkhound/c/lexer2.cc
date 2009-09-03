// lexer2.cc            see license.txt for copyright and terms of use
// code for lexer2.h

#include "lexer2.h"      // this module
#include "trace.h"       // tracingSys
#include "strutil.h"     // encodeWithEscapes
#include "exc.h"         // xformat
#include "cc_lang.h"     // CCLang
#include "glrconfig.h"   // SOURCELOC

#include <stdlib.h>      // strtoul
#include <string.h>      // strlen, strcmp

// ------------------ token type descriptions ----------------------
struct Lexer2TokenTypeDesc
{
  // type code; should equal index in l2TokTypes[] (but not checked)
  Lexer2TokenType tokType;

  // name as it appears in C code
  char const *typeName;

  // spelling for keywords and operators; descriptive name for others
  char const *spelling;

  // true for tokens where we emit spelling as Bison alias; also, this
  // must be true for us to consider 'spelling' to mean a keyword or
  // operator spelling as opposed to an L2_NAME
  bool bisonSpelling;
};

// name it once, get both the symbol and the string
#define N(name) name, #name

Lexer2TokenTypeDesc const l2TokTypes[] = {
  // eof
  { N(L2_EOF),                  "EOF", false },

  // name
  { N(L2_NAME),                 "NAME", false },
  { N(L2_TYPE_NAME),            "TYPE_NAME", false },
  { N(L2_VARIABLE_NAME),        "VARIABLE_NAME", false },

  // literals
  { N(L2_INT_LITERAL),          "INT_LITERAL", false },
  { N(L2_FLOAT_LITERAL),        "FLOAT_LITERAL", false },
  { N(L2_STRING_LITERAL),       "STRING_LITERAL", false },
  { N(L2_CHAR_LITERAL),         "CHAR_LITERAL", false },

  // dsw: user-defined qualifiers
  // FIX: false means what?
  { N(L2_UDEF_QUAL),            "UDEF_QUAL", false },

  // keywords
  { N(L2_ASM),                  "asm", true },
  { N(L2_AUTO),                 "auto", true },
  { N(L2_BREAK),                "break", true },
  { N(L2_BOOL),                 "bool", true },
  { N(L2_CASE),                 "case", true },
  { N(L2_CATCH),                "catch", true },
  { N(L2_CDECL),                "cdecl", true },
  { N(L2_CHAR),                 "char", true },
  { N(L2_CLASS),                "class", true },
  { N(L2_CONST),                "const", true },
  { N(L2_CONST_CAST),           "const_cast", true },
  { N(L2_CONTINUE),             "continue", true },
  { N(L2_DEFAULT),              "default", true },
  { N(L2_DELETE),               "delete", true },
  { N(L2_DO),                   "do", true },
  { N(L2_DOUBLE),               "double", true },
  { N(L2_DYNAMIC_CAST),         "dynamic_cast", true },
  { N(L2_ELSE),                 "else", true },
  { N(L2_ENUM),                 "enum", true },
  { N(L2_EXPLICIT),             "explicit", true },
  { N(L2_EXPORT),               "export", true },
  { N(L2_EXTERN),               "extern", true },
  { N(L2_FALSE),                "false", true },
  { N(L2_FLOAT),                "float", true },
  { N(L2_FOR),                  "for", true },
  { N(L2_FRIEND),               "friend", true },
  { N(L2_GOTO),                 "goto", true },
  { N(L2_IF),                   "if", true },
  { N(L2_INLINE),               "inline", true },
  { N(L2_INT),                  "int", true },
  { N(L2_LONG),                 "long", true },
  { N(L2_MUTABLE),              "mutable", true },
  { N(L2_NAMESPACE),            "namespace", true },
  { N(L2_NEW),                  "new", true },
  { N(L2_OPERATOR),             "operator", true },
  { N(L2_PASCAL),               "pascal", true },
  { N(L2_PRIVATE),              "private", true },
  { N(L2_PROTECTED),            "protected", true },
  { N(L2_PUBLIC),               "public", true },
  { N(L2_REGISTER),             "register", true },
  { N(L2_REINTERPRET_CAST),     "reinterpret_cast", true },
  { N(L2_RETURN),               "return", true },
  { N(L2_SHORT),                "short", true },
  { N(L2_SIGNED),               "signed", true },
  { N(L2_SIZEOF),               "sizeof", true },
  { N(L2_STATIC),               "static", true },
  { N(L2_STATIC_CAST),          "static_cast", true },
  { N(L2_STRUCT),               "struct", true },
  { N(L2_SWITCH),               "switch", true },
  { N(L2_TEMPLATE),             "template", true },
  { N(L2_THIS),                 "this", true },
  { N(L2_THROW),                "throw", true },
  { N(L2_TRUE),                 "true", true },
  { N(L2_TRY),                  "try", true },
  { N(L2_TYPEDEF),              "typedef", true },
  { N(L2_TYPEID),               "typeid", true },
  { N(L2_TYPENAME),             "typename", true },
  { N(L2_UNION),                "union", true },
  { N(L2_UNSIGNED),             "unsigned", true },
  { N(L2_USING),                "using", true },
  { N(L2_VIRTUAL),              "virtual", true },
  { N(L2_VOID),                 "void", true },
  { N(L2_VOLATILE),             "volatile", true },
  { N(L2_WCHAR_T),              "wchar_t", true },
  { N(L2_WHILE),                "while", true },
  
  // operators
  { N(L2_LPAREN),               "(", true },
  { N(L2_RPAREN),               ")", true },
  { N(L2_LBRACKET),             "[", true },
  { N(L2_RBRACKET),             "]", true },
  { N(L2_ARROW),                "->", true },
  { N(L2_COLONCOLON),           "::", true },
  { N(L2_DOT),                  ".", true },
  { N(L2_BANG),                 "!", true },
  { N(L2_TILDE),                "~", true },
  { N(L2_PLUS),                 "+", true },
  { N(L2_MINUS),                "-", true },
  { N(L2_PLUSPLUS),             "++", true },
  { N(L2_MINUSMINUS),           "--", true },
  { N(L2_AND),                  "&", true },
  { N(L2_STAR),                 "*", true },
  { N(L2_DOTSTAR),              ".*", true },
  { N(L2_ARROWSTAR),            "->*", true },
  { N(L2_SLASH),                "/", true },
  { N(L2_PERCENT),              "%", true },
  { N(L2_LEFTSHIFT),            "<<", true },
  { N(L2_RIGHTSHIFT),           ">>", true },
  { N(L2_LESSTHAN),             "<", true },
  { N(L2_LESSEQ),               "<=", true },
  { N(L2_GREATERTHAN),          ">", true },
  { N(L2_GREATEREQ),            ">=", true },
  { N(L2_EQUALEQUAL),           "==", true },
  { N(L2_NOTEQUAL),             "!=", true },
  { N(L2_XOR),                  "^", true },
  { N(L2_OR),                   "|", true },
  { N(L2_ANDAND),               "&&", true },
  { N(L2_OROR),                 "||", true },
  { N(L2_QUESTION),             "?", true },
  { N(L2_COLON),                ":", true },
  { N(L2_EQUAL),                "=", true },
  { N(L2_STAREQUAL),            "*=", true },
  { N(L2_SLASHEQUAL),           "/=", true },
  { N(L2_PERCENTEQUAL),         "%=", true },
  { N(L2_PLUSEQUAL),            "+=", true },
  { N(L2_MINUSEQUAL),           "-=", true },
  { N(L2_ANDEQUAL),             "&=", true },
  { N(L2_XOREQUAL),             "^=", true },
  { N(L2_OREQUAL),              "|=", true },
  { N(L2_LEFTSHIFTEQUAL),       "<<=", true },
  { N(L2_RIGHTSHIFTEQUAL),      ">>=", true },
  { N(L2_COMMA),                ",", true },
  { N(L2_ELLIPSIS),             "...", true },
  { N(L2_SEMICOLON),            ";", true },
  { N(L2_LBRACE),               "{", true },
  { N(L2_RBRACE),               "}", true },
  
  // extensions for parsing gnu
  { N(L2___ATTRIBUTE__),        "__attribute__", true },
  { N(L2___FUNCTION__),         "__FUNCTION__", true },
  { N(L2___LABEL__),            "__label__", true },
  { N(L2___PRETTY_FUNCTION__),  "__PRETTY_FUNCTION__", true },
  { N(L2___TYPEOF__),           "__typeof__", true },
  
  // my own extension
  { N(L2_OWNER),                "owner_ptr_qualifier", true },
  
  // additional tokens to help in specifying disambiguation
  { N(L2_PREFER_REDUCE),        "PREFER_REDUCE", true },
  { N(L2_PREFER_SHIFT),         "PREFER_SHIFT", true },
                                                           
  // theorem-prover extensions
  { N(L2_THMPRV_ASSERT),        "thmprv_assert", true },
  { N(L2_THMPRV_ASSUME),        "thmprv_assume", true },
  { N(L2_THMPRV_INVARIANT),     "thmprv_invariant", true },
  { N(L2_IMPLIES),              "==>", true },
  { N(L2_THMPRV_PRE),           "thmprv_pre", true },
  { N(L2_THMPRV_POST),          "thmprv_post", true },
  { N(L2_THMPRV_LET),           "thmprv_let", true },
  { N(L2_THMPRV_ATTR),          "thmprv_attr", true },
  { N(L2_THMPRV_FORALL),        "thmprv_forall", true },
  { N(L2_THMPRV_EXISTS),        "thmprv_exists", true },
  { N(L2_THMPRV_PURE_ASSERT),   "thmprv_pure_assert", true },
  { N(L2_THMPRV_BIND),          "thmprv_bind", true },
  { N(L2_THMPRV_DECL),          "thmprv_decl", true },
  { N(L2_THMPRV_PREDICATE),     "thmprv_predicate", true },
};

#undef N


struct KeywordMap {
  char const *spelling;     // token source value
  Lexer2TokenType tokType;  // destination classification
};

// map of GNU keywords into ANSI keywords (I don't know and
// don't care about whatever differences there may be)
KeywordMap gnuKeywordMap[] = {
  { "__asm__", L2_ASM },
  { "__const__", L2_CONST },
  { "__label__", L2___LABEL__ },
  { "__inline__", L2_INLINE },
  { "__signed__", L2_SIGNED },
  { "__volatile__", L2_VOLATILE },
  { "__FUNCTION__", L2___FUNCTION__ },
  { "__PRETTY_FUNCTION__", L2___PRETTY_FUNCTION__ },

  // ones that GNU c has
  //{ "inline", L2_NAME },
};

// make C++ keywords not appear to be keywords; need to make this
// dependent on some flag somewhere ..
KeywordMap c_KeywordMap[] = {
  { "bool", L2_NAME },
  { "catch", L2_NAME },
  { "class", L2_NAME },
  { "complex", L2_NAME },
  { "const_cast", L2_NAME },
  { "delete", L2_NAME },
  { "dynamic_cast", L2_NAME },
  { "explicit", L2_NAME },
  { "export", L2_NAME },
  { "friend", L2_NAME },
  { "mutable", L2_NAME },
  { "namespace", L2_NAME },
  { "new", L2_NAME },     // back to recognizing 'new'
  { "operator", L2_NAME },
  { "private", L2_NAME },
  { "protected", L2_NAME },
  { "public", L2_NAME },
  { "reinterpret_cast", L2_NAME },
  { "static_cast", L2_NAME },
  { "template", L2_NAME },
  { "this", L2_NAME },
  { "throw", L2_NAME },
  { "try", L2_NAME },
  { "typename", L2_NAME },
  { "using", L2_NAME },
  { "virtual", L2_NAME },
};


Lexer2TokenType lookupKeyword(CCLang &lang, rostring keyword)
{
  // works?
  STATIC_ASSERT(TABLESIZE(l2TokTypes) == L2_NUM_TYPES);

  xassert(TABLESIZE(l2TokTypes) == L2_NUM_TYPES);

  {loopi(TABLESIZE(gnuKeywordMap)) {
    if (0==strcmp(gnuKeywordMap[i].spelling, keyword)) {
      return gnuKeywordMap[i].tokType;
    }
  }}

  if (!lang.recognizeCppKeywords) {
    {loopi(TABLESIZE(c_KeywordMap)) {
      if (0==strcmp(c_KeywordMap[i].spelling, keyword)) {
        return c_KeywordMap[i].tokType;
      }
    }}
  }

  {loopi(L2_NUM_TYPES) {
    if (l2TokTypes[i].bisonSpelling &&
        0==strcmp(l2TokTypes[i].spelling, keyword)) {
      return l2TokTypes[i].tokType;
    }
  }}

  return L2_NAME;     // not found, so is user-defined name
}

char const *l2Tok2String(Lexer2TokenType type)
{
  xassert(TABLESIZE(l2TokTypes) == L2_NUM_TYPES);
  xassert(type < L2_NUM_TYPES);

  return l2TokTypes[type].spelling;
}

char const *l2Tok2SexpString(Lexer2TokenType type)
{
  xassert(TABLESIZE(l2TokTypes) == L2_NUM_TYPES);
  xassert(type < L2_NUM_TYPES);

  // for sexps let's use the L2_XXX names; among other things,
  // that will prevent syntactic parentheses from being interpreted
  // by the sexp reader (which probably wouldn't be all that
  // terrible, but not what I want...)
  return l2TokTypes[type].typeName;
}


// list of "%token" declarations for Bison
void printBisonTokenDecls(bool spellings)
{
  loopi(L2_NUM_TYPES) {
    //if (i == L2_EOF) {
    //  continue;    // bison doesn't like me to define a token with code 0
    //}
    // but: now I'm using these for my own parser, and I want the 0

    Lexer2TokenTypeDesc const &desc = l2TokTypes[i];
    xassert(desc.tokType == i);    // check correspondence between index and tokType

    printf("%%token %-20s %3d   ", desc.typeName, desc.tokType);
    if (spellings && desc.bisonSpelling) {
      printf("\"%s\"\n", desc.spelling);       // string token
    }
    else {
      printf("\n");                            // no spelling.. testing..
    }
  }
}


// list of token declarations for my parser
void printMyTokenDecls()
{
  printf("// this list automatically generated by lexer2\n"
         "\n"
         "// form:\n"
         "//   <code> : <name> [<alias>] ;\n"
         "\n");

  loopi(L2_NUM_TYPES) {
    Lexer2TokenTypeDesc const &desc = l2TokTypes[i];
    xassert(desc.tokType == i);    // check correspondence between index and tokType

    printf("  %3d : %-20s ", desc.tokType, desc.typeName);
    if (desc.bisonSpelling) {
      printf("\"%s\" ;\n", desc.spelling);       // string token alias
    }
    else {
      printf(";\n");                             // no alias
    }
  }
}


// ----------------------- Lexer2Token -------------------------------
Lexer2Token::Lexer2Token(Lexer2TokenType aType, SourceLoc aLoc)
  : type(aType),
    intValue(0),     // legal? apparently..
    loc(aLoc),
    sourceMacro(NULL)
{}

Lexer2Token::~Lexer2Token()
{}


string Lexer2Token::toString(bool asSexp) const
{
  return toStringType(asSexp, type);
}

string Lexer2Token::toStringType(bool asSexp, Lexer2TokenType type) const
{
  string tokType;
  if (!asSexp) {
    tokType = l2Tok2String(type);
  }
  else {
    tokType = l2Tok2SexpString(type);
  }

  // get the literal value, if any
  string litVal;
  switch (type) {
    case L2_NAME:
    case L2_TYPE_NAME:
    case L2_VARIABLE_NAME:
    case L2_STRING_LITERAL:
      litVal = stringc << strValue;
      break;

    case L2_INT_LITERAL:
      litVal = stringc << intValue;
      break;

    default:
      return tokType;
  }

  if (!asSexp) {
    return stringc << tokType << "(" << litVal << ")";
  }
  else {
    return stringc << "(" << tokType << " " << litVal << ")";
  }
}


string Lexer2Token::unparseString() const
{
  switch (type) {
    case L2_NAME:
      return string(strValue);

    case L2_STRING_LITERAL:
      return stringc << quoted(strValue);

    case L2_INT_LITERAL:
      return stringc << intValue;

    default:
      return l2Tok2String(type);     // operators and keywords
  }
}


void Lexer2Token::print() const
{
  printf("[L2] Token at %s: %s\n",
         toString(loc).c_str(), toString().c_str());
}


void quotedUnescape(ArrayStack<char> &dest, rostring src,
                    char delim, bool allowNewlines)
{
  // strip quotes or ticks
  decodeEscapes(dest, substring(toCStr(src)+1, strlen(src)-2),
                delim, allowNewlines);
}


// ------------------------- lexer 2 itself ----------------------------
// jobs performed:
//  - distinctions are drawn, e.g. keywords and operators
//  - whitespace and comments are stripped
//  - meaning is extracted, e.g. for integer literals
// not performed yet:
//  - preprocessor actions are performed: inclusion and macro expansion
void lexer2_lex(Lexer2 &dest, Lexer1 const &src, char const *fname)
{
  // keep track of previous L2 token emitted so we can do token
  // collapsing for string juxtaposition
  Lexer2Token *prevToken = NULL;

  #ifndef USE_RECLASSIFY
    #define USE_RECLASSIFY 1    // if nobody tells me, it's probably enabled
  #endif
  bool const yieldVarName = !USE_RECLASSIFY || tracingSys("yieldVariableName");
  bool const debugLexer2 = tracingSys("lexer2");

  // iterate over all the L1 tokens
  ObjListIter<Lexer1Token> L1_iter(src.tokens);
  for (; !L1_iter.isDone(); L1_iter.adv()) {
    // convenient renaming
    Lexer1Token const *L1 = L1_iter.data();

    if (L1->type == L1_PREPROCESSOR ||     // for now
        L1->type == L1_WHITESPACE   ||
        L1->type == L1_COMMENT      ||
        L1->type == L1_ILLEGAL) {
      continue;    // filter it out entirely
    }

    if (L1->type == L1_STRING_LITERAL         &&
        prevToken != NULL                     &&
        prevToken->type == L2_STRING_LITERAL) {
      // coalesce adjacent strings (this is not efficient code..)
      stringBuilder sb;
      sb << prevToken->strValue;

      ArrayStack<char> tempString;
      quotedUnescape(tempString, L1->text, '"',
                     src.allowMultilineStrings);
      sb.append(tempString.getArray(), tempString.length());

      prevToken->strValue = dest.idTable.add(sb);
      continue;
    }

    // create the object for the yielded token; don't know the type
    // yet at this point, so I use L2_NAME as a placeholder
    Lexer2Token *L2 =
      new Lexer2Token(L2_NAME, L1->loc);

    try {
      switch (L1->type) {
        case L1_IDENTIFIER:                                   
          // get either keyword's type, or L2_NAME
          L2->type = lookupKeyword(dest.lang, L1->text);
          if (L2->type == L2_NAME) {
            // save name's text
            L2->strValue = dest.idTable.add(L1->text.c_str());
          }
          break;

        case L1_INT_LITERAL:
          L2->type = L2_INT_LITERAL;
          L2->intValue = strtoul(L1->text.c_str(), NULL /*endptr*/, 0 /*radix*/);
          break;

        case L1_FLOAT_LITERAL:
          L2->type = L2_FLOAT_LITERAL;
          L2->floatValue = new float(atof(L1->text.c_str()));
          break;

        case L1_STRING_LITERAL: {
          L2->type = L2_STRING_LITERAL;

          char const *srcText = L1->text.c_str();
          if (*srcText == 'L') srcText++;

          ArrayStack<char> tmp;
          quotedUnescape(tmp, srcText, '"',
                         src.allowMultilineStrings);

          for (int i=0; i<tmp.length(); i++) {
            if (tmp[i]==0) {
              std::cout << "warning: literal string with embedded nulls not handled properly\n";
              break;
            }
          }

          L2->strValue = dest.idTable.add(tmp.getArray());
          break;
        }

        case L1_UDEF_QUAL: {
          L2->type = L2_UDEF_QUAL;
          L2->strValue = dest.idTable.add(L1->text.c_str());
          break;
        }

        case L1_CHAR_LITERAL: {
          L2->type = L2_CHAR_LITERAL;

          char const *srcText = L1->text.c_str();
          if (*srcText == 'L') srcText++;

          ArrayStack<char> tmp;
          quotedUnescape(tmp, srcText, '\'',
                         false /*allowNewlines*/);

          if (tmp.length() != 1) {
            xformat("character literal must have 1 char");
          }

          L2->charValue = tmp[0];
          break;
        }

        case L1_OPERATOR:
          L2->type = lookupKeyword(dest.lang, L1->text);      // operator's type
          xassert(L2->type != L2_NAME);            // otherwise invalid operator text..
          break;

        default:
          xfailure("unknown L1 type");
      }
    }
    catch (xFormat &x) {
      std::cout << toString(L1->loc) << ": " << x.cond() << std::endl;
      continue;
    }

    // for testing the performance of the C parser against Bison, I
    // want to disable the reclassifier, so I need to yield
    // L2_VARIABLE_NAME directly
    if (yieldVarName && (L2->type == L2_NAME)) {
      L2->type = L2_VARIABLE_NAME;
    }

    // append this token to the running list
    dest.addToken(L2);
    prevToken = L2;

    // (debugging) print it
    if (debugLexer2) {
      L2->print();
    }
  }

  // final token
  dest.addEOFToken();
}


// --------------------- Lexer2 ------------------
Lexer2::Lexer2(CCLang &L)
  : myIdTable(new StringTable()),
    lang(L),
    idTable(*myIdTable),      // hope this works..
    tokens(),
    tokensMut(tokens),
    currentToken(tokens)
{
  init();
}

Lexer2::Lexer2(CCLang &L, StringTable &extTable)
  : myIdTable(NULL),
    lang(L),
    idTable(extTable),
    tokens(),
    tokensMut(tokens),
    currentToken(tokens)
{
  init();
}

void Lexer2::init()
{
  // init for predictable behavior; esp. for 'loc', which won't
  // be further updated if source loc info is off
  type = 0;
  sval = 0;
  loc = SL_UNKNOWN;
}

Lexer2::~Lexer2()
{
  if (myIdTable) {
    delete myIdTable;
  }
}


SourceLoc Lexer2::startLoc() const
{
  if (tokens.isNotEmpty()) {
    return tokens.firstC()->loc;
  }
  else {
    return SL_UNKNOWN;
  }
}


inline void Lexer2::copyFields()
{
  type = currentToken.data()->type;
  sval = currentToken.data()->sval;
  SOURCELOC( loc = currentToken.data()->loc; )
}


void Lexer2::beginReading()
{
  currentToken.reset(tokens);
  copyFields();
}


STATICDEF void Lexer2::nextToken(Lexer2 *ths)
{
  ths->currentToken.adv();
  ths->copyFields();
}
       
LexerInterface::NextTokenFunc Lexer2::getTokenFunc() const
{
  return (NextTokenFunc)&Lexer2::nextToken;
}


string Lexer2::tokenDesc() const
{
  return currentToken.data()->toStringType(false /*asSexp*/,
                                           (Lexer2TokenType)LexerInterface::type);
}


string Lexer2::tokenKindDesc(int kind) const
{                                            
  return l2Tok2String((Lexer2TokenType)kind);
}


// ------------- experimental interface for (e.g.) bison ------------
// this is defined by the bison-parser
extern Lexer2Token const *yylval;

char const *bison_hack_source_fname = NULL;

// returns token types until EOF, at which point L2_EOF is returned
Lexer2TokenType lexer2_gettoken()
{
  static Lexer1 *lexer1 = NULL;
  static Lexer2 *lexer2 = NULL;
  static ObjListIter<Lexer2Token> *iter = NULL;

  if (!lexer1) {
    // do first phase
    lexer1 = new Lexer1(bison_hack_source_fname);
    FILE *fp = fopen(bison_hack_source_fname, "r");
    if (!fp) {
      throw_XOpen(bison_hack_source_fname);
    }
    lexer1_lex(*lexer1, fp);

    if (lexer1->errors > 0) {
      printf("%d error(s)\n", lexer1->errors);
      //return L2_EOF;   // done
    }

    // do second phase
    lexer2 = new Lexer2(*new CCLang);
    lexer2_lex(*lexer2, *lexer1, "<stdin>");

    // prepare to return tokens
    iter = new ObjListIter<Lexer2Token>(lexer2->tokens);
  }

  if (!iter->isDone()) {
    // grab type to return
    yylval = iter->data();
    Lexer2TokenType ret = iter->data()->type;

    // advance to next token
    iter->adv();

    // return one we just advanced past
    return ret;
  }
  else {
    // done; don't bother freeing things
    yylval = NULL;
    return L2_EOF;
  }
}


// ----------------------- testing --------------------
#ifdef TEST_LEXER2

// no bison-parser present, so define it myself
Lexer2Token const *yylval = NULL;

int main(int argc, char **argv)
{
  SourceLocManager mgr;

  if (argc > 1 && 0==strcmp(argv[1], "-bison")) {
    printBisonTokenDecls(true /*spellings*/);
    return 0;
  }

  if (argc > 1 && 0==strcmp(argv[1], "-myparser")) {
    printMyTokenDecls();
    return 0;
  }

  if (argc < 2) {
    printf("usage: %s <file>\n", argv[0]);
    return 0;
  }
  bison_hack_source_fname = argv[1];

  while (lexer2_gettoken() != L2_EOF) {
    yylval->print();
  }

  return 0;
}

#endif // TEST_LEXER2
