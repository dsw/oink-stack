// xmlhelp.cc
// support routines for XML reading/writing

#include "xmlhelp.h"            // this module
#include <stdlib.h>             // atof, atol
#include <ctype.h>              // isprint, isdigit, isxdigit
#include <stdio.h>              // sprintf
// #include "strtokp.h"            // StrtokParse
#include "exc.h"                // xformat
#include "ptrintmap.h"          // PtrIntMap


// FIX: pull this out into the configuration script
#define CANONICAL_XML_IDS

#ifdef CANONICAL_XML_IDS
xmlUniqueId_t nextXmlUniqueId = 1;
PtrIntMap<void const, xmlUniqueId_t> addr2id;
#endif

xmlUniqueId_t mapAddrToUniqueId(void const * const addr) {
#ifdef CANONICAL_XML_IDS
  // special-case the NULL pointer
  if (addr == NULL) return 0;
  // otherwise, maintain a map to a canonical address
  xmlUniqueId_t id0 = addr2id.get(addr);
  if (!id0) {
    id0 = nextXmlUniqueId++;
    addr2id.add(addr, id0);
  }
  return id0;
#else
  // avoid using the map
  return reinterpret_cast<xmlUniqueId_t>(addr);
#endif
}

// manage identity of AST nodes
xmlUniqueId_t uniqueIdAST(void const * const obj) {
  return mapAddrToUniqueId(obj);
}

// string xmlPrintPointer(char const *label, xmlUniqueId_t id) {
//   stringBuilder sb;
//   sb.reserve(20);
//   if (!id) {
//     // sm: previously, the code for this function just inserted 'p'
//     // as a 'void const *', but that is nonportable, as gcc-3 inserts
//     // "0" while gcc-2 emits "(nil)"
//     sb << label << "0";
//   }
//   else {
//     sb << label;
//     // sm: I question whether this is portable, but it may not matter
//     // since null pointers are the only ones that are treated
//     // specially (as far as I can tell)
// //     sb << stringBuilder::Hex(reinterpret_cast<long unsigned>(p));
//     // dsw: just using ints now
//     sb << id;
//   }
//   return sb;
// }

// string toXml_bool(bool b) {
//   if (b) return "true";
//   else return "false";
// }


// string toXml_int(int i) {
//   return stringc << i;
// }

void fromXml_int(int &i, const char *str) {
  long i0 = strtol(str, NULL, 10);
  i = i0;
}


// string toXml_long(long i) {
//   return stringc << i;
// }

void fromXml_long(long &i, const char *str) {
  long i0 = strtol(str, NULL, 10);
  i = i0;
}


// string toXml_unsigned_int(unsigned int i) {
//   return stringc << i;
// }

void fromXml_unsigned_int(unsigned int &i, const char *str) {
  unsigned long i0 = strtoul(str, NULL, 10);
  i = i0;
}


// string toXml_unsigned_long(unsigned long i) {
//   return stringc << i;
// }

void fromXml_unsigned_long(unsigned long &i, const char *str) {
  unsigned long i0 = strtoul(str, NULL, 10);
  i = i0;
}


// string toXml_double(double x) {
//   return stringc << x;
// }

void fromXml_double(double &x, const char *str) {
  x = atof(str);
}


string toXml_SourceLoc(SourceLoc loc) {
  // use "(noloc)" and "(init)" so we don't have to encode to &lt;init&gt;
  if (loc == SL_UNKNOWN) {
    return "(noloc)";
  } else if (loc == SL_INIT) {
    return "(init)";
  } else {
    // NOTE: the nohashline here is very important; never change it
    return sourceLocManager->getString_nohashline(loc);
  }
}

// Avoid allocating memory to construct a substring, by being sneaky.  This
// isn't thread safe.  When put on the stack, the optimizer should only end up
// using one word, for 'save'.
class SneakySubstring {
public:
  SneakySubstring(const char *begin0, const char *end0)
    : begin(begin0), end(end0), save(*end0)
  {
    const_cast<char*>(end) [0] = '\0';
  }

  ~SneakySubstring() {
    const_cast<char*>(end) [0] = save;
  }

  operator const char *() const {
    return begin;
  }

  const char *begin;
  const char *end;
  char save;
};


// #define string_prefix_match(s, prefix) (0==strncmp(s, prefix, sizeof(prefix)-1))

