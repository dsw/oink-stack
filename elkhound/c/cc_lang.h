// cc_lang.h            see license.txt for copyright and terms of use
// language options that the parser (etc.) is sensitive to

#ifndef CCLANG_H
#define CCLANG_H

class CCLang {
public:
  // when this is true, and the parser sees "struct Foo { ... }",
  // it will pretend it also saw "typedef struct Foo Foo;" -- i.e.,
  // the structure (or class) tag name is treated as a type name
  // by itself
  bool tagsAreTypes;

  // when true, recognize C++ keywords in input stream
  bool recognizeCppKeywords;

public:
  CCLang() { ANSI_C(); }

  void ANSI_C();            // settings for ANSI C
  void ANSI_Cplusplus();    // settings for ANSI C++
};

#endif // CCLANG_H
