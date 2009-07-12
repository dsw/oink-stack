// cparse.h            see license.txt for copyright and terms of use
// data structures used during parsing of C code

#ifndef CPARSE_H
#define CPARSE_H

#include "strhash.h"       // StringHash
#include "strtable.h"      // StringTable
#include "objlist.h"       // ObjList
#include "c.ast.gen.h"     // C AST, for action function signatures

class CCLang;

// parsing action state
class ParseEnv {
public:
  StringTable &str;               // string table
  StringRef intType;              // "int"
  StringRef strRefAttr;           // "attr"
  ObjList<StringHash> types;      // stack of hashes which identify names of types
  CCLang &lang;                   // language options

public:
  ParseEnv(StringTable &table, CCLang &lang);
  ~ParseEnv();

  void enterScope();
  void leaveScope();
  void addType(StringRef type);
  bool isType(StringRef name);

  void declareClassTag(StringRef tagName);
};

#endif // CPARSE_H