// Note: this function is performance-critical for deserialization, so don't
// use StrtokParse.
void fromXml_SourceLoc(SourceLoc &loc, const char *str) {
  // the file format is filename:line:column

  if (streq(str, "(noloc)")) {
    loc = SL_UNKNOWN;
    return;
  }

  if (streq(str, "(init)")) {
    loc = SL_INIT;
    return;
  }

  char const *end = str + strlen(str);

  int line;
  int col;

  while (true) {
    if (end <= str) {
      // FIX: this is a parsing error but I don't want to throw an
      // exception out of this library function
      loc = SL_UNKNOWN;
      return;
    }

    end--;
    if (*end == ':') {
      col = atoi(end+1);
      if (!col) {
        loc = SL_UNKNOWN;
        return;
      }
      break;
    }

    if (!isdigit(*end)) {
      loc = SL_UNKNOWN;
      return;
    }
  }

  while (true) {
    if (end <= str) {
      loc = SL_UNKNOWN;
      return;
    }

    end--;
    if (*end == ':') {
      line = atoi(end+1);
      if (!line) {
        loc = SL_UNKNOWN;
        return;
      }
      break;
    }

    if (!isdigit(*end)) {
      loc = SL_UNKNOWN;
      return;
    }
  }

  if (end <= str) {
    loc = SL_UNKNOWN;
    return;
  }

  // the substring (str, end] is the filename.
  SneakySubstring file(str, end);

  loc = sourceLocManager->encodeLineCol(file, line, col);
}

// named escape codes
#define lt_CODE "lt;"
#define gt_CODE "gt;"
#define amp_CODE "amp;"
#define quot_CODE "quot;"
#define apos_CODE "apos;"
// int const lt_codelen   = strlen(lt_CODE);
// int const gt_codelen   = strlen(gt_CODE);
// int const amp_codelen  = strlen(amp_CODE);
// int const quot_codelen = strlen(quot_CODE);

// Output SRC with escaping and quotes to output stream directly.  This is
// more efficient than constructing strings and then outputting that.
ostream &outputXmlAttrQuoted(ostream &o, const char *src)
{
  o << '\'';

  for (; *src; ++src) {
    unsigned char c = *src;

    // escape those special to xml attributes;
    // http://www.w3.org/TR/2004/REC-xml-20040204/#NT-AttValue
    switch (c) {
    default: break;             // try below
    case '<': o << "&" lt_CODE;   continue;
    case '>': o << "&" gt_CODE;   continue; // this one not strictly required here
    case '&': o << "&" amp_CODE;  continue;
      // Don't need to escape '"' if surrounding quote is "'"
    // case '"': o << "&" quot_CODE; continue;
    case '\'': o << "&" apos_CODE; continue;
    }

    // try itself
    if (isprint(c)) {
      o << c;
      continue;
    }

    // use the most general notation
    char tmp[7];
    // dsw: the sillyness of XML knows no bounds: it is actually more
    // efficient to use the decimal encoding since sometimes you only
    // need 4 or 5 characters, whereas with hex you are guaranteed to
    // need 6, however the uniformity makes it easier to decode hex.
    // Why not make the shorter encoding also the default so that it
    // really is shorter in the big picture?
    sprintf(tmp, "&#x%02X;", c);
    o << tmp;
  }

  return o << '\'';
}

string xmlAttrEncode(char const *p, int len) {
  stringBuilder sb;
  sb.reserve(len*3/2);

  for(int i=0; i<len; ++i) {
    unsigned char c = p[i];

    // escape those special to xml attributes;
    // http://www.w3.org/TR/2004/REC-xml-20040204/#NT-AttValue
    switch (c) {
    default: break;             // try below
    case '<': sb << "&" lt_CODE;   continue;
    case '>': sb << "&" gt_CODE;   continue; // this one not strictly required here
    case '&': sb << "&" amp_CODE;  continue;
    case '"': sb << "&" quot_CODE; continue;
    case '\'': sb << "&" apos_CODE; continue;
    }

    // try itself
    if (isprint(c)) {
      sb << c;
      continue;
    }

    // use the most general notation
    char tmp[7];
    // dsw: the sillyness of XML knows no bounds: it is actually more
    // efficient to use the decimal encoding since sometimes you only
    // need 4 or 5 characters, whereas with hex you are guaranteed to
    // need 6, however the uniformity makes it easier to decode hex.
    // Why not make the shorter encoding also the default so that it
    // really is shorter in the big picture?
    sprintf(tmp, "&#x%02X;", c);
    sb << tmp;
  }

  return sb;
}

