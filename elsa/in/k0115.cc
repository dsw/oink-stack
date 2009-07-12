// using own member function

// i/xalan_1.8-4/ElemApplyTemplates.cpp.8c892e8b0da809df12dd69739b9228a7.ii:28377:2:
// error: duplicate member declaration of `transformChild' in class
// xalanc_1_8::ElemApplyTemplates; previous at
// i/xalan_1.8-4/ElemApplyTemplates.cpp.8c892e8b0da809df12dd69739b9228a7.ii:28370:2

// ERR-MATCH: duplicate member declaration of `.*?' in class

struct S {
  void foo( );
  using S::foo;
};
