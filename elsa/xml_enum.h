// xml_enum.h            see license.txt for copyright and terms of use

#ifndef XML_ENUM_H
#define XML_ENUM_H

enum XmlToken {
  XTOK_EOF,

  // non-keyword name
  XTOK_NAME,

  // literals
  XTOK_INT_LITERAL,
  XTOK_FLOAT_LITERAL,
  XTOK_HEX_LITERAL,
  XTOK_STRING_LITERAL,

  // punctuation
  XTOK_LESSTHAN,                // "<"
  XTOK_GREATERTHAN,             // ">"
  XTOK_EQUAL,                   // "="
  XTOK_SLASH,                   // "/"

  // special attributes
  XTOK_DOT_ID,                  // "_id"

#include "xml_enum_1.gen.h"

  // dummy terminals
  NUM_XML_TOKEN_TYPES,

};  // enum TokenType

#endif // XML_ENUM_H