string xmlAttrEncode(const char *src) {
  return xmlAttrEncode(src, strlen(src));
}

string xmlAttrQuote(const char *src) {
  return stringc << '\''
                 << xmlAttrEncode(src)
                 << '\'';
}

// XML dequoting and unescaping is now done in the lexer: see
// xml_lex_extra.cc.


// string xmlAttrDeQuote(const char *text) {
//   int len = strlen(text);

//   if ( text[0] == '\'' && text[len-1] == '\'' ) {
//     // decode escapes
//     return xmlAttrDecode(text+1, text+len-1, '\'');
//   }

//   if ( text[0] == '"' && text[len-1] == '"' ) {
//     // decode escapes
//     return xmlAttrDecode(text+1, text+len-1, '"');
//   }

//   xformat(stringc << "quoted string is missing quotes: " << text);
// }

// // process characters between 'src' and 'end'.  The 'end' is so we don't have
// // to create a new string just to strip quotes.
// string xmlAttrDecode(char const *src, const char *end, char delim)
// {
//   stringBuilder result;
//   result.reserve(end-src);
//   while (src != end) {
//     // check for newlines
//     if (*src == '\n') {
//       xformat("unescaped newline (unterminated string)");
//     }

//     // check for the delimiter
//     if (*src == delim) {
//       xformat(stringc << "unescaped delimiter (" << delim << ") in "
//               << substring(src,end-src));
//     }

//     // check for normal characters
//     if (*src != '&') {
//       // normal character
//       result << char(*src);
//       src++;
//       continue;
//     }
//     src++;                      // advance past amperstand

//     // checked for named escape codes
// #define DO_ESCAPE(NAME, CHAR)
//     if (strncmp(NAME ##_CODE, src, (sizeof(NAME ##_CODE)-1)) == 0) {
//       result << char(CHAR);
//       src += (sizeof(NAME ##_CODE)-1);
//       continue;
//     }

//     DO_ESCAPE(lt,   '<');
//     DO_ESCAPE(gt,   '>');
//     DO_ESCAPE(amp,  '&');
//     DO_ESCAPE(quot, '"');
//     DO_ESCAPE(apos, '\'');
// #undef DO_ESCAPE

//     // check for numerical escapes
//     if (*src != '#') {
//       xformat(stringc << "use of an unimplemented or illegal amperstand escape (" << *src << ")");
//     }
//     ++src;

//     // process decimal and hex escapes: decimal '&#DDD;' where D is a
//     // decimal digit or hexadecimal '&#xHH;' where H is a hex digit.
//     if (!(*src == 'x' || isdigit(*src))) {
//       xformat(stringc << "illegal charcter after '&#' (" << *src << ")");
//     }

//     // are we doing hex or decimal processing?
//     //
//     // http://www.w3.org/TR/2004/REC-xml-20040204/#NT-CharRef "If the
//     // character reference begins with "&#x", the digits and letters
//     // up to the terminating ; provide a hexadecimal representation of
//     // the character's code point in ISO/IEC 10646. If it begins just
//     // with "&#", the digits up to the terminating ; provide a decimal
//     // representation of the character's code point."
//     bool hex = (*src == 'x');
//     if (hex) {
//       src++;

//       // strtoul is willing to skip leading whitespace, so I need
//       // to catch it myself
//       if (!isxdigit(*src)) {
//         // dsw: NOTE: in the non-hex case, the leading digit has
//         // already been seen
//         xformat("non-hex digit following '&#x' escape");
//       }
//       xassert(isxdigit(*src));
//     } else {
//       xassert(isdigit(*src));
//     }

//     // parse the digit
//     char const *endptr;
//     unsigned long val = strtoul(src, (char**)&endptr, hex? 16 : 10);
//     if (src == endptr) {
//       // this can't happen with the octal escapes because
//       // there is always at least one valid digit
//       xformat("invalid '&#' escape");
//     }

//     // keep it
//     result << ((char)(unsigned char)val);    // possible truncation..
//     src = endptr;
//   }
//   return result;
// }
